#ifndef __DEFINITION_OF_UTIL_H__
#define __DEFINITION_OF_UTIL_H__

#include <string>
#include <vector>
#include <map>
#include <vector>

using namespace std; 


map<string,string> argument_unpack(int argc, const char * argv[]);
string strip(const string str, string wc);


class string2double{
public:
	double operator()(const string& str);
};
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
class string2string{
public:
	string operator()(const string& str);
};
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
class string2int{
public:
	int operator()(const string& str);
};
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
template<class T, class functor>
vector<T> split(string line, char delimit)
{
	functor f; 
	vector<T> result; 
	line = strip(line, "\r\n"); 
	string str(1, delimit); 
	line = strip(line, str); 
	line += delimit; // 제일 마지막 원소를 놓치지 않기 위한 trick. 
	int length = line.size(); 
	int i = 0; 
	int pre_pos = 0; 
	for(i = 0; i<length; i++){
		if(line[i] == delimit || line[i] == '\r' || line[i] == '\n'){
			string str = line.substr(pre_pos, i - pre_pos); 
			pre_pos = i + 1; 
			T value = f(str); 
			//double value = atof(str.c_str()); 
			result.push_back(value); 
		}
	}

	//cout << endl;


	return result; 
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
#endif
