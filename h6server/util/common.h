#pragma once
#include <vector>

using namespace std;
namespace COMMON
{
	void combineNearNumers(vector<float> &idxs, int len_threshold);
	int minEditDistance(string word1, string word2);
	string stripAllSpace(string str);  /* È¥³ý¿Õ¸ñ */

	bool floatEqual(float d1, float d2, float accuracy = 0.000001);
	bool charEqual(unsigned char a, unsigned char b);
	bool isFloat(string str);
}