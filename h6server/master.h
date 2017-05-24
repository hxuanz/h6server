#pragma once
/* 任务分发 */
#include "json/json.h"
class Master
{
public:
	Master();
	~Master();
	void svc(std::string request_path, std::string req_data, Json::Value& result);
};

