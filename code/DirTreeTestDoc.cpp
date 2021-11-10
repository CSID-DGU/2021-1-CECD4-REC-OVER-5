// DirTreeTestDoc.cpp : implementation of the CDirTreeTestDoc class
//

#include "stdafx.h"
#include "DirTreeTest.h"

#include "DirTreeTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestDoc

IMPLEMENT_DYNCREATE(CDirTreeTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CDirTreeTestDoc, CDocument)
	//{{AFX_MSG_MAP(CDirTreeTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestDoc construction/destruction

CDirTreeTestDoc::CDirTreeTestDoc()
{
	// TODO: add one-time construction code here

}

CDirTreeTestDoc::~CDirTreeTestDoc()
{
}

BOOL CDirTreeTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestDoc serialization

void CDirTreeTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestDoc diagnostics

#ifdef _DEBUG
void CDirTreeTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDirTreeTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDirTreeTestDoc commands
