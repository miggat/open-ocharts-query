/******************************************************************************
 *
 * Project:  open-ocharts-query
 * Purpose:  Chart FeatureInfo Request Handler (z/x/y via query)
 *
 ***************************************************************************
 *   GPL v2+
 ***************************************************************************
 */
#ifndef CHARTREQUESTHANDLER_H
#define CHARTREQUESTHANDLER_H

#include "RequestHandler.h"
#include "Logger.h"
#include "ChartList.h"
#include "Renderer.h"
#include "TokenHandler.h"

#include <wx/string.h>
#include <cstdlib> // atoi, atof

class ChartRequestHandler : public RequestHandler
{
public:
    // Prefijo de charts
    const wxString URL_PREFIX = wxT("/charts/");

private:
    ChartSet* set;
    wxString name;
    wxString urlPrefix;

public:
    // Mantiene la signatura original, pero no usamos TokenHandler
    ChartRequestHandler(ChartSet* set, TokenHandler* tk /*unused*/)
    {
        (void)tk;
        this->set = set;
        this->name = set->GetKey();
        this->urlPrefix = URL_PREFIX + name + wxT("/");
    }

    // ÚNICO endpoint: /charts/<set>/featureInfo?z&x&y&lat&lon&tolerance
    virtual HTTPResponse* HandleRequest(HTTPRequest* request)
    {
        if (!set || !set->IsActive())
            return new HTTPResponse();

        // --- Logs de diagnóstico ---
        LOG_INFO(_T("HandleRequest url='%s' urlPrefix='%s'"), request->url, urlPrefix);

        // Localiza la parte tras /charts/<set>/ (robusto ante prefijos)
        wxString full = request->url;
        int pos = full.Find(urlPrefix);
        wxString rest = (pos != wxNOT_FOUND) ? full.Mid(pos + urlPrefix.Length())
                                             : full.Mid(urlPrefix.Length());

        rest.Replace("//", "/");
        if (rest.StartsWith("/")) rest = rest.AfterFirst('/');

        wxString noQuery = rest.BeforeFirst('?');
        wxString firstSeg = noQuery.BeforeFirst('/');
        LOG_INFO(_T("Parsed rest='%s' noQuery='%s' firstSeg='%s'"), rest, noQuery, firstSeg);

        // Acepta 'featureInfo' (tolerante a 'featureInfo/' al mirar el primer segmento)
        if (!firstSeg.StartsWith(wxT("featureInfo"))) {
            return new HTTPJsonErrorResponse("unsupported endpoint (use /featureInfo)");
        }

        // --------- Parámetros obligatorios ---------
        NameValueMap* query = &(request->query);

        auto itZ  = query->find("z");         if (itZ  == query->end())  return new HTTPJsonErrorResponse("missing z");
        auto itX  = query->find("x");         if (itX  == query->end())  return new HTTPJsonErrorResponse("missing x");
        auto itY  = query->find("y");         if (itY  == query->end())  return new HTTPJsonErrorResponse("missing y");
        auto itLat = query->find("lat");      if (itLat == query->end()) return new HTTPJsonErrorResponse("missing lat");
        auto itLon = query->find("lon");      if (itLon == query->end()) return new HTTPJsonErrorResponse("missing lon");
        auto itTol = query->find("tolerance");if (itTol == query->end()) return new HTTPJsonErrorResponse("missing tolerance");

        // Parseo
        int z  = atoi(itZ->second);
        int x  = atoi(itX->second);
        int y  = atoi(itY->second);
        double lat = atof(itLat->second);
        double lon = atof(itLon->second);
        double tolerance = atof(itTol->second);



        // Fabricar URL clásica "z/x/y.png" para TileInfo
        wxString tileUrl = wxString::Format(wxT("%d/%d/%d.png"), z, x, y);
        TileInfo tile(tileUrl, name);
        if (!tile.valid) {
            LOG_ERROR(_T("Invalid TileInfo for url=%s"), tileUrl);
            return new HTTPJsonErrorResponse("invalid tile coordinates");
        }

        LOG_INFO(_T("FeatureInfo request: set=%s z=%d x=%d y=%d lat=%.6f lon=%.6f tol=%.6f"),
                 name, z, x, y, lat, lon, tolerance);

        // Llamada API (única ruta soportada)
        wxString result = Renderer::Instance()->FeatureRequestApi(set, tile, lat, lon, tolerance);

        // Respuesta JSON { status: "OK", data: <json-string> }
        HTTPResponse* rt = new HTTPStringResponse(
            "application/json",
            wxString::Format("{" JSON_SV(status, OK) "," JSON_IV(data, %s) "}", result));
        rt->responseHeaders["Access-Control-Allow-Origin"] = "*";
        return rt;
    }

    // Patrón de URL: comodín bajo /charts/<set>/..., filtramos dentro
    virtual wxString GetUrlPattern()
    {
        return urlPrefix + wxT("*");
    }
};

#endif /* CHARTREQUESTHANDLER_H */
