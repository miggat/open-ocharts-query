#pragma once
#include <wx/string.h>
#include "s52s57.h"     // S57attVal, wxArrayOfS57attVal, OGR_* enums
#include "bbox.h"       // LLBBox
#include "mygeom.h"     // PolyTessGeo, LineGeometryDescriptor, MultipointGeometryDescriptor
#include "georef.h"     // toSM/fromSM

#ifndef GEO_POINT
#define GEO_POINT 1
#endif
#ifndef GEO_AREA
#define GEO_AREA 3
#endif

class S57Obj {
public:
    S57Obj();
    explicit S57Obj(const char* featureName);
    ~S57Obj();

    bool AddIntegerAttribute(const char* acronym, int val);
    bool AddDoubleAttribute(const char* acronym, double val);
    bool AddStringAttribute(const char* acronym, char* val);

    bool SetPointGeometry(double lat, double lon, double ref_lat, double ref_lon);
    bool SetLineGeometry(LineGeometryDescriptor* pGeo, GeoPrim_t geoType,
                         double ref_lat, double ref_lon);
    bool SetAreaGeometry(PolyTessGeo* ppg, double ref_lat, double ref_lon);
    bool SetMultipointGeometry(MultipointGeometryDescriptor* pGeo,
                               double ref_lat, double ref_lon);

    int       GetAttributeIndex(const char* AttrSeek);
    wxString  GetAttrValueAsString(const char* AttrName);

    // Campos que Osenc.cpp espera
    char   FeatureName[7] = {0};
    int    Primitive_type = 0;

    LLBBox BBObj;
    bool   bBBObj_valid = false;

    double m_lon = 0.0, m_lat = 0.0;
    double x = 0.0, y = 0.0;

    PolyTessGeo* pPolyTessGeo = nullptr;

    // líneas / multipunto
    int*   m_lsindex_array = nullptr;
    int    m_n_lsindex = 0;
    int    m_n_edge_max_points = 0;

    int     npt = 0;
    double* geoPt = nullptr;
    double* geoPtz = nullptr;
    double* geoPtMulti = nullptr;

    // atributos
    char*               att_array = nullptr;   // bloques de 6 chars (acrónimo)
    int                 n_attr = 0;
    wxArrayOfS57attVal* attVal = nullptr;

    // varios
    bool   bIsClone = false;
    bool   bIsAssociable = false;
    bool   bIsAton = false;
    double Scamin = 1e8 + 2;
    int    SuperScamin = -1;
    int    nRef = 0;

    double x_rate = 1.0, y_rate = 1.0, x_origin = 0.0, y_origin = 0.0;

private:
    void Init();
};
