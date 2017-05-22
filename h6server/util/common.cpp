#include "common.h"
#include <string>
#include <algorithm>
#include <iostream>
#include <sstream>
using namespace std;

namespace COMMON
{
	void combineNearNumers(vector<float> &idxs, int len_threshold)
	{
		vector<float> new_idxs;
		new_idxs.push_back(idxs.front());  // ��β���ֱ���
		int begin = 0, end;
		for (size_t i = 1; i < idxs.size(); i++)
		{
			float diff = idxs[i] - idxs[i - 1];
			if (diff > 1)
			{
				end = i - 1;
				if (end - begin > len_threshold)
				{
					float val = (idxs[begin] + idxs[end]) / 2;
					new_idxs.push_back(val);
				}
				begin = i;
			}
		}
		new_idxs.push_back(idxs.back());
		idxs = new_idxs;
	}

	bool floatEqual(float d1, float d2, float accuracy)
	{
		return abs(d1 - d2) < accuracy;
	}
	/* �Ƿ���ͬһ����ĸ�� �����ִ�Сд*/
	bool isCharEqual(unsigned char a, unsigned char b)
	{
		if (isalpha(a) && isalpha(b))
		{
			return toupper(a) == toupper(b);
		}
		return false;
	}

	/*  TODO: ���ۿ��Կ������¶���
	�����C��G֮����滻�����С*/
	int minEditDistance(string word1, string word2) {
		int m = word1.length(), n = word2.length();
		vector<int> dp(m + 1, 0);
		for (int i = 1; i <= m; i++)
		{
			dp[i] = i;
		}
		for (int j = 1; j <= n; j++)
		{
			int pre = dp[0];
			dp[0] = j;
			for (int i = 1; i <= m; i++)
			{
				int tmp = dp[i];
				if (isCharEqual(word1[i - 1], word2[j - 1]))  // ���Դ�Сд
					dp[i] = pre;
				else
				{
					dp[i] = min(pre + 1, min(dp[i] + 1, dp[i - 1] + 1));
				}
				pre = tmp;
			}
		}
		return dp.back();
	}

	/* ȥ���ո� */
	string stripAllSpace(string str)
	{
		vector<char> tmp;
		for (char c : str)
		{
			if (c == ' ') continue;
			tmp.push_back(c);
		}
		return string(tmp.begin(), tmp.end());
	}
	
	bool isFloat(string myString) {
		std::istringstream iss(myString);
		float f;
		iss >> noskipws >> f; // noskipws considers leading whitespace invalid
		return iss.eof() && !iss.fail();// Check the entire string was consumed and if either failbit or badbit is set
	}
}