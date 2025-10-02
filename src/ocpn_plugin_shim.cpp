#include "ocpn_plugin_shim.h"
#include <math.h>

// Constantes equivalentes a las usadas en OpenCPN
static constexpr double PI = 3.14159265358979323846;
static constexpr double DEGREE = PI / 180.0;
static constexpr double mercator_k0 = 1.0;                // como en OCPN
static constexpr double WGS84_semimajor_axis_meters = 6378137.0;

// Inversa Mercator “simple” (idéntica a georef.cpp de OCPN)
// lat/lon en grados, x/y en metros, centrado en (lat0, lon0)
extern "C" void fromSM_Plugin(double x, double y, double lat0, double lon0, double* lat, double* lon)
{
    const double z  = WGS84_semimajor_axis_meters * mercator_k0;
    const double s0 = sin(lat0 * DEGREE);
    const double y0 = (.5 * log((1.0 + s0) / (1.0 - s0))) * z;

    *lat = (2.0 * atan(exp((y0 + y) / z)) - PI / 2.0) / DEGREE;
    *lon = lon0 + (x / (DEGREE * z));
}

// Directa Mercator “simple” (simétrica a la anterior)
extern "C" void toSM_Plugin(double lat, double lon, double lat0, double lon0, double* x, double* y)
{
    const double z  = WGS84_semimajor_axis_meters * mercator_k0;

    const double s0 = sin(lat0 * DEGREE);
    const double y0 = (.5 * log((1.0 + s0) / (1.0 - s0))) * z;

    const double s  = sin(lat * DEGREE);
    const double yy = (.5 * log((1.0 + s) / (1.0 - s))) * z;

    *x = DEGREE * z * (lon - lon0);
    *y = yy - y0;
}

// versión de OpenCPN:

// void toSM(double lat, double lon, double lat0, double lon0, double *x,
//           double *y) {
//   double xlon = lon;

//   /*  Make sure lon and lon0 are same phase */

//   if ((lon * lon0 < 0.) && (fabs(lon - lon0) > 180.)) {
//     lon < 0.0 ? xlon += 360.0 : xlon -= 360.0;
//   }

//   const double z = WGS84_semimajor_axis_meters * mercator_k0;

//   *x = (xlon - lon0) * DEGREE * z;

//   // y =.5 ln( (1 + sin t) / (1 - sin t) )
//   const double s = sin(lat * DEGREE);
//   const double y3 = (.5 * log((1 + s) / (1 - s))) * z;

//   const double s0 = sin(lat0 * DEGREE);
//   const double y30 = (.5 * log((1 + s0) / (1 - s0))) * z;
//   *y = y3 - y30;
// }