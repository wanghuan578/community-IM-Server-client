
#pragma once

#include<atlstr.h> 
#include <string>

using namespace std;

class CIniParser
{
public:
	CIniParser(CString file_path);

	~CIniParser();
	bool SetProfileValue(CString app_name, CString key_name, CString &value);
	bool SetProfileValue(CString app_name, CString key_name, int value);
	bool GetProfileValue(CString app_name, CString key_name, CString default_value, string &value);
	bool GetProfileValue(CString app_name, CString key_name, CString default_value, int &value);

protected:
	string WChar2Ansi(LPCWSTR pwszSrc);

private:
	CString FilePath_;
	CString ReadBuffer_;
};