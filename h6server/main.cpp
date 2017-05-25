// Ӧ�ó������ڵ㡣

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
	/************  �������� ************/
	ArgParser parser;
	parser.parse(argc, argv);
	if (parser.hasArg("h", "help"))
	{
		return printHelp();
	}

	string port = "12345";
	string log_dir;

	if (parser.hasArg("i"))
	{
		port = parser.getArg("i");
	}
	cout << "��ʹ�ö˿ڣ�" << port << endl;

	if (parser.hasArg("o"))
	{
		log_dir = parser.getArg("o");
		cout << "��ʹ����־Ŀ¼��" << log_dir << endl;
	}
	else
	{
		cerr << "���� -o ָ����־Ŀ¼��" << endl;
		return -1;
	}

	/************  ��ʼ��LOGϵͳ ************/
	MyLog::init(log_dir);
	_INFO("init log.\n");

	/************  ����server���� ************/
	try
	{
		Master mater; /* ����ַ� */
		http::server::server s("0.0.0.0", port, mater);

		{
			cout << "��������" << endl;
			_INFO("��������");
		}

		s.run();  //�����ȴ�
	}
	catch (std::exception& e)
	{
		_ERROR("exception: " + string(e.what()));
	}

	return 0;
}

