#if !defined(AFX_DIRTREE_H__1FA7C585_66B4_4662_9458_188975AA37F3__INCLUDED_)
#define AFX_DIRTREE_H__1FA7C585_66B4_4662_9458_188975AA37F3__INCLUDED_





#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DirTree.h : header file
//


//#include "DirList.h"


#include <set>
#include <vector>


/////////////////////////////////////////////////////////////////////////////
// CDirTree window

class CDirTree : public CTreeCtrl
{
// Construction
public:
	CDirTree();

// Attributes
public:
private:
	
protected:
	CImageList			_smallImageList; 
	CImageList			_largeImageList; 
//	CDirList			*_pListCtrl; 



// Operations
public:
	BOOL SetRootDirectory(CString rootDir); 
	BOOL CreateSystemImageList(CImageList *pImageList, CString rootDir, BOOL bLargeIcon);
 	CString GetSelectedItemFullName(int index);
	CString GetFullNodeName(HTREEITEM hNode);
 	std::vector<CString> GetSelectedItemFullFileNameList();


private:
	BOOL ShowDirectoryInTree(HTREEITEM hParentItem);
	BOOL GetChildNodeName(HTREEITEM hNode, std::set<CString> *dest);
	BOOL GetAllChildNode(HTREEITEM hNode, std::vector<HTREEITEM>* dest);
	BOOL ShowDrives(HTREEITEM hParentItem);
	CString GetSelectedItemFullNodeName();



protected:
//	afx_msg LRESULT OnListDoubleClicked(WPARAM wParam, LPARAM lParam); 
//	afx_msg LRESULT OnListRDoubleClicked(WPARAM wParam, LPARAM lParam); 


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirTree)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREE_H__1FA7C585_66B4_4662_9458_188975AA37F3__INCLUDED_)
