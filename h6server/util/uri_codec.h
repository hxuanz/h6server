#pragma once
#include <string>
#include <assert.h>
std::string UriDecode(const std::string & sSrc);
std::string UriEncode(const std::string & sSrc);


/// Perform URL-decoding on a string.
std::string url_decode(std::string in);
