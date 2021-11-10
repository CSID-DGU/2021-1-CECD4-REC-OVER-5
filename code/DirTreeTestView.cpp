// DirTreeTestView.cpp : implementation of the CDirTreeTestView class
//

#include "stdafx.h"
#include "DirTreeTest.h"

#include "DirTreeTestDoc.h"
#include "DirTreeTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView

IMPLEMENT_DYNCREATE(CDirTreeTestView, CView)

BEGIN_MESSAGE_MAP(CDirTreeTestView, CView)
	//{{AFX_MSG_MAP(CDirTreeTestView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView construction/destruction

CDirTreeTestView::CDirTreeTestView()
{
	// TODO: add construction code here

}

CDirTreeTestView::~CDirTreeTestView()
{
}

BOOL CDirTreeTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView drawing

void CDirTreeTestView::OnDraw(CDC* pDC)
{
	CDirTreeTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView printing

BOOL CDirTreeTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDirTreeTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDirTreeTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView diagnostics

#ifdef _DEBUG
void CDirTreeTestView::AssertValid() const
{
	CView::AssertValid();
}

void CDirTreeTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDirTreeTestDoc* CDirTreeTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDirTreeTestDoc)));
	return (CDirTreeTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestView message handlers

void CDirTreeTestView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	

	if(!::IsWindow(_tree.GetSafeHwnd())){
		CRect rect;
		GetClientRect(&rect); 
		_tree.Create(WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0,0,300,rect.Height()), this, 101020);  
  		_tree.SetRootDirectory(""); 

	}
	
}
