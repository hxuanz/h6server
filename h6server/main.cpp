// main.cpp : �������̨Ӧ�ó������ڵ㡣

#include "server/server.hpp"
#include "util/my_log.h"
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
	/*  �������� */
	ArgParser parser;
	parser.parse(argc, argv);
	if (parser.hasArg("h", "help"))
	{
		return printHelp();
	}

	string port = "12345";
	string log_dir = "d:/logs";

	if (parser.hasArg("i"))
	{
		port = parser.getArg("i");
	}

	if (parser.hasArg("o"))
	{
		log_dir = parser.getArg("o");
	}

	/*  ��ʼ��LOGϵͳ */
	MyLog::init(log_dir);

	/*  ����server���� */
	try
	{
		Master mater;
		http::server::server s("0.0.0.0", port, mater);

		{
			string tmp = "��������\n�˿�: " + port + "\n��־Ŀ¼ : " + log_dir + "\n ----------------------";
			cout << tmp << endl;
			_INFO(tmp);
		}

		s.run();  //����
	}
	catch (std::exception& e)
	{
		_ERROR("exception: " + string(e.what()));
	}

	return 0;
}

