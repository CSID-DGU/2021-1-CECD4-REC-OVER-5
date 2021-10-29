#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string.h>
#include <string>
#include <iostream>
#include <ctime>

#import "ADS.tlb" no_namespace named_guids
#import "Trinet.Core.IO.Ntfs.tlb"

#define BUFFER_SIZE 2000
using namespace std;

/*
pszDirectory : 찾고자 하는 폴더의 위치
pszFilter : 검색할 필터
*.dds  dds 파일만 검색하는 경우
*      모든 파일및 폴더를 찾는다.
\\    폴더만 찾는다
iLevel    : 검색 할 하위 폴더의 깊이
1      바로 하위 폴더만 검색
-1    모든 폴더 검색
2      바로 하위 폴더, 그 다음 하위 폴더 검색
*/

//SYSTEM TIME TO STRING
template<typename ... Args> std::string string_format(const std::string& format, Args ... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0' 
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside }

}

//vtList : 파일 이름 리스트, timeList : 파일 최근 수정 시간 리스트
void GetFindFileList(const char* pszDirectory, const char* pszFilter, std::vector <std::string>* vtList, std::vector <std::string>* CtimeList,
	std::vector <std::string>* LAtimeList, std::vector <std::string>* LWtimeList,
	std::vector <std::string>* downloadList, std::vector <std::string>* flagList, int iLevel = -1)
	//void GetFindFileList(const char* pszDirectory, const char* pszFilter, std::vector <std::string>* vtList, int iLevel = -1)
{
	ADSInterface* ads = NULL;
	cout << "CoInitialize : " << CoInitialize(NULL) << endl;
	HRESULT hr = CoCreateInstance(CLSID_ADSClass, NULL, CLSCTX_INPROC_SERVER, IID_ADSInterface, reinterpret_cast<void**>(&ads));
	cout << "hr : " << hr << endl;
	if (!SUCCEEDED(hr)) {
		cout << "dll 불러오기 실패";
		return;
	}

	

	if (iLevel == 0)
		return;

	char buf[BUFFER_SIZE];
	char Ctimebuf[BUFFER_SIZE] = "";
	char LAtimebuf[BUFFER_SIZE] = "";
	char LWtimebuf[BUFFER_SIZE] = "";

	char downloadbuf[BUFFER_SIZE] = "";
	char flagbuf[BUFFER_SIZE] = "";

	WIN32_FIND_DATA FindFileData;
	bool bAddFile = true;
	bool bAddFolder = false;

	if (pszFilter[1] == 0)
	{
		if (pszFilter[0] == '\\')
		{
			bAddFile = false;
			bAddFolder = true;
		}
		else if (pszFilter[0] == '*')
		{
			bAddFolder = true;
		}
	}

	strcpy_s(buf, BUFFER_SIZE, pszDirectory);
	//strcpy(buf, pszDirectory);
	strcat_s(buf, BUFFER_SIZE, "\\");
	//strcat(buf, "\\");

	if (bAddFolder == true)
		strcat_s(buf, BUFFER_SIZE, "*.*");
	//strcat(buf, "*.*");
	else
		strcat_s(buf, BUFFER_SIZE, pszFilter);
	//strcat(buf, pszFilter);

	HANDLE hHandle = FindFirstFile(buf, &FindFileData);


	for (; hHandle != INVALID_HANDLE_VALUE; )
	{
		if ((FindFileData.cFileName[0] != '.' || strlen(FindFileData.cFileName) > 2))
		{
			strcpy_s(buf, BUFFER_SIZE, pszDirectory);
			strcpy_s(Ctimebuf, BUFFER_SIZE, "");
			strcpy_s(LAtimebuf, BUFFER_SIZE, "");
			strcpy_s(LWtimebuf, BUFFER_SIZE, "");


			strcat_s(buf, BUFFER_SIZE, "\\");
			strcat_s(buf, BUFFER_SIZE, FindFileData.cFileName);


			SYSTEMTIME Sys1, Sys2, Sys3;


			//			GetFileTime(hHandle, &FindFileData.ftCreationTime, &FindFileData.ftLastAccessTime, &FindFileData.ftLastWriteTime);

			FileTimeToSystemTime(&FindFileData.ftCreationTime, &Sys1);
			FileTimeToSystemTime(&FindFileData.ftLastAccessTime, &Sys2);
			FileTimeToSystemTime(&FindFileData.ftLastWriteTime, &Sys3);

			string Ctime, LAtime, LWtime;
			Ctime = string_format("%d/%d/%d/%d:%d:%d", Sys1.wYear, Sys1.wMonth, Sys1.wDay, Sys1.wHour, Sys1.wMinute, Sys1.wSecond);
			LAtime = string_format("%d/%d/%d/%d:%d:%d", Sys2.wYear, Sys2.wMonth, Sys2.wDay, Sys2.wHour, Sys2.wMinute, Sys2.wSecond);
			LWtime = string_format("%d/%d/%d/%d:%d:%d", Sys3.wYear, Sys3.wMonth, Sys3.wDay, Sys3.wHour, Sys3.wMinute, Sys3.wSecond);
			const char* Ct = Ctime.c_str();
			const char* LAt = LAtime.c_str();
			const char* LWt = LWtime.c_str();
			strcat_s(Ctimebuf, BUFFER_SIZE, Ct);
			strcat_s(LAtimebuf, BUFFER_SIZE, LAt);
			strcat_s(LWtimebuf, BUFFER_SIZE, LWt);

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (bAddFolder) {
					vtList->push_back(buf);
					CtimeList->push_back(Ctimebuf);
					LAtimeList->push_back(LAtimebuf);
					LWtimeList->push_back(LWtimebuf);
				}
				GetFindFileList(buf, pszFilter, vtList, CtimeList, LAtimeList, LWtimeList, downloadList, flagList, iLevel - 1);
				//				GetFindFileList(buf, pszFilter, vtList, iLevel - 1);
			}
			else
			{
				if (bAddFile) {
					vtList->push_back(buf);
					CtimeList->push_back(Ctimebuf);
					LAtimeList->push_back(LAtimebuf);
					LWtimeList->push_back(LWtimebuf);
					
					//다운로드 start				
					if (ads->isDownloadFile(buf)) {
						downloadList->push_back("1");
					}
					else {
						downloadList->push_back("0");
					}
					//다운로드 end


					//플래그 start
					string s(ads->getADSStream(buf, "rudyflag"), SysStringLen(ads->getADSStream(buf, "rudyflag")));
					flagList->push_back(s);
					//플래그 end


				}
			}
		}

		if (!FindNextFile(hHandle, &FindFileData))
			break;
	}

	FindClose(hHandle);
}

int main()
{
	
	vector <string> vtFileList, vtFileCtimeList, vtFileLAtimeList, vtFileLWtimeList;
	vector <string> vtFileDownloadList, vtFileFlagList;
	vector <string>::iterator iter;

	//	GetFindFileList("d:\\Android", "*.*", &vtFileList, -1);
	//	GetFindFileList("C:\\Users\\yuti9", "*.*", &vtFileList, -1);
	GetFindFileList("C:\\test", "*.*", &vtFileList, &vtFileCtimeList, &vtFileLAtimeList, &vtFileLWtimeList,
		&vtFileDownloadList, &vtFileFlagList, -1);
	int FileCount = (int)vtFileList.size();

	string rpt, ctime, latime, lwtime;
	string download, flag;

	for (iter = vtFileList.begin(); iter != vtFileList.end(); ++iter)
		rpt += "\n" + *iter;

	for (iter = vtFileCtimeList.begin(); iter != vtFileCtimeList.end(); ++iter)
		ctime += "\n" + *iter;

	for (iter = vtFileLAtimeList.begin(); iter != vtFileLAtimeList.end(); ++iter)
		latime += "\n" + *iter;

	for (iter = vtFileLWtimeList.begin(); iter != vtFileLWtimeList.end(); ++iter)
		lwtime += "\n" + *iter;

	for (iter = vtFileDownloadList.begin(); iter != vtFileDownloadList.end(); ++iter)
		download += "\n" + *iter;
	for (iter = vtFileFlagList.begin(); iter != vtFileFlagList.end(); ++iter)
		flag += "\n" + *iter;
	/*
	{
		const char *fileName = iter->c_str();
		cout << fileName << endl;
	}
	*/
	
	FILE* dest, * ctimedest, * latimedest, * lwtimedest;
	FILE* downloaddest, * flagdest;

	fopen_s(&dest, "C:\\test\\파일이름.txt", "wb");
	fopen_s(&ctimedest, "C:\\test\\생성시간.txt", "wb");
	fopen_s(&lwtimedest, "C:\\test\\수정시간.txt", "wb");
	fopen_s(&latimedest, "C:\\test\\접근시간.txt", "wb");

	fopen_s(&downloaddest, "C:\\test\\다운로드유무.txt", "wb");
	fopen_s(&flagdest, "C:\\test\\플래그.txt", "wb");

	fwrite(rpt.c_str(), 1, rpt.size(), dest);
	fwrite(ctime.c_str(), 1, ctime.size(), ctimedest);
	fwrite(lwtime.c_str(), 1, lwtime.size(), lwtimedest);
	fwrite(latime.c_str(), 1, latime.size(), latimedest);

	fwrite(download.c_str(), 1, download.size(), downloaddest);
	fwrite(flag.c_str(), 1, flag.size(), flagdest);


	fclose(dest);
	fclose(ctimedest);
	fclose(lwtimedest);
	fclose(latimedest);
	fclose(downloaddest);
	fclose(flagdest);



    return 0;
}

