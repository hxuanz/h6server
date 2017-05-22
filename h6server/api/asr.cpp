#include "asr.h"
extern "C"{
	#include <libavutil/opt.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/samplefmt.h>
	#include <libswresample/swresample.h>
}

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <errno.h>

#include "qisr.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include <fstream>

#include "../util/my_log.h"
using namespace std;

typedef unsigned char uchar;

/* �ϴ��û��ʱ� */
int upload_userwords()
{
	char*			userwords = NULL;
	unsigned int	len = 0;
	unsigned int	read_len = 0;
	FILE*			fp = NULL;
	int				ret = -1;

	fp = fopen("userwords.txt", "rb");
	if (NULL == fp)
	{
		printf("\nopen [userwords.txt] failed! \n");
		goto upload_exit;
	}

	fseek(fp, 0, SEEK_END);
	len = ftell(fp); //��ȡ��Ƶ�ļ���С
	fseek(fp, 0, SEEK_SET);

	userwords = (char*)malloc(len + 1);
	if (NULL == userwords)
	{
		_ERROR("\nout of memory! \n");
		goto upload_exit;
	}

	read_len = fread((void*)userwords, 1, len, fp); //��ȡ�û��ʱ�����
	if (read_len != len)
	{
		_ERROR("\nread [userwords.txt] failed!\n");
		goto upload_exit;
	}
	userwords[len] = '\0';

	MSPUploadData("userwords", userwords, len, "sub = uup, dtt = userword", &ret); //�ϴ��û��ʱ�
	if (MSP_SUCCESS != ret)
	{
		printf("\nMSPUploadData failed ! errorCode: %d \n", ret);
		goto upload_exit;
	}

upload_exit:
	if (NULL != fp)
	{
		fclose(fp);
		fp = NULL;
	}
	if (NULL != userwords)
	{
		free(userwords);
		userwords = NULL;
	}

	return ret;
}
#define	BUFFER_SIZE	4096
#define FRAME_LEN	640 
#define HINTS_SIZE  100

void read_av_file(string audio_file, vector<char>& av_data)
{
	ifstream is(audio_file);
	is.seekg(0, is.end);
	int length = is.tellg();
	is.seekg(0, is.beg);

	char * buffer = new char[length];
	is.read(buffer, length);
	av_data.assign(buffer, buffer + length);
	is.close();
	delete[] buffer;
}

void run_iat(vector<uchar>& av_data, string session_begin_params, string& result)
{
	const char*		session_id = NULL;
	char			hints[HINTS_SIZE] = { NULL }; //hintsΪ�������λỰ��ԭ�����������û��Զ���
	unsigned int	total_len = 0;
	int				aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;		//��Ƶ״̬
	int				ep_stat = MSP_EP_LOOKING_FOR_SPEECH;		//�˵���
	int				rec_stat = MSP_REC_STATUS_SUCCESS;			//ʶ��״̬
	int				errcode = MSP_SUCCESS;

	long pcm_count = 0;
	uchar *p_pcm = av_data.data();
	long pcm_size = av_data.size();
	
	_INFO("\n��ʼ������д ...\n");
	session_id = QISRSessionBegin(NULL, session_begin_params.c_str(), &errcode); //��д����Ҫ�﷨����һ������ΪNULL
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRSessionBegin failed! error code:%d\n", errcode);
		goto iat_exit;
	}

	while (1)
	{
		unsigned int len = 10 * FRAME_LEN; // ÿ��д��200ms��Ƶ(16k��16bit)��1֡��Ƶ20ms��10֡=200ms��16k�����ʵ�16λ��Ƶ��һ֡�Ĵ�СΪ640Byte
		int ret = 0;

		if (pcm_size < 2 * len)
			len = pcm_size;
		if (len <= 0)
			break;

		aud_stat = MSP_AUDIO_SAMPLE_CONTINUE;
		if (0 == pcm_count)
			aud_stat = MSP_AUDIO_SAMPLE_FIRST;

		printf(">");
		ret = QISRAudioWrite(session_id, (const void *)&p_pcm[pcm_count], len, aud_stat, &ep_stat, &rec_stat);
		if (MSP_SUCCESS != ret)
		{
			printf("\nQISRAudioWrite failed! error code:%d\n", ret);
			goto iat_exit;
		}

		pcm_count += (long)len;
		pcm_size -= (long)len;

		if (MSP_REC_STATUS_SUCCESS == rec_stat) //�Ѿ��в�����д���
		{
			const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
			if (MSP_SUCCESS != errcode)
			{
				printf("\nQISRGetResult failed! error code: %d\n", errcode);
				goto iat_exit;
			}
			if (NULL != rslt)
			{
				unsigned int rslt_len = strlen(rslt);
				total_len += rslt_len;
				if (total_len >= BUFFER_SIZE)
				{
					printf("\nno enough buffer for rec_result !\n");
					goto iat_exit;
				}
				result += string(rslt, rslt_len);
			}
		}

		if (MSP_EP_AFTER_SPEECH == ep_stat)
			break;
		Sleep(200); //ģ����˵��ʱ���϶��200ms��Ӧ10֡����Ƶ
	}
	errcode = QISRAudioWrite(session_id, NULL, 0, MSP_AUDIO_SAMPLE_LAST, &ep_stat, &rec_stat);
	if (MSP_SUCCESS != errcode)
	{
		printf("\nQISRAudioWrite failed! error code:%d \n", errcode);
		goto iat_exit;
	}

	while (MSP_REC_STATUS_COMPLETE != rec_stat)
	{
		const char *rslt = QISRGetResult(session_id, &rec_stat, 0, &errcode);
		if (MSP_SUCCESS != errcode)
		{
			printf("\nQISRGetResult failed, error code: %d\n", errcode);
			goto iat_exit;
		}
		if (NULL != rslt)
		{
			unsigned int rslt_len = strlen(rslt);
			total_len += rslt_len;
			if (total_len >= BUFFER_SIZE)
			{
				printf("\nno enough buffer for rec_result !\n");
				goto iat_exit;
			}
			result += string(rslt, rslt_len);
		}
		Sleep(150); //��ֹƵ��ռ��CPU
	}

iat_exit:

	QISRSessionEnd(session_id, hints);
}


ASR::ASR()
{
	const char* login_params = "appid = 5913cf8c, work_dir = ."; // ��¼������appid��msc���,��������Ķ�
	/* �û���¼ */
	int ret = MSPLogin(NULL, NULL, login_params); //��һ���������û������ڶ������������룬����NULL���ɣ������������ǵ�¼����	
	if (MSP_SUCCESS != ret)
	{
		printf("MSPLogin failed , Error code %d.\n", ret);
	}
}


ASR::~ASR()
{
	MSPLogout(); 
}


int ASR::convertFromat(unsigned char *src_data, unsigned char *dst_data)
{
	// TODO  ͨ�� http://ffmpeg.org/doxygen/trunk/index.html ����ƵתΪ 16k�����ʵ�wav�ļ�
}


int ASR::recognise(vector<uchar>& av_data)
{
	/*
	* sub:				����ҵ������
	* domain:			����
	* language:			����
	* accent:			����
	* sample_rate:		��Ƶ������
	* result_type:		ʶ������ʽ
	* result_encoding:	��������ʽ
	*
	* ��ϸ����˵������ġ�Ѷ��������MSC--API�ĵ���
	*/
	string session_begin_params = "sub = iat, domain = iat, language = zh_cn, accent = mandarin, sample_rate = 16000, result_type = plain, result_encoding = gb2312";

	//upload_userwords();  //�ϴ��û��ʱ�ɹ�
	run_iat(av_data, session_begin_params, result_); //iflytek02��Ƶ����Ϊ���������ء�������ϴ����û��ʱ�ʶ����Ϊ���������ٿء���

	//_getch();
	return 0;
}
