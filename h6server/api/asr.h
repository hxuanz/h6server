#pragma once
/* 
����ʶ�𡾿ƴ�Ѷ�ɡ�
Ŀǰ��֧�� 16000Ƶ�� ������ mav ��ʽ���ļ� 
*/
#include <vector>
using namespace std;

typedef unsigned char uchar;
class ASR
{
public:
	static ASR& Instance()
	{
		static ASR singleton;
		return singleton;
	}
	int convertFromat(uchar *src_data, uchar *dst_data);  //TODO
	int recognise(vector<uchar>& av_data); 
	string retrieve() { return result_; }

protected:
	ASR();
	~ASR();
private:
	string result_;
};

