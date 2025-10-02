#include "ocharts/s57obj.h"
#include <cstdlib>
#include <cstring>

S57Obj::S57Obj() { Init(); }

S57Obj::S57Obj(const char* featureName) {
    Init();
    attVal = new wxArrayOfS57attVal();
    std::strncpy(FeatureName, featureName, 6);
    FeatureName[6] = 0;

    if (!std::strncmp(FeatureName, "DEPARE", 6) || !std::strncmp(FeatureName, "DRGARE", 6))
        bIsAssociable = true;
}

S57Obj::~S57Obj() {
    if (!bIsClone) {
        if (attVal) {
            for (unsigned int iv = 0; iv < attVal->GetCount(); iv++) {
                S57attVal* vv = attVal->Item(iv);
                if (vv) {
                    std::free(vv->value);
                    delete vv;
                }
            }
            delete attVal;
        }
        std::free(att_array);

        delete pPolyTessGeo;

        std::free(geoPt);
        std::free(geoPtz);
        std::free(geoPtMulti);

        std::free(m_lsindex_array);
    }
}

void S57Obj::Init() {
    att_array = nullptr;
    attVal = nullptr;
    n_attr = 0;
    pPolyTessGeo = nullptr;

    bIsClone = false;
    bIsAssociable = false;
    bIsAton = false;

    Scamin = 1e8 + 2;
    SuperScamin = -1;
    nRef = 0;

    m_lsindex_array = nullptr;
    m_n_lsindex = 0;
    m_n_edge_max_points = 0;

    x_rate = 1.0; y_rate = 1.0; x_origin = 0.0; y_origin = 0.0;
}

bool S57Obj::AddIntegerAttribute(const char* acronym, int val) {
    if (!attVal) attVal = new wxArrayOfS57attVal();

    S57attVal* v = new S57attVal;
    int* p = static_cast<int*>(std::malloc(sizeof(int)));
    *p = val;
    v->valType = OGR_INT;
    v->value = p;

    att_array = static_cast<char*>(std::realloc(att_array, 6 * (n_attr + 1)));
    std::strncpy(att_array + (6 * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(v);

    if (!std::strncmp(acronym, "SCAMIN", 6))
        Scamin = val;

    return true;
}

bool S57Obj::AddDoubleAttribute(const char* acronym, double val) {
    if (!attVal) attVal = new wxArrayOfS57attVal();

    S57attVal* v = new S57attVal;
    double* p = static_cast<double*>(std::malloc(sizeof(double)));
    *p = val;
    v->valType = OGR_REAL;
    v->value = p;

    att_array = static_cast<char*>(std::realloc(att_array, 6 * (n_attr + 1)));
    std::strncpy(att_array + (6 * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(v);
    return true;
}

bool S57Obj::AddStringAttribute(const char* acronym, char* val) {
    if (!attVal) attVal = new wxArrayOfS57attVal();

    S57attVal* v = new S57attVal;
    char* p = static_cast<char*>(std::malloc(std::strlen(val) + 1));
    std::strcpy(p, val);
    v->valType = OGR_STR;
    v->value = p;

    att_array = static_cast<char*>(std::realloc(att_array, 6 * (n_attr + 1)));
    std::strncpy(att_array + (6 * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(v);
    return true;
}

bool S57Obj::SetPointGeometry(double lat, double lon, double ref_lat, double ref_lon) {
    Primitive_type = GEO_POINT;

    m_lon = lon;
    m_lat = lat;

    double bound = 1.0 / 60.0; // ~1 NM
    BBObj.Set(m_lat - bound, m_lon - bound, m_lat + bound, m_lon + bound);
    bBBObj_valid = false;

    toSM(lat, lon, ref_lat, ref_lon, &x, &y);
    npt = 1;
    return true;
}

bool S57Obj::SetLineGeometry(LineGeometryDescriptor* pGeo, GeoPrim_t geoType,
                             double ref_lat, double ref_lon) {
    Primitive_type = geoType;

    BBObj.Set(pGeo->extent_s_lat, pGeo->extent_w_lon,
              pGeo->extent_n_lat, pGeo->extent_e_lon);
    bBBObj_valid = true;

    double e1, e2, n1, n2;
    toSM(pGeo->extent_n_lat, pGeo->extent_e_lon, ref_lat, ref_lon, &e1, &n1);
    toSM(pGeo->extent_s_lat, pGeo->extent_w_lon, ref_lat, ref_lon, &e2, &n2);
    x = (e1 + e2) / 2.0;
    y = (n1 + n2) / 2.0;

    double xll, yll;
    fromSM(x, y, ref_lat, ref_lon, &yll, &xll);
    m_lon = xll;
    m_lat = yll;

    m_n_lsindex = pGeo->indexCount;
    m_lsindex_array = pGeo->indexTable;
    m_n_edge_max_points = 0;

    return true;
}

bool S57Obj::SetAreaGeometry(PolyTessGeo* ppg, double ref_lat, double ref_lon) {
    Primitive_type = GEO_AREA;
    pPolyTessGeo = ppg;

    BBObj.Set(ppg->Get_ymin(), ppg->Get_xmin(), ppg->Get_ymax(), ppg->Get_xmax());
    bBBObj_valid = true;

    double e1, e2, n1, n2;
    toSM(ppg->Get_ymax(), ppg->Get_xmax(), ref_lat, ref_lon, &e1, &n1);
    toSM(ppg->Get_ymin(), ppg->Get_xmin(), ref_lat, ref_lon, &e2, &n2);
    x = (e1 + e2) / 2.0;
    y = (n1 + n2) / 2.0;

    double xll, yll;
    fromSM(x, y, ref_lat, ref_lon, &yll, &xll);
    m_lon = xll;
    m_lat = yll;

    return true;
}

bool S57Obj::SetMultipointGeometry(MultipointGeometryDescriptor* pGeo,
                                   double ref_lat, double ref_lon) {
    Primitive_type = GEO_POINT;

    npt = pGeo->pointCount;
    geoPtz     = static_cast<double*>(std::malloc(npt * 3 * sizeof(double)));
    geoPtMulti = static_cast<double*>(std::malloc(npt * 2 * sizeof(double)));

    double* pdd = geoPtz;
    double* pdl = geoPtMulti;

    float* pfs = reinterpret_cast<float*>(pGeo->pointTable);
    for (int ip = 0; ip < npt; ip++) {
        float easting  = *pfs++;
        float northing = *pfs++;
        float depth    = *pfs++;

        *pdd++ = easting;
        *pdd++ = northing;
        *pdd++ = depth;

        double xll, yll;
        fromSM(easting, northing, ref_lat, ref_lon, &yll, &xll);
        *pdl++ = xll;
        *pdl++ = yll;
    }

    BBObj.Set(pGeo->extent_s_lat, pGeo->extent_w_lon,
              pGeo->extent_n_lat, pGeo->extent_e_lon);
    bBBObj_valid = true;

    return true;
}

int S57Obj::GetAttributeIndex(const char* AttrSeek) {
    char* patl = att_array;
    for (int i = 0; i < n_attr; i++) {
        if (!std::strncmp(patl, AttrSeek, 6))
            return i;
        patl += 6;
    }
    return -1;
}

wxString S57Obj::GetAttrValueAsString(const char* AttrName) {
    wxString str;
    int idx = GetAttributeIndex(AttrName);
    if (idx >= 0 && attVal) {
        S57attVal* v = attVal->Item(idx);
        switch (v->valType) {
            case OGR_STR:  str = wxString(static_cast<char*>(v->value), wxConvUTF8); break;
            case OGR_REAL: str.Printf(_T("%g"), *static_cast<double*>(v->value));    break;
            case OGR_INT:  str.Printf(_T("%d"), *static_cast<int*>(v->value));       break;
            default:       str = _T("Unknown attribute type");                       break;
        }
    }
    return str;
}
