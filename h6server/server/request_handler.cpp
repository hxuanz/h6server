#include "request_handler.hpp"
#include "mime_types.hpp"
#include "reply.hpp"
#include "request.hpp"

#include "json/json.h"
#include "../util/uri_codec.h"
#include "../master.h"
namespace http {
	namespace server {

		request_handler::request_handler(Master& mater) : mater_(mater)
		{
		}

		void request_handler::handle_request(const request& req, reply& rep)
		{
			std::string request_path = url_decode(req.uri);
			// �������� / ��ͷ�ľ���·��
			if (request_path.empty() || request_path[0] != '/' || request_path.find("..") != std::string::npos)
			{
				rep = reply::stock_reply(reply::bad_request);
				return;
			}
			/* �ַ����󣬷������� */ 
			Json::Value result_root;
			
			mater_.svc(request_path, req.data, result_root);
			
			Json::FastWriter writer;
			rep.content = writer.write(result_root);
			rep.status = reply::ok;
			rep.headers.resize(2);
			rep.headers[0] = { "Content-Length", std::to_string(rep.content.size()) };
			rep.headers[1] = { "Content-Type", "application/json"};  //��Ӧ����extensionȷ��
		}
	} // namespace server
} // namespace http