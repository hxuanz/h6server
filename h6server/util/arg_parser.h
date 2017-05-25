#pragma once

#include <cstring>
#include <cstdio>
#include <unordered_map>
#include <vector>
using namespace std;

class ArgParser
{
public:
	ArgParser();
	ArgParser::~ArgParser();

	void parse(int argc, char* argv[]); /* 解析 命令行参数 */

	inline string getArg(string key){ return hasArg(key) ? _args[key] : ""; }
	inline bool hasArg(string key){ return _args.count(key) == 1; }
	inline bool hasArg(string key1, string key2){ return hasArg(key1) || hasArg(key2); }

	inline string getPositionalArgByIndex(int i) { return _free_options[i]; }

private:

	unordered_map<string, string> _args;
	vector<string> _free_options;

};
