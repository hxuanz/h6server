// 应用程序的入口点。

#include "server/server.hpp"
#include "util/log.h"
#include "util/arg_parser.h"
#include "master.h"

using namespace std;

int printHelp()
{
	cout << "usage: h6ocr -i port -o log_dir" << endl;
	return 0;
}

int main(int argc, char* argv[])
{
	/*  解析参数 */
	ArgParser parser;
	parser.parse(argc, argv);
	if (parser.hasArg("h", "help"))
	{
		return printHelp();
	}

	string port = "12345";
	string log_dir = "logs";

	if (parser.hasArg("i"))
	{
		port = parser.getArg("i");
	}

	if (parser.hasArg("o"))
	{
		log_dir = parser.getArg("o");
	}

	/*  初始化LOG系统 */
	MyLog::init(log_dir);
	_INFO("init log.\n");
	/*  启动server服务 */
	try
	{
		Master mater; /* 任务分发 */
		http::server::server s("0.0.0.0", port, mater);

		{
			string tmp = "启动服务\n端口: " + port + "\n日志目录 : " + log_dir + "\n ----------------------";
			cout << tmp << endl;
			_INFO(tmp);
		}

		s.run();  //阻塞等待
	}
	catch (std::exception& e)
	{
		_ERROR("exception: " + string(e.what()));
	}

	return 0;
}

