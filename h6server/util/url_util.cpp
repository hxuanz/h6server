// Uri encode and decode.
// RFC1630, RFC1738, RFC2396

#include "url_util.h"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <vector>

using namespace std;
/// Perform URL-decoding on a string.
string url_decode(string in)
{
	string out;
	out.reserve(in.size());
	for (size_t i = 0; i < in.size(); ++i)
	{
		if (in[i] == '%')
		{
			if (i + 3 <= in.size())
			{
				int value = 0;
				istringstream is(in.substr(i + 1, 2));
				if (is >> hex >> value)  // ʮ������
				{
					out += static_cast<char>(value);
					i += 2;
				}
				else
				{
					return "";
				}
			}
			else
			{
				return "";
			}
		}
		else if (in[i] == '+')
		{
			out += ' ';
		}
		else
		{
			out += in[i];
		}
	}
	return out;
}

/* ����post��data�ֶΣ���ֵ�ԡ�  eg��  ....=...&...=...&......=... */
void parse_params(string data, unordered_map<string, string>& params)
{
	vector<string> strs;
	boost::split(strs, data, boost::is_any_of("&"));
	for (auto str : strs)
	{
		vector<string> key_val;
		boost::split(key_val, str, boost::is_any_of("="));
		/* ��������֮����URL���� */
		params[url_decode(key_val.front())] = url_decode(key_val.back());
	}
}
