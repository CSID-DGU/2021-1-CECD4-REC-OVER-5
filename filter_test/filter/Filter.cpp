#include <iostream>
#include <string>
#include "Filter.h"

/*
	다운로드 유무 배열 – int Down[]
	생성 날짜 배열 - char Made[]
	수정 날짜 배열 – char Fix[]
	백업 플래그 확인 배열 – int Flag[]
	확장자 배열 (문서 확장자 확인) – char Ext[]
	용량 배열 – int Cap[]
	우선 백업 중요도 배열 – int Pri[]
	최근 백업 날짜 - char BackupDate
	*/


int DateToInt(const char a_date_str[]) // 배열에 저장된 날짜 문자열을 비교하기 위하여 숫자로 변환
{
	int date_num = 0;

	if (a_date_str[] != '-')
	{
		// '-' 문자는 제외하고 a_date_str[] 문자를 숫자로 변경해서 합산한다.
		// 그리고 합산하기 전에 date_num 에 저장된 값은 10 을 곱해서 자릿수를 증가시킨다.
		date_num = date_num * 10 + a_date_str[] - '0';
	}

	return date_num;
}

int Filter::filter_1(int a)
{

	if (Down[a] == 1) // 다운받은 파일일 경우
		if (Made[a] != Fix[a]) // 생성일 수정일 다를 경우
		{
			if (DateToInt(Fix[a]) > DateToInt(BackupDate[10])) // 수정일이 최근 백업 날짜보다 최근일 경우
			{
				Pri[a] = 1;
			}
			else if (DateToInt(Fix[a]) < DateToInt(BackupDate[10])) // 최근 백업 날짜가 수정일보다 최근일 경우
			{
				Pri[a] = 0;
			}
		}
		else if (Made[a] == Fix[a]) // 수정일 생성일 같을 경우
		{
			Pri[a] = 0;
		}
		else if (Down[a] == 0) // 다운받은 파일이 아닐 경우
			if (DateToInt(Fix[a]) > DateToInt(BackupDate[10])) // 수정일이 최근 백업 날짜보다 최근일 경우
			{
				Pri[a] = 1;
			}
			else if (DateToInt(Fix[a]) < DateToInt(BackupDate[10])) // 최근 백업 날짜가 수정일보다 최근일 경우
			{
				Pri[a] = 0;
			}


	return Pri[a];
}

int Extension(int a) // 문서 확장자
{

	if (Ext[a] = 'doc')
		Cpacity(a);
	else if (Ext[a] = 'docx')
		Cpacity(a);
	else if (Ext[a] = 'hwp')
		Cpacity(a);
	else if (Ext[a] = 'ppt')
		Cpacity(a);
	else if (Ext[a] = 'pptx')
		Cpacity(a);

	return Pri[a];
}

int Cpacity(int a)
{
	if (Cap[a] >= 1000000) //용량 기준치는 임의로 1000000로 설정해서 진행, 넘으면 중요, 넘지 않으면 중요도 낮음
		Pri[a] = 1;
	else
		Pri[a] = 0;

	return Pri[a];
}