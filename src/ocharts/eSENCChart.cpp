#include "ocharts/s52s57.h"
#include "ocpn_plugin_shim.h"

//----------------------------------------------------------------------------------
//      S57Obj CTOR
//----------------------------------------------------------------------------------

S57Obj::S57Obj()
{
    Init();
}

//----------------------------------------------------------------------------------
//      S57Obj DTOR
//----------------------------------------------------------------------------------

S57Obj::~S57Obj()
{
    //  Don't delete any allocated records of simple copy clones
    if (!bIsClone)
    {
        if (attVal)
        {
            for (unsigned int iv = 0; iv < attVal->GetCount(); iv++)
            {
                S57attVal *vv = attVal->Item(iv);
                void *v2 = vv->value;
                free(v2);
                delete vv;
            }
            delete attVal;
        }
        free(att_array);

        //         if( pPolyTessGeo ) {
        // #ifdef ocpnUSE_GL
        //             bool b_useVBO = g_b_EnableVBO  && !auxParm1;    // VBO allowed?

        //             PolyTriGroup *ppg_vbo = pPolyTessGeo->Get_PolyTriGroup_head();
        //             if (b_useVBO && ppg_vbo && auxParm0 > 0 && ppg_vbo->single_buffer ) {
        //                  glDeleteBuffers(1, (GLuint *)&auxParm0);
        //             }
        // #endif
        //             delete pPolyTessGeo;
        //         }

        // if( pPolyTrapGeo ) delete pPolyTrapGeo;

        // if (FText)
        //     delete FText;

        if (geoPt)
            free(geoPt);
        if (geoPtz)
            free(geoPtz);
        if (geoPtMulti)
            free(geoPtMulti);

        if (m_lsindex_array)
            free(m_lsindex_array);

        if (m_ls_list)
        {
            line_segment_element *element = m_ls_list;
            while (element)
            {
                line_segment_element *next = element->next;
                delete element;
                element = next;
            }
        }
    }
}

void S57Obj::Init()
{
    att_array = NULL;
    attVal = NULL;
    n_attr = 0;

    // pPolyTessGeo = NULL;
    // pPolyTrapGeo = NULL;

    bCS_Added = 0;
    CSrules = NULL;
    // FText = NULL;
    bFText_Added = 0;
    geoPtMulti = NULL;
    geoPtz = NULL;
    geoPt = NULL;
    bIsClone = false;
    Scamin = 10000000; // ten million enough?
    nRef = 0;

    bIsAton = false;
    bIsAssociable = false;
    m_n_lsindex = 0;
    m_lsindex_array = NULL;
    m_n_edge_max_points = 0;
    m_ls_list = 0;
    m_ls_list_legacy = NULL;

    iOBJL = -1; // deferred, done by OBJL filtering in the PLIB as needed
    bBBObj_valid = false;

    //        Set default (unity) auxiliary transform coefficients
    x_rate = 1.0;
    y_rate = 1.0;
    x_origin = 0.0;
    y_origin = 0.0;

    auxParm0 = 0;
    auxParm1 = 0;
    auxParm2 = 0;
    auxParm3 = 0;
}

//----------------------------------------------------------------------------------
//      S57Obj CTOR from FeatureName
//----------------------------------------------------------------------------------
S57Obj::S57Obj(const char *featureName)
{
    Init();

    attVal = new wxArrayOfS57attVal();

    strncpy(FeatureName, featureName, 6);
    FeatureName[6] = 0;

    if (!strncmp(FeatureName, "DEPARE", 6) || !strncmp(FeatureName, "DRGARE", 6))
        bIsAssociable = true;
}

bool S57Obj::AddIntegerAttribute(const char *acronym, int val)
{

    S57attVal *pattValTmp = new S57attVal;

    int *pAVI = (int *)malloc(sizeof(int)); // new int;
    *pAVI = val;

    pattValTmp->valType = OGR_INT;
    pattValTmp->value = pAVI;

    att_array = (char *)realloc(att_array, 6 * (n_attr + 1));
    strncpy(att_array + (6 * sizeof(char) * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(pattValTmp);

    //  TODO  This will be a bit slow, do better?
    if (!strncmp(acronym, "SCAMIN", 6))
        Scamin = val;

    return true;
}

bool S57Obj::AddIntegerListAttribute(const char *acronym, int *pval, int nValue)
{

    return true;
}

bool S57Obj::AddDoubleAttribute(const char *acronym, double val)
{

    S57attVal *pattValTmp = new S57attVal;

    double *pAVI = (double *)malloc(sizeof(double)); // new double;
    *pAVI = val;

    pattValTmp->valType = OGR_REAL;
    pattValTmp->value = pAVI;

    att_array = (char *)realloc(att_array, 6 * (n_attr + 1));
    strncpy(att_array + (6 * sizeof(char) * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(pattValTmp);

    return true;
}

bool S57Obj::AddDoubleListAttribute(const char *acronym, double *pval, int nValue)
{

    return true;
}

bool S57Obj::AddStringAttribute(const char *acronym, char *val)
{

    S57attVal *pattValTmp = new S57attVal;

    char *pAVS = (char *)malloc(strlen(val) + 1); // new string
    strcpy(pAVS, val);

    pattValTmp->valType = OGR_STR;
    pattValTmp->value = pAVS;

    att_array = (char *)realloc(att_array, 6 * (n_attr + 1));
    strncpy(att_array + (6 * sizeof(char) * n_attr), acronym, 6);
    n_attr++;

    attVal->Add(pattValTmp);

    return true;
}

bool S57Obj::SetPointGeometry(double lat, double lon, double ref_lat, double ref_lon)
{
    Primitive_type = GEO_POINT;

    m_lon = lon;
    m_lat = lat;

    //  Set initial Point BoundingBox limits quite small...
    BBObj.Set(m_lat - .0001, m_lon - .0001, m_lat + .0001, m_lon + .0001);
    bBBObj_valid = true;

    //  Calculate SM from chart common reference point
    double easting, northing;
    toSM_Plugin(lat, lon, ref_lat, ref_lon, &easting, &northing);

    x = easting;
    y = northing;

    npt = 1;

    return true;
}

bool S57Obj::SetLineGeometry(LineGeometryDescriptor *pGeo, GeoPrim_t geoType, double ref_lat, double ref_lon)
{
    Primitive_type = geoType;

    // set s57obj bbox as lat/lon
    /// BBObj.SetMin( pGeo->extent_w_lon, pGeo->extent_s_lat );
    /// BBObj.SetMax( pGeo->extent_e_lon, pGeo->extent_n_lat );
    BBObj.Set(pGeo->extent_s_lat, pGeo->extent_w_lon, pGeo->extent_n_lat, pGeo->extent_e_lon);
    bBBObj_valid = true;

    //  and declare x/y of the object to be average east/north of all points
    double e1, e2, n1, n2;
    toSM_Plugin(pGeo->extent_n_lat, pGeo->extent_e_lon, ref_lat, ref_lon, &e1, &n1);
    toSM_Plugin(pGeo->extent_s_lat, pGeo->extent_w_lon, ref_lat, ref_lon, &e2, &n2);

    x = (e1 + e2) / 2.;
    y = (n1 + n2) / 2.;

    //  Set the object base point
    double xll, yll;
    fromSM_Plugin(x, y, ref_lat, ref_lon, &yll, &xll);
    m_lon = xll;
    m_lat = yll;

    //  Set the edge and connected node table indices
    m_n_lsindex = pGeo->indexCount;
    m_lsindex_array = pGeo->indexTable; // object owns this array now.

    m_n_edge_max_points = 0; // TODO this could be precalulated and added to next SENC format

    return true;
}

bool S57Obj::SetMultipointGeometry(MultipointGeometryDescriptor *pGeo, double ref_lat, double ref_lon)
{
    Primitive_type = GEO_POINT;

    npt = pGeo->pointCount;

    geoPtz = (double *)malloc(npt * 3 * sizeof(double));
    geoPtMulti = (double *)malloc(npt * 2 * sizeof(double));

    double *pdd = geoPtz;
    double *pdl = geoPtMulti;

    float *pfs = (float *)(pGeo->pointTable); // start of point data
    for (int ip = 0; ip < npt; ip++)
    {
        float easting, northing;
        easting = *pfs++;
        northing = *pfs++;
        float depth = *pfs++;

        *pdd++ = easting;
        *pdd++ = northing;
        *pdd++ = depth;

        //  Convert point from SM to lat/lon for later use in decomposed bboxes
        double xll, yll;
        fromSM_Plugin(easting, northing, ref_lat, ref_lon, &yll, &xll);

        *pdl++ = xll;
        *pdl++ = yll;
    }

    // set s57obj bbox as lat/lon
    BBObj.Set(pGeo->extent_s_lat, pGeo->extent_w_lon, pGeo->extent_n_lat, pGeo->extent_e_lon);
    bBBObj_valid = true;

    return true;
}