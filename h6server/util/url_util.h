#pragma once
#include <string>
#include <unordered_map>


/// Perform URL-decoding on a string.
std::string url_decode(std::string in);

/* ½âÎö¼üÖµ¶Ô¡£  eg£º  ....=...&...=...&......=... */
void parse_params(std::string data, std::unordered_map<std::string, std::string>& params);
