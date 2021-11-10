#include "stdafx.h"
#include "util.h"

#include <set>


//---------------------------------------------------------------------------------------
// The first argc is a name of executible file. 
// Therefore, skip the first argv. 
// All elements of argv start with '-'. When pack it into result, remove the dash. 
// Policy: argc should be odd because every options are pair, that is, 
// (option_name, option_value). 
// 
//---------------------------------------------------------------------------------------
map<string,string> argument_unpack(int argc, const char * argv[])
{
	map<string,string> result;
	if(argc % 2 == 0) return result;
	for(int i = 1; i<argc; i+=2){
		string option_name = string(argv[i]); 
		option_name = option_name.substr(1, option_name.length()); 
		string option_value = string(argv[i+1]); 
		result.insert(make_pair(option_name, option_value));
	}
	
	
	return result;
}

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
string strip(const string str, string wc)
{
	int length = str.size();
	int i = 0; 
	int first_pos = 0;
	int last_pos = length - 1;
	set<char> white_char; 
	for(i = 0; i<wc.size(); i++){
		white_char.insert(wc[i]); 
	}

	// �տ��� ���鼭 [wc]�� ���� ���� ������ ù ��° ��ġ Ž��. 
	for(i = 0; i<length; i++){
		// white character�� �ƴϸ� ��� Ž��. 
		if(white_char.find(str[i]) != white_char.end()) continue; 
		if(white_char.find(str[i]) == white_char.end()){
			first_pos = i;
			//cout << "f" << first_pos << endl; 
			break;
		}
	}
	// �ڿ��� ������ ���鼭 [wc]�� ���� ���� ������ ù ��°
	// ��ġ Ž��. 
	for(i = length - 1; i>=0; i--){
		if(white_char.find(str[i]) != white_char.end()) continue; 
		if(white_char.find(str[i]) == white_char.end()){
			last_pos = i; 
			break; 
		}
	}

	if(first_pos > last_pos){
		first_pos = 0; 
		last_pos = length - 1;
	}

	return str.substr(first_pos, last_pos - first_pos + 1); 
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
double string2double::operator ()(const string& str)
{
	return atof(str.c_str()); 
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
string string2string::operator ()(const string& str)
{
	return str; 
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//
int string2int::operator ()(const string& str)
{
	return atoi(str.c_str()); 
}
//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//


