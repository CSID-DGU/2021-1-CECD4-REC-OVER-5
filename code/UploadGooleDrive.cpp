#include "includePython.h"
#include <iostream>

using namespace std;

int main() {

	PyObject *pName, *pModule, *pFunc, *pValue;
	Py_Initialize();

	pName = PyUnicode_FromString("UploadToGoogleDrive"); //.py 파일 이름

	pModule = PyImport_Import(pName); // 생성한 PyObject pName을 import한다.

	if (pModule == NULL) {
		cout << "파일 없음" << endl;
		return 0;
	}

	pFunc = PyObject_GetAttrString(pModule, "uploadToGoogleDrive"); //py파일의 실행할 함수 이름

	
	pValue = PyObject_CallObject(pFunc, NULL); // pFunc에 매개변수를 전달해서 실행한다. 현재 매개변수가 NULL인 경우이다.
	

	Py_Finalize();
	
	return 0;
}
