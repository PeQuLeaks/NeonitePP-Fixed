#pragma once
#include "curl.h"
#include "url.h"

#define URL_PROTOCOL XOR("http")
#define URL_HOST XOR("127.0.0.1")
#define URL_PORT XOR("5595")
/*
#define URL_HOST XOR("scuffedeggs.cf")
#define URL_PORT XOR("80")

*/
//NPP port = 5746
//NV2 port = 5595

inline bool isReady;
inline std::string gUrl;
inline float gVersion;

static bool ProdMode;
static bool bIsProdMode;
static bool bIsVersionFound;

inline CURLcode(*CurlSetOpt)(struct Curl_easy*, CURLoption, va_list) = nullptr;
inline CURLcode(*CurlEasySetOpt)(struct Curl_easy*, CURLoption, ...) = nullptr;

inline CURLcode CurlSetOpt_(struct Curl_easy* data, CURLoption option, ...)
{
	va_list arg;
	va_start(arg, option);

	const auto result = CurlSetOpt(data, option, arg);

	va_end(arg);
	return result;
}

inline CURLcode CurlEasySetOptDetour(struct Curl_easy* data, CURLoption tag, ...)
{
	va_list arg;
	va_start(arg, tag);

	CURLcode result = {};

	if (!data) return CURLE_BAD_FUNCTION_ARGUMENT;

	//Ssl bypass
	if (tag == CURLOPT_SSL_VERIFYPEER)
	{
		result = CurlSetOpt_(data, tag, 0);
	}

	//Disable Proxy
	if (tag == CURLOPT_PROXY)
	{
		result = CurlSetOpt_(data, tag, "");
	}

	//URL redirection
	else if (tag == CURLOPT_URL)
	{
		std::string url = va_arg(arg, char*);
		gUrl = url;

		if (url.find("ClientQuest") != std::string::npos) isReady = !isReady;

#ifdef URL_HOST

		if (!ProdMode)
		{
			Uri uri = Uri::Parse(url);

			//if (uri.Host.ends_with(XOR(".epicgames.com")) || uri.Host.ends_with(XOR(".akamaized.net")) || uri.Host.find("fortnite") )
			//{
				//printf("LogURL: %s\n", url.c_str());
				url = Uri::CreateUri(URL_PROTOCOL, URL_HOST, URL_PORT, uri.Path, uri.QueryString);
			//}
		}

#endif
		result = CurlSetOpt_(data, tag, url.c_str());
	}

	//Version determination
	else if (tag == CURLOPT_HTTPHEADER && !bIsVersionFound)
	{
		auto list = va_arg(arg, curl_slist*);;

		while (list->next != nullptr && list->data)
		{
			std::string listData = list->data;
			if (listData.starts_with(XOR("User-Agent:")))
			{
				const auto version = listData.erase(0, listData.find_first_of("0123456789")); //find first number
				gVersion = std::stof(version);
				bIsVersionFound = !bIsVersionFound;
			}
			list = list->next;
		}
	}

	else
	{
		result = CurlSetOpt(data, tag, arg);
	}

	va_end(arg);
	return result;
}
