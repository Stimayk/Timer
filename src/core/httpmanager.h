#pragma once

#include <hv/HttpMessage.h>
#include <sdk/common.h>

using IPv4Response = std::function<void(const std::string& ip)>;

#ifndef HTTPRES_CALLBACK
#define HTTPRES_CALLBACK(fn) static void fn(const HttpResponsePtr& res)
#endif

#ifndef HTTPRES_CALLBACK_L
#define HTTPRES_CALLBACK_L(...) [__VA_ARGS__](const HttpResponsePtr& res) -> void
#endif

namespace HTTP {
	void Get(const std::string& url, HttpResponseCallback cb = 0);
	void Get(const std::string& url, const json& params, HttpResponseCallback cb = 0);

	void Post(const std::string& url, HttpResponseCallback cb = 0);
	void Post(const std::string& url, const json& body, HttpResponseCallback cb = 0);

	void GetPublicIP(IPv4Response callback);
} // namespace HTTP
