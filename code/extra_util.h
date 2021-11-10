#ifndef __DEFINITION_OF_EXTRA_UTILITY_FUNCTIONS__
#define __DEFINITION_OF_EXTRA_UTILITY_FUNCTIONS__

#include <shlwapi.h>
#include <shlobj.h>


#include <set>
#include <vector>
#include <string>

#include <conio.h>




CString GetMyDocumentLabel(int *sysIconIndex);
CString GetMyComputerLabel(int *sysIconIndex);
CString GetDesktopLabel(int *sysIconIndex); 
BOOL DirectoryHasSubfolder(CString fullPath); 
BOOL GetFirstSubfolderName(CString fullPath, CString* subfolder); 
BOOL CheckInformation(CString cifFileName, std::vector<CString>* filter, std::vector<CString>* value, std::vector<bool> *mandatory);
BOOL LoadFindOption(CString fileName, std::vector<CString>* filter, std::vector<CString>* value, std::vector<bool> *mandatory);
CString FindCIFValue(CString cifFileName, CString key);
std::vector<CString> GetPMIDListFromSearchResult(CString searchXMLFile); 
CString MakeTempFileName(CString ext = ""); 
BOOL UpdateCIFValue(CString cifFileName, CString fieldName, CString value, bool bForce = false);
BOOL GetAllChildNode(CTreeCtrl* tree, HTREEITEM hNode, std::vector<HTREEITEM>* dest);
CString remove_tag(CString line); 
BOOL FindKeyWord(std::set<CString> *dest,  std::vector<CString>* keyword, CString title, bool bExact);
bool is_invalid_title_word(std::string title); 
bool word_length_func(const std::string& op1, const std::string& op2); 
int combi(int r, int n);
bool combination(std::vector<int>* src, std::vector<std::vector<int> > *dest, int r, int n);



#endif