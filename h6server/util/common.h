#pragma once
#include <vector>

using namespace std;
namespace COMMON
{
	void combineNearNumers(vector<float> &idxs, int len_threshold); /* 给定一个数组，合并相邻的数字*/
	int minEditDistance(string word1, string word2);  /* 计算两个单词的编辑距离 */
	string stripAllSpace(string str);  /* 去除字符串中的所有空格 */
	void stripRightLF(string &str); // 去除字符串右边的换行符

	bool floatEqual(float d1, float d2, float accuracy = 0.000001);
	bool charEqual(unsigned char a, unsigned char b);
	bool isFloat(string str);
}