#pragma once

#include <tuple>
#include <LiteHttpdDev.h>
#include <event2/http.h>

class RequestParamsBuilder final {
	RequestParamsBuilder() = delete;

public:
	static void build(
		RequestParams& params, evhttp_request* request,
		bool https, uint16_t port);

private:
	static std::tuple<std::string, uint16_t> parseHostStr(const std::string& hostStr, bool https);
};
