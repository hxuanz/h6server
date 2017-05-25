#pragma once
#include <vector>

using namespace std;
namespace COMMON
{
	void combineNearNumers(vector<float> &idxs, int len_threshold); /* ����һ�����飬�ϲ����ڵ�����*/
	int minEditDistance(string word1, string word2);  /* �����������ʵı༭���� */
	string stripAllSpace(string str);  /* ȥ���ַ����е����пո� */
	void stripRightLF(string &str); // ȥ���ַ����ұߵĻ��з�

	bool floatEqual(float d1, float d2, float accuracy = 0.000001);
	bool charEqual(unsigned char a, unsigned char b);
	bool isFloat(string str);
}