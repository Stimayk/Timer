#include "surf_global.h"
#include <hv/requests.h>
#include <hv/hthread.h>

void CSurfGlobalAPIPlugin::CreateRequest(std::string sEndpointAlias, const GlobalAPIRequest& req, HttpResponseCallback& cb) {
	if (!m_umEndpoint.contains(sEndpointAlias)) {
		SURF_ASSERT(false);
		return;
	}

	auto& sEndpoint = m_umEndpoint[sEndpointAlias];

	switch (req.method) {
		case HTTP_GET: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_GET;

			std::string fullUrl = sEndpoint;
			if (!req.empty()) {
				std::string queryString;
				for (auto it = req.begin(); it != req.end(); ++it) {
					if (!queryString.empty()) {
						queryString += "&";
					}
					queryString += it.key() + "=" + it.value().dump();
				}

				fullUrl = sEndpoint + "?" + queryString;
			}
			http_req->url = fullUrl;

			http_req->timeout = req.iTimeout;

			if (req.bBearerToken) {
				http_req->SetBearerTokenAuth(m_sBearerToken);
			}

			http_client_send_async(http_req, cb);
			break;
		}
		case HTTP_POST: {
			HttpRequestPtr http_req(new HttpRequest);
			http_req->method = HTTP_POST;
			http_req->url = sEndpoint;
			http_req->timeout = req.iTimeout;

			if (!req.empty()) {
				std::string sBody = req.dump();
				http_req->SetBody(sBody);
			}

			if (req.bBearerToken) {
				http_req->SetBearerTokenAuth(m_sBearerToken);
			}

			http_req->SetHeader("Content-Type", "application/json");

			http_client_send_async(http_req, cb);
			break;
		}
		default: {
			SURF_ASSERT(false);
			break;
		}
	}
}

void CSurfGlobalAPIPlugin::GetAuthStatus(HttpResponseCallback cb) {
	GlobalAPIRequest req;
	req.bBearerToken = false;
	req.method = HTTP_GET;
	return CreateRequest("auth", req, cb);
}
