#pragma once
/* 
语音识别【科大讯飞】
目前仅支持 16000频率 单声道 mav 格式的文件 
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

