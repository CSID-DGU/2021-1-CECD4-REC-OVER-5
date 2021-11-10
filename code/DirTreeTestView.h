// DirTreeTestView.h : interface of the CDirTreeTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_DIRTREETESTVIEW_H__CE054E25_33F4_4962_A3EC_F23BBE77658E__INCLUDED_)
#define AFX_DIRTREETESTVIEW_H__CE054E25_33F4_4962_A3EC_F23BBE77658E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DirTree.h"

class CDirTreeTestView : public CView
{
protected: // create from serialization only
	CDirTreeTestView();
	DECLARE_DYNCREATE(CDirTreeTestView)

// Attributes
public:
	CDirTreeTestDoc* GetDocument();
	CDirTree		_tree; 

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTreeTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirTreeTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CDirTreeTestView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in DirTreeTestView.cpp
inline CDirTreeTestDoc* CDirTreeTestView::GetDocument()
   { return (CDirTreeTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIRTREETESTVIEW_H__CE054E25_33F4_4962_A3EC_F23BBE77658E__INCLUDED_)
