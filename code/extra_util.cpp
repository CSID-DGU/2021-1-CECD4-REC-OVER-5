#include "stdafx.h"
#include "util.h"
#include "extra_util.h"


#include <fstream>
#include <algorithm>


CString GetMyDocumentLabel(int *sysIconIndex)
{
	LPMALLOC pMalloc; 
	SHGetMalloc(&pMalloc); 
	SHFILEINFO sfi = { 0 }; 
	// 'My Document' or '내 문서' text
	LPITEMIDLIST pidl; 
	SHGetSpecialFolderLocation(NULL,CSIDL_PERSONAL, &pidl); 
	SHGetFileInfo((char*)pidl, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX | SHGFI_PIDL); 
	pMalloc->Free(pidl); 
	pMalloc->Release(); 

	if(sysIconIndex != NULL){
		*sysIconIndex = sfi.iIcon; 
	}
	return sfi.szDisplayName; 
}
CString GetMyComputerLabel(int *sysIconIndex)
{
	LPMALLOC pMalloc; 
	SHGetMalloc(&pMalloc); 
	SHFILEINFO sfi = { 0 }; 
	// 'My Computer' or '내 컴퓨터' text
	LPITEMIDLIST pidl; 
	SHGetSpecialFolderLocation(NULL,CSIDL_DRIVES, &pidl); 
	SHGetFileInfo((char*)pidl, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX | SHGFI_PIDL); 
	pMalloc->Free(pidl); 
	pMalloc->Release(); 

	if(sysIconIndex != NULL){
		*sysIconIndex = sfi.iIcon; 
	}
	return sfi.szDisplayName; 
}
CString GetDesktopLabel(int *sysIconIndex)
{
	TCHAR path[MAX_PATH] = ""; 
	LPMALLOC pMalloc; 
	SHGetMalloc(&pMalloc); 
	SHFILEINFO sfi = { 0 }; 
	// 'Desktop' or '바탕화면' text
	LPITEMIDLIST pidl; 
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl); 
	SHGetFileInfo((char*)pidl, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX | SHGFI_PIDL); 
	pMalloc->Free(pidl);
	pMalloc->Release(); 

	if(sysIconIndex != NULL){
		*sysIconIndex = sfi.iIcon; 
	}
	return sfi.szDisplayName; 

}
BOOL DirectoryHasSubfolder(CString fullPath)
{
	if(PathIsDirectory(fullPath) == FALSE){
		return false; 
	}

	WIN32_FIND_DATA fd = { 0 }; 
	HANDLE hSearch = NULL; 
	BOOL bFinish = false; 
	if(fullPath[1] == ':' && fullPath.GetLength() == 3){
		fullPath = fullPath.Left(2); 
	}

	BOOL has_subfolder = false; 
	hSearch = FindFirstFile(fullPath  + "\\*.*", &fd); 
	while(!bFinish){
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if( (_tcslen(fd.cFileName) == 1 && fd.cFileName[0] == '.') ||
				(_tcslen(fd.cFileName) == 2 && fd.cFileName[0] == '.' && fd.cFileName[1] == '.')){
				has_subfolder = true; 
				break;
			}
		}
		bFinish = !FindNextFile(hSearch, &fd); 
	}
	FindClose(hSearch); 
	
	return has_subfolder; 
}
BOOL GetFirstSubfolderName(CString fullPath, CString* subfolder)
{
	if(PathIsDirectory(fullPath) == FALSE){
		return false; 
	}

	WIN32_FIND_DATA fd = { 0 }; 
	HANDLE hSearch = NULL; 
	BOOL bFinish = false; 
	if(fullPath[1] == ':' && fullPath.GetLength() == 3){
		fullPath = fullPath.Left(2); 
	}

	BOOL has_subfolder = false; 
	hSearch = FindFirstFile(fullPath  + "\\*.*", &fd); 
	while(!bFinish){
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if( (_tcslen(fd.cFileName) == 1 && fd.cFileName[0] == '.') ||
				(_tcslen(fd.cFileName) == 2 && fd.cFileName[0] == '.' && fd.cFileName[1] == '.') ||
				(PathIsSystemFolder(fullPath+"\\"+fd.cFileName,0) == TRUE)
				
				){
				bFinish = !FindNextFile(hSearch, &fd); 
				continue;
			}
				*subfolder = fd.cFileName; 
				FindClose(hSearch); 
				return true; 
		}
		bFinish = !FindNextFile(hSearch, &fd); 
	}
	FindClose(hSearch); 


	return false; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL CheckInformation(CString cifFileName, std::vector<CString>* filter, std::vector<CString>* value, std::vector<bool> *mandatory)
{
/*
	bool bCheckedMandatoryCondition = false; 
	int nCheckedFilterNum = 0; 
	std::ifstream ifile(cifFileName.operator LPCTSTR()); 
	std::string line; 
	while(std::getline(ifile, line)){
		std::vector<std::string> v = split<std::string, string2string>(line, '\t'); 
		if(v.size() != 2) continue; 
		CString filter_name_in_file = v[0].c_str(); 
		std::transform(v[1].begin(), v[1].end(), v[1].begin(), tolower); 
		CString filter_value_in_file = v[1].c_str(); 
		std::vector<CString>::iterator fpos = filter->begin(); 
		std::vector<CString>::iterator vpos = value->begin(); 
		std::vector<bool>::iterator mpos = mandatory->begin(); 
		for(; fpos != filter->end(); fpos++, vpos++, mpos++){
			vpos->MakeLower(); 
			if(*mpos == true){
				if(filter_name_in_file == *fpos){
					if(filter_value_in_file.Find(*vpos) == -1){
						return false;
					}
					nCheckedFilterNum++; 
				}
			}
			else{
				
			}
 
		}
	}


	int man_num = 0; 
	std::vector<bool>::iterator pos = mandatory->begin(); 
	for(; pos != mandatory->end(); pos++){
		man_num += (*pos == true); 
	}
	
	return nCheckedFilterNum >= man_num; 

	// 여기까지 왔다는 것은, 
	// 1. mandatory 인 것 중 FALSE 가 하나도 없었고
	// 2. mandatory 아닌 것 중 TRUE 가 하나도 없었다. 
	// 어차피 mandatory 가 아닌 것은 더이상 check할 필요 없고, 
	// mandatory 였던 것들이 전부 나왔는지만 확인하면 된다. 
	// 그런데 mandatory 인 것 중 한 번이라도 안 나온 것이 있다면
	// 빠져나갔을 것이므로, 여기까지 왔다는 것은 true를 의미한다.

	// 그러나 아무런 조건도 검사하지 않았을 수도 있으니, 
	// mandatory 조건을 검사했었는지만 check 한다.

	return bCheckedMandatoryCondition;
	*/

	return true;
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL LoadFindOption(CString fileName, std::vector<CString>* filter, std::vector<CString>* value, std::vector<bool> *mandatory)
{
/*
	std::ifstream ifile(fileName.operator LPCTSTR()); 
	std::string line; 
	while(std::getline(ifile, line)){
		std::vector<std::string> v = split<std::string, string2string>(line, '\t'); 
		if(v.size() != 3) continue; 
		CString v1 = v[0].c_str(); 
		CString v2 = v[1].c_str(); 
		CString v3 = v[2].c_str(); 
		if(strip(v[1], " \r\n\t").empty() == false){
			filter->push_back(v[0].c_str()); 
			value->push_back(v[1].c_str()); 
			mandatory->push_back(v[2] == "1"); 
		}
	}
*/
	return true;
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
CString FindCIFValue(CString cifFileName, CString key)
{
/*
	std::ifstream ifile(cifFileName.operator LPCTSTR()); 
	if(ifile.is_open() == false){
		return "";
	}
	std::string line; 
	while(std::getline(ifile, line)){
		std::vector<std::string> v = split<std::string, string2string>(line, '\t'); 
		if(v.size() != 2) continue; 
		if(v[0] == key.operator LPCTSTR()){
			return v[1].c_str(); 
		}
	}
*/

	return ""; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
std::vector<CString> GetPMIDListFromSearchResult(CString searchXMLFile)
{
	std::vector<CString> result; 
/*	std::ifstream ifile(searchXMLFile); 
	if(ifile.is_open() == false) return result; 
	std::string line; 
	while(std::getline(ifile, line)){
		std::string L = strip(line, "\t \n"); 
		if(L == "</IdList>") break;
		if(L.substr(0,4) == "<Id>"){
			int i = 0; 
			int length = (int)L.size(); 
			int opening_greater_pos = L.find('>'); 
			int closing_less_pos = L.rfind('<'); 
			if( opening_greater_pos != std::string::npos && closing_less_pos != std::string::npos &&
				opening_greater_pos < closing_less_pos)
			{
				std::string pmid = L.substr(opening_greater_pos+1, closing_less_pos - opening_greater_pos - 1); 
				result.push_back(pmid.c_str()); 
			}
		}
	}
*/

	return result; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
CString MakeTempFileName(CString ext)
{
	LARGE_INTEGER li = { 0 }; 
	QueryPerformanceCounter(&li); 
	srand(li.LowPart); 
	int i = 0; 
	CString fileName = ""; 
	for(i = 0; i<20; i++){
		char c = rand()%26; 
		if(rand()%2){
			c += 65; // 'A'
		}
		else{
			c += 97; // 'a'
		}
		fileName += c; 		
	}

	return fileName + ext; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL UpdateCIFValue(CString cifFileName, CString fieldName, CString value, bool bForce)
{
	value.TrimLeft(); value.TrimRight();
	if(value.IsEmpty() == true && bForce == false) return false; 
	std::ifstream ifile(cifFileName.operator LPCTSTR()); 
	if(ifile.is_open() == false) return false; 
	std::string line; 
	std::map<CString,CString> content; 
	while(std::getline(ifile, line)){
		std::vector<std::string> v = split<std::string, string2string>(line, '\t'); 
		if(v.size() != 2) continue; 
		content.insert(std::make_pair(v[0].c_str(), v[1].c_str())); 
	}
	ifile.close(); 
	DeleteFile(cifFileName); 

	content[fieldName] = value; 

	std::ofstream ofile(cifFileName.operator LPCTSTR(), std::ios::trunc); 
	std::map<CString, CString>::iterator pos = content.begin(); 
	if(ofile.is_open() == false) return false; 
	for(; pos != content.end(); pos++){
		// 강제로 값을 쓰는데 쓸 값이 없다면 field name조차 쓰지 않고 넘어간다.
		if(bForce == true && pos->second.IsEmpty() == true) continue; 
		ofile << pos->first.operator LPCTSTR() << '\t' << pos->second.operator LPCTSTR() << std::endl; 
	}

	ofile.close(); 
	SetFileAttributes(cifFileName, GetFileAttributes(cifFileName) | FILE_ATTRIBUTE_HIDDEN); 

	return true; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL GetAllChildNode(CTreeCtrl* tree, HTREEITEM hNode, std::vector<HTREEITEM>* dest)
{
//	TRACE("GetAllChildNode\n"); 
	HTREEITEM hNextItem = NULL; 
	HTREEITEM hChildItem = tree->GetChildItem(hNode); 
	while(hChildItem != NULL){
		hNextItem = tree->GetNextItem(hChildItem, TVGN_NEXT); 
		dest->push_back(hChildItem); 
		hChildItem = hNextItem; 
	}

	return true; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
CString remove_tag(CString line)
{
	CString result = ""; 
	int length = line.GetLength(); 
	int i = 0; 
	bool is_in_tag = false; 
	for(i = 0; i<length; i++){
		if(line[i] == '<'){
			is_in_tag = true; 
			continue; 
		}
		if(line[i] == '>'){
			is_in_tag = false; 
			continue; 
		}
		if(is_in_tag == false){
			result += line[i]; 
		}
	}

	return result; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL FindKeyWord(std::set<CString> *dest,  std::vector<CString>* keyword, CString title, bool bExact)
{
	// title에 있는 것을 space(' ', ASCII code 32)로 분리하여
	// [title_keyword.txt]에 등록되어 있는 것만 취한다.
	title.MakeLower(); 
	std::vector<std::string> word_list = split<std::string, string2string>(title.operator LPCTSTR(), ' '); 
	std::vector<CString> word_list_in_title;
	std::vector<std::string>::iterator wpos = word_list.begin(); 
	for( ; wpos != word_list.end(); wpos++){
		CString k = wpos->c_str(); 
		k.TrimLeft(); k.TrimRight(); 
		if(k.IsEmpty() == true) continue; 
		word_list_in_title.push_back(k); 
	}

	// 제목에 있는 단어 중 [keyword]를 갖고 포함하는 단어를 선별한다. 
	std::vector<CString>::iterator pos = word_list_in_title.begin(); 
	for(; pos != word_list_in_title.end(); pos++){
		bool contain_keyword = false; 
		std::vector<CString>::iterator kpos = keyword->begin(); 
		for(; kpos != keyword->end(); kpos++){
			if(*kpos == *pos){
				contain_keyword = true; 
				break; 
			}
			// 여기로 왔다는 것은 똑같지는 않다는 뜻
			if(bExact == true) continue; 
			if(pos->GetLength() < kpos->GetLength()) continue; 
			int find_pos = pos->Find(*kpos); 
			if(find_pos > 0){
				contain_keyword = true; 
				break; 
			}
			// 제목에 있는 단어가 keyword를 갖고 있는데, 
			// 뒤쪽에 알파벳이 아닌 기호가 오는 것은 상관이 없다.
			else if(find_pos == 0){
				if(pos->GetLength() > kpos->GetLength()){
					char c = pos->operator[](kpos->GetLength()); 
					bool c_is_alpha = ('a' <= c && c <='z') || ('A' <= c && c <= 'Z'); 
					if(c == 's' || c_is_alpha == false){
						contain_keyword = true; 
						break; 
					}
				}
			}
		}

		if(contain_keyword == true){
			dest->insert(*pos); 
			cprintf("keyword in title:\t%s <---- %s\n", *pos, *kpos); 
		}

	}
	

	return true; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
bool is_invalid_title_word(std::string word)
{
	int i = 0; 
	int length = (int)word.size(); 
	if(length <= 3) return true;
	if(atoi(word.c_str()) > 0) return true;
	for(i = 0; i<length; i++){
		char c = word[i]; 
		bool is_alpha_num = ('0' <= c && c <= '9') || ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'); 
		if(is_alpha_num == false){
			if(c == '-' || c == '\''){
//				return true;
				continue; 
			}
			else{
				return true;
			}
		}
	}

	return false; 

}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
bool word_length_func(const std::string& op1, const std::string& op2)
{
	return op1.size() > op2.size(); 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
int combi(int r, int n)
{
	if( r == n && r >= 1) return 1;
	if( n >= 1 && r == 1) return n; 
	if( r > (int)(n/2) ) return combi(n-r, n); 

	if(n == 4 && r == 2) return 6; 

	if(n == 5 && r == 2) return 10; 

	if(n == 6 && r == 2) return 15; 
	if(n == 6 && r == 3) return 20; 

	if(n == 7 && r == 2) return 21; 
	if(n == 7 && r == 3) return 35; 

	if(n == 8 && r == 2) return 28; 
	if(n == 8 && r == 3) return 56; 
	if(n == 8 && r == 4) return 70; 

	if(n == 9 && r == 2) return 36; 
	if(n == 9 && r == 3) return 84; 
	if(n == 9 && r == 4) return 126; 

	if(n == 10 && r == 2) return 45; 
	if(n == 10 && r == 3) return 120; 
	if(n == 10 && r == 4) return 210; 
	if(n == 10 && r == 5) return 252; 


	return 0;
}
bool combination(std::vector<int>* src, std::vector<std::vector<int> > *dest, int r, int n)
{

	int i = 0; 
	vector<int> s(r); 
	for(i = 1; i<=r; i++){
		s[i-1] = i-1; 
	}
	vector<int>::iterator pos = s.begin(); 
	vector<int> ve; 
	for(; pos != s.end(); pos++){
		ve.push_back(src->operator[](*pos)); 
	}
	dest->push_back(ve); 
//	while(next_permutation(ve.begin(), ve.end())){
//		dest->push_back(ve); 
//	}

	//print_vector(&ve); 

	int m = 0;
	int max_val = 0; 

	int count = combi(r,n);

	for(i = 2; i<=count; i++){
		m = r; 
		max_val = n-1; 
		while( s[m-1] == max_val){
			m--; 
			max_val--;
		}
		s[m-1] = s[m-1]+1;
		int j = 0;
		for(j = m+1; j<=r; j++){
			s[j-1] = s[j-2]+1;
		}

		pos = s.begin(); 
		ve.clear(); 
		for(; pos != s.end(); pos++){
			ve.push_back(src->operator[](*pos)); 
		}
		dest->push_back(ve); 
//		while(next_permutation(ve.begin(), ve.end())){
//			dest->push_back(ve); 
//		}
		//print_vector(&ve);

	}

	//cout << "end of combination" << endl;


	return true;
}
