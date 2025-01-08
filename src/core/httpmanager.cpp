#include "httpmanager.h"
#include <utils/utils.h>

#include <hv/requests.h>
#include <hv/hthread.h>

using namespace hv;

void HTTP::Get(const std::string& url, HttpResponseCallback cb) {
	Get(url, json(), cb);
}

void HTTP::Get(const std::string& url, const json& params, HttpResponseCallback cb) {
	HttpRequestPtr req(new HttpRequest);
	req->method = HTTP_GET;

	std::string fullUrl = url;
	if (!params.empty()) {
		std::string queryString;
		for (auto it = params.begin(); it != params.end(); ++it) {
			if (!queryString.empty()) {
				queryString += "&";
			}
			queryString += it.key() + "=" + it.value().dump();
		}

		fullUrl = url + "?" + queryString;
	}
	req->url = fullUrl;

	req->timeout = 10;
	http_client_send_async(req, cb);
}

void HTTP::Post(const std::string& url, HttpResponseCallback cb) {
	Post(url, json(), cb);
}

void HTTP::Post(const std::string& url, const json& body, HttpResponseCallback cb) {
	HttpRequestPtr req(new HttpRequest);
	req->method = HTTP_POST;
	req->url = url;
	req->timeout = 10;

	if (!body.empty()) {
		std::string sBody = body.dump();
		req->SetBody(sBody);
	}

	req->SetHeader("Content-Type", "application/json");

	http_client_send_async(req, cb);
}

void HTTP::GetPublicIP(IPv4Response callback) {
	static std::string ipv4;
	if (!ipv4.empty()) {
		callback(ipv4);
		return;
	}

	ipv4 = UTIL::GetPublicIP();
	if (!ipv4.empty()) {
		callback(ipv4);
		return;
	}

	Get("http://whatismyip.akamai.com", [callback](const HttpResponsePtr& res) {
		if (!res || res->status_code != 200) {
			return;
		}

		if (!ipv4.empty()) {
			callback(ipv4);
			return;
		}

		ipv4 = res->Body();
		callback(ipv4);
	});
}
