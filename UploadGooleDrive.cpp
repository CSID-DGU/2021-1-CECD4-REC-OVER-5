#include "includePython.h"
#include <iostream>

using namespace std;

int main() {

	PyObject *pName, *pModule, *pFunc, *pValue;
	Py_Initialize();

	pName = PyUnicode_FromString("UploadToGoogleDrive"); //.py ���� �̸�

	pModule = PyImport_Import(pName); // ������ PyObject pName�� import�Ѵ�.

	if (pModule == NULL) {
		cout << "���� ����" << endl;
		return 0;
	}

	pFunc = PyObject_GetAttrString(pModule, "uploadToGoogleDrive"); //py������ ������ �Լ� �̸�

	
	pValue = PyObject_CallObject(pFunc, NULL); // pFunc�� �Ű������� �����ؼ� �����Ѵ�. ���� �Ű������� NULL�� ����̴�.
	

	Py_Finalize();
	
	return 0;
}