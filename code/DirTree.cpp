// DirTree.cpp : implementation file
//

#include "stdafx.h"
#include "DirTree.h"
#include "resource.h"

#include "extra_util.h"

#include <shlwapi.h>
#include <shlobj.h>

#include <fstream>
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTree

CDirTree::CDirTree()
{
}

CDirTree::~CDirTree()
{
	if(_smallImageList.GetSafeHandle()){
		_smallImageList.Detach(); 
	}
	if(_largeImageList.GetSafeHandle()){
		_largeImageList.Detach(); 
	}
}


BEGIN_MESSAGE_MAP(CDirTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CDirTree)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemExpanded)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTree message handlers
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::CreateSystemImageList(CImageList *pImageList, CString rootDir, BOOL bLargeIcon)
{
	ASSERT(pImageList  != NULL); 
	pImageList->Detach(); 
	pImageList->DeleteImageList();
	SHFILEINFO sfi = { 0 }; 
	CImageList *pSysImageList = NULL; 
	if(bLargeIcon == TRUE){
		pImageList->Attach((HIMAGELIST)SHGetFileInfo("C:\\", 0, &sfi, sizeof(sfi), SHGFI_LARGEICON | SHGFI_SYSICONINDEX));
	}
	else{
		pImageList->Attach((HIMAGELIST)SHGetFileInfo("C:\\", 0, &sfi, sizeof(sfi), SHGFI_SMALLICON | SHGFI_SYSICONINDEX));
	}
	return true; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
int CDirTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1){
		return -1;
	}
	

	long style = GetWindowLong(this->GetSafeHwnd(),GWL_STYLE) | TVS_HASBUTTONS | TVS_HASLINES | TVS_SHOWSELALWAYS | TVS_LINESATROOT | TVS_TRACKSELECT;
	SetWindowLong(this->GetSafeHwnd(),GWL_STYLE,style);

	return 0;
}
//----------------------------------------------------------------------------------------------------------------------//
// tree control의 root node 를 [rootDir]로 설정한다. 
// 만약 [rootDir]이 공백일 경우 '바탕화면'에 해당하는 경로를 root 로 설정한다. 
//----------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::SetRootDirectory(CString rootDir)
{
	DeleteAllItems(); 
	CreateSystemImageList(&_smallImageList, rootDir, FALSE); 
	CreateSystemImageList(&_largeImageList, rootDir, TRUE); 
	SetImageList(&_smallImageList, TVSIL_NORMAL); 

	UINT nFlags = SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_SYSICONINDEX; 
	SHFILEINFO sfi = { 0 }; 
	if(rootDir == ""){
		int sysIconIndex = -1; 
		// 'Desktop' or '바탕화면' text
		CString label = GetDesktopLabel(&sysIconIndex); 
		HTREEITEM hRoot = InsertItem(label, sysIconIndex,sysIconIndex, TVI_ROOT); 
		// 'My Document' or '내 문서' text
		label = GetMyDocumentLabel(&sysIconIndex); 
		HTREEITEM hItem = InsertItem(label, sysIconIndex,sysIconIndex, hRoot); 		
		TCHAR desktop_path[MAX_PATH] = ""; 
		SHGetSpecialFolderPath(NULL,desktop_path,CSIDL_DESKTOPDIRECTORY,0);
		CString subfolder; 
		if(GetFirstSubfolderName(desktop_path,&subfolder) == true){
			CString first_subfolder; 
			first_subfolder.Format("%s\\%s",desktop_path, subfolder); 
			SHGetFileInfo(first_subfolder, 0, &sfi, sizeof(sfi), nFlags); 
			InsertItem(subfolder, sfi.iIcon, sfi.iIcon, hItem); 
		}



		// 'My Computer' or '내 컴퓨터' text
		label = GetMyComputerLabel(&sysIconIndex); 
		hItem = InsertItem(label, sysIconIndex,sysIconIndex, hRoot);
		ShowDrives(hItem); 

	}
	else{
		SHGetFileInfo(rootDir, 0, &sfi, sizeof(sfi), nFlags); 
		HTREEITEM hRoot = InsertItem(rootDir, sfi.iIcon, sfi.iIcon, TVI_ROOT); 
		ShowDirectoryInTree(hRoot); 
		this->SelectItem(hRoot); 
		Expand(GetRootItem(),TVE_TOGGLE); 
	}
	return true; 
}
//-----------------------------------------------------------------------------------------------------------------------------//
// [hNode]의 모든 하위 노드를 찾아서 [dest]에 집어 넣는다. 
//-----------------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::GetAllChildNode(HTREEITEM hNode, std::vector<HTREEITEM>* dest)
{
	HTREEITEM hNextItem = NULL; 
	HTREEITEM hChildItem = this->GetChildItem(hNode); 
	while(hChildItem != NULL){
		hNextItem = this->GetNextItem(hChildItem, TVGN_NEXT); 
		dest->push_back(hChildItem); 
		hChildItem = hNextItem; 
	}

	return true; 
}
//-----------------------------------------------------------------------------------------------------------------------------//
// [hNode]의 모든 1단계 하위 노드의 텍스트를 [dest]에 집어 넣는다.
//-----------------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::GetChildNodeName(HTREEITEM hNode, std::set<CString> *dest)
{
	if(this->ItemHasChildren(hNode) == false) return true; 

	std::vector<HTREEITEM> all_child_node; 
	GetAllChildNode(hNode, &all_child_node); 
	std::vector<HTREEITEM>::iterator pos = all_child_node.begin(); 
	for(; pos != all_child_node.end(); pos++){
		CString itemText = this->GetItemText(*pos); 
		dest->insert(itemText); 	
	}

	return true; 
}
//-----------------------------------------------------------------------------------------------------------------------------//
// [hParentItem]에 컴퓨터에 있는 드라이브를 찾아서 붙여 준다.
//-----------------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::ShowDrives(HTREEITEM hParentItem)
{
	std::set<CString> child_node_name; 
	// Get all child nodes attached to [hParentItem]. 
	GetChildNodeName(hParentItem, &child_node_name); 

	DWORD logicalDrives = GetLogicalDrives(); 
	unsigned int i = 64; 
	do{
		i++; 
		if((logicalDrives & 1) != 0){
			CString strDrive = ""; 
			strDrive.Format("%c:\\",i); 
			UINT driveType = GetDriveType(strDrive); 
 
			SHFILEINFO sfi = { 0 }; 
			SHGetFileInfo(strDrive, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX); 
			if(child_node_name.count(sfi.szDisplayName) == 0){
				HTREEITEM hItem = InsertItem(sfi.szDisplayName, sfi.iIcon, sfi.iIcon, hParentItem); 
				CString subfolder; 
				// CD-ROM이나 flopy disk는 하위 폴더를 표시하지 않는다.
				if(driveType == DRIVE_REMOVABLE || driveType == DRIVE_CDROM) continue; 
				if(GetFirstSubfolderName(strDrive,&subfolder) == true){
					SHGetFileInfo(strDrive+subfolder, 0, &sfi, sizeof(sfi), SHGFI_DISPLAYNAME | SHGFI_SYSICONINDEX); 
					InsertItem(subfolder, sfi.iIcon, sfi.iIcon, hItem); 
				}
			}
		}
	}while((logicalDrives >>= 1) != 0);

	return true; 
}
//-----------------------------------------------------------------------------------------------------------------------------//
// [hParentItem]에 하위 폴더를 집어 넣는다.
//-----------------------------------------------------------------------------------------------------------------------------//
BOOL CDirTree::ShowDirectoryInTree(HTREEITEM hParentItem)
{
	CString fullRootPath = GetFullNodeName(hParentItem); 
	std::set<CString> child_node_name; 
	// Get all child nodes attached to [hParentItem]. 
	GetChildNodeName(hParentItem, &child_node_name); 

	if(fullRootPath == "My Computer"){
		ShowDrives(hParentItem); 
		return true;
	}

	std::vector<CString> node_name_list; 
	std::vector<bool> is_node_directory; 
	WIN32_FIND_DATA fd = { 0 }; 
	HANDLE hSearch = NULL; 
	BOOL bFinish = false; 
	if(fullRootPath[1] == ':' && fullRootPath.GetLength() == 3){
		fullRootPath = fullRootPath.Left(2); 
	}

	// 1단계: [fullRootPath]의 하위 폴더를 모두 [node_name_list]에 집어 넣는다.
	TRACE("Seraching subfolder and files...: %d:%d\n", CTime::GetCurrentTime().GetMinute(), CTime::GetCurrentTime().GetSecond()); 
	hSearch = FindFirstFile(fullRootPath  + "\\*.*", &fd); 
	while(!bFinish){
		// If there is already a node below [hParentItem], 
		// skip adding the node to [hParentItem]. 
		if(child_node_name.count(fd.cFileName) == 1){
			bFinish = !FindNextFile(hSearch, &fd); 
			continue; 
		}
		bool is_current_node_directory = false; 
		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY){
			if( (_tcslen(fd.cFileName) == 1 && fd.cFileName[0] == '.') ||
				(_tcslen(fd.cFileName) == 2 && fd.cFileName[0] == '.' && fd.cFileName[1] == '.') ||
				(PathIsSystemFolder(fullRootPath+"\\"+fd.cFileName, NULL) == true)
				){
				bFinish = !FindNextFile(hSearch, &fd); 
				continue; 
			}
			is_current_node_directory = true; 
		}
		node_name_list.push_back(fd.cFileName);
		is_node_directory.push_back(is_current_node_directory); 
		bFinish = !FindNextFile(hSearch, &fd); 
	}
	FindClose(hSearch); 

	// 2단계: [node_name_list]에 있는, [fullRootPath]의 모든 하위 폴더를
	// [hParentItem] node 밑에 집어 넣는다.
	TRACE("Inserting subfolders...: %d:%d\n", CTime::GetCurrentTime().GetMinute(), CTime::GetCurrentTime().GetSecond()); 
	UINT nFlags = SHGFI_DISPLAYNAME | SHGFI_TYPENAME | SHGFI_SYSICONINDEX; 
	SHFILEINFO sfi = { 0 }; 
	// Add subfolders of [hParentItem] below it. 
	// Insert a subfolder(grand-subfolder?), if any, to each subfolder, that is,
	// by doing this we can make each subfolder containing folders to have '+' button. 
	int i = 0; 
	for(i = 0; i<node_name_list.size() ; i++){
		if(is_node_directory[i] == true){
			CString path = fullRootPath + "\\" + node_name_list[i];
			SHGetFileInfo(path, 0, &sfi, sizeof(sfi), nFlags); 
			HTREEITEM hItem = InsertItem(node_name_list[i], sfi.iIcon, sfi.iIcon, hParentItem); 
		}
	}

	// 3단계: 각 하위 폴더의 하위 폴더를 집어 넣는다. 
	// 이 단계를 수행해야 하위 폴더를 갖고 있는 폴더 옆에 '+' 표시가
	// 생겨서 폴더가 더 있다는 것을 알 수 있다. 따라서 이 단계에서 이 작업을
	// 수행해 주자. 
	TRACE("Adding + buttons...: %d:%d\n", CTime::GetCurrentTime().GetMinute(), CTime::GetCurrentTime().GetSecond()); 
	std::vector<HTREEITEM> all_child_node; 
	GetAllChildNode(hParentItem, &all_child_node); 
	std::vector<HTREEITEM>::iterator pos = all_child_node.begin(); 
	for(; pos != all_child_node.end(); pos++){
		CString fullChildPath = GetFullNodeName(*pos); 
		CString subfolder; 
		if(GetFirstSubfolderName(fullChildPath,&subfolder) == true){
			std::set<CString> all_grand_child_name; 
			GetChildNodeName(*pos, &all_grand_child_name); 
			if(all_grand_child_name.count(subfolder) == 0){
				SHGetFileInfo(fullChildPath+"\\"+subfolder, 0, &sfi, sizeof(sfi), nFlags); 
				InsertItem(subfolder, sfi.iIcon, sfi.iIcon, *pos); 
			}
		}
	}

	return true; 
}
//----------------------------------------------------------------------------------------------------------------------//
// Given a node, make a string such that text of parent node precedes
// it's child node, and there is one '\' between the text of parent node and of 
// a child node. For example, suppose that there is a tree which looks like this: 
//
// D:\
//  |
//  +---NCBIDATA
//         |
//         +---H_sapiens
//                 |
//                 +---chr1
//
// and [hNode] points to [chr1]. Then full node name of [hNode] is
// [D:\NCBIDATA\H_sapiens\chr1].
//----------------------------------------------------------------------------------------------------------------------//
CString CDirTree::GetFullNodeName(HTREEITEM hNode)
{
	
	CString path("");

	HTREEITEM hRoot = GetRootItem();
	if(hNode == hRoot){
		TCHAR path[MAX_PATH] = ""; 
		SHGetSpecialFolderPath(NULL,path,CSIDL_DESKTOPDIRECTORY,0);
		return path; 
	}

	CString myDocumentLabel = GetMyDocumentLabel(NULL); 
	CString myComputerLabel = GetMyComputerLabel(NULL); 
	CString desktopLabel = GetDesktopLabel(NULL); 

	if(hRoot == GetParentItem(hNode)){
		CString currentItemText = GetItemText(hNode); 
		// Check if the current node is for 'My Document.'
		if(currentItemText == myDocumentLabel){
			TCHAR path[MAX_PATH] = ""; 
			SHGetSpecialFolderPath(NULL,path,CSIDL_PERSONAL,0);
			return path; 
		}
		// Check if the current node is for 'My Computer.'
		if(currentItemText == myComputerLabel){
			// All entries can't have 'My Computer' except for 'My Computer' node. 
			// 
			return "My Computer"; 
		}

		// Now that all entries we meet here are in desktop folder. 
		TCHAR path[MAX_PATH] = ""; 
		SHGetSpecialFolderPath(NULL,path,CSIDL_DESKTOPDIRECTORY,0);
		CString fullPath(path); 
		fullPath += ("\\" + currentItemText); 
		return fullPath;
	}




	// CListCtrl::GetParentItem(hNODE) function returns NULL when [hNODE] is a root item. 
	// Each text of parent item will be inserted at the beginning of [path]. 
	while(hNode != NULL){
		CString itemText = GetItemText(hNode); 
		if(itemText == myComputerLabel || itemText == desktopLabel || itemText == myDocumentLabel){
			if(itemText == myComputerLabel){
				CString drive ="";
				CString folder = ""; 
				if(path.Find('\\') != -1){
					drive = path[path.Find('\\')-3];//
					drive += ":\\"; 
					folder = path.Right(path.GetLength() - path.Find('\\')-1); 
				}
				else{
					drive = path[path.GetLength()-3];//+":\\"; 
					drive += ":\\"; 
				}


				if(folder != ""){
					path = drive + folder; 
				}
				else{
					path = drive; 
				}
			}
			else if(itemText == desktopLabel){
				TCHAR desktop_path[MAX_PATH] = ""; 
				SHGetSpecialFolderPath(NULL,desktop_path,CSIDL_DESKTOPDIRECTORY,0);
				CString temp_path;
				temp_path.Format("%s\\%s",desktop_path, path); 
				path = temp_path; 
			}
			else if(itemText == myDocumentLabel){
				TCHAR myDocument_path[MAX_PATH] = ""; 
				SHGetSpecialFolderPath(NULL,myDocument_path,CSIDL_PERSONAL,0);
				CString temp_path;
				temp_path.Format("%s\\%s",myDocument_path, path); 
				path = temp_path; 
			}

			break;
		}

		if(path.IsEmpty())
			path = GetItemText(hNode);
		else{
			CString prePath(path);
			path = GetItemText(hNode);
			// When a root item is a node of DRIVE, it's text looks like "C:\"
			// And it is to be sured that there is only one '\' between text of 
			// parent and child nodes. 
			if(path[path.GetLength()-1] == '\\')
				path += prePath;
			else
				path += "\\" + prePath;
		}

		hNode = GetParentItem(hNode);
	
	}

	return path; 
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
void CDirTree::OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;


	HTREEITEM hCurItem = pNMTreeView->itemNew.hItem;
	TRACE("%s - expanded.\n", GetItemText(hCurItem)); 
	ShowDirectoryInTree(hCurItem); 

	
	*pResult = 0;
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
CString CDirTree::GetSelectedItemFullNodeName()
{
	HTREEITEM hSelTreeItem = GetSelectedItem(); 
	return GetFullNodeName(hSelTreeItem);
}
//----------------------------------------------------------------------------------------------------------------------//
//----------------------------------------------------------------------------------------------------------------------//
