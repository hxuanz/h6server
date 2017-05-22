#include "master.h"

#include <iostream>
#include <unordered_map>
#include <boost/algorithm/string.hpp>

#include "util/uri_codec.h"
#include "util/my_log.h"
#include "util/base64.h"

#include "err.h"

#include "api/blood_ocr.h"
#include "api/asr.h"
using namespace std;

/* 解析post的data字段，键值对。  eg：  ....=...&...=...&......=... */
void parse_params(string data, unordered_map<string, string>& params)
{
	vector<string> strs;
	boost::split(strs, data, boost::is_any_of("&"));
	for (auto str : strs)
	{
		vector<string> key_val;
		boost::split(key_val, str, boost::is_any_of("="));
		/* 参数解析之后再URL解码 */
		params[url_decode(key_val.front())] = url_decode(key_val.back());
	}
}

Master::Master()
{
}


Master::~Master()
{
}

void Master::svc(string request_path, string req_data, Json::Value& result_root)
{
	int ret = 0;
	if (request_path == "/ocr/xhd")
	{
		_INFO("解析数据段");
		/* 解析数据段*/
		unordered_map<string, string> params; /* 解析 post 请求的数据段*/
		parse_params(req_data, params);
		if (params.count("imageData") == 0 || params.count("scaleData") == 0)
		{
			_ERROR("请求缺少参数");
			ret = H6OCRERROR::INVILD_PARAMS;
			goto BREAK;
		}
		if (params.at("imageData").size() == 0)
		{
			ret = H6OCRERROR::INVILD_PARAMS_imageData;
			_ERROR("imageData 有问题");
			goto BREAK;
		}
		if (params.at("scaleData").size() == 0)
		{
			ret = H6OCRERROR::INVILD_PARAMS_scaleData;
			_ERROR("scaleData 有问题");
			goto BREAK;
		}

		/* 调用 ocr api*/
		Blood_OCR& bloodocr = Blood_OCR::Instance();
		ret = bloodocr.loadDictionary(params.at("scaleData"));
		if (ret != 0)
			goto BREAK;
		vector<uchar> iamge_buffer = base64_decode(params.at("imageData"));  //base64_decode
		ret = bloodocr.recognise(iamge_buffer);
		if (ret != 0)
			goto BREAK;

		bloodocr.retrieve(result_root);
	}
	else if (request_path == "/asr")
	{
		/* 调用 asr api*/
		ASR& asr = ASR::Instance();
		vector<uchar> av_data = base64_decode(req_data);
		ret = asr.recognise(av_data);
		if (ret != 0)
			goto BREAK;

		result_root["data"] =  asr.retrieve();
	}
	else /*  后续可能会有其他类型的请求 */
	{
		ret = H6OCRERROR::INVILD_URI;
	}


BREAK:
	if (ret != SUCCESS)
	{
		cout << "fail!" << endl;
		_ERROR("本次OCR失败");
		_ERROR("错误代码: " + to_string(ret));
	}
	else
	{
		cout << "success!" << endl;
	}
	result_root["error_no"] = ret;
}