#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void fromSM_Plugin(double x, double y, double lat0, double lon0, double *lat, double *lon);
void toSM_Plugin(double lat, double lon, double lat0, double lon0, double *x, double *y);

#ifdef __cplusplus
}
#endif
