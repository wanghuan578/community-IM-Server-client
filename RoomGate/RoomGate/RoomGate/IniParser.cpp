
#include "IniParser.h"

#include <assert.h>


using namespace std;

CIniParser::CIniParser(CString file_path)
{
	FilePath_ = file_path;
}

CIniParser::~CIniParser()
{

}

bool CIniParser::GetProfileValue(CString app_name, CString key_name, CString default_value, string &value)
{
	if (GetPrivateProfileString(app_name, key_name, default_value, ReadBuffer_.GetBuffer(MAX_PATH), MAX_PATH, FilePath_))
	{
		value = WChar2Ansi(ReadBuffer_.GetBuffer(ReadBuffer_.GetLength()));

		return true;
	}
	else
	{
		assert(0);

		return false;
	}
}

bool CIniParser::GetProfileValue(CString app_name, CString key_name, CString default_value, int &value)
{
	if (GetPrivateProfileString(app_name, key_name, default_value, ReadBuffer_.GetBuffer(MAX_PATH), MAX_PATH, FilePath_))
	{
		value = _wtoi(ReadBuffer_);

		return true;
	}
	else
	{
		assert(0);

		return false;
	}
}

bool CIniParser::SetProfileValue(CString app_name, CString key_name, CString &value)
{
	if (WritePrivateProfileString(app_name, key_name, value, FilePath_))
	{
		return true;
	}
	else
	{
		assert(0);

		return false;
	}
}

bool CIniParser::SetProfileValue(CString app_name, CString key_name, int value)
{
	wchar_t buff[32];

	_itow(value, buff, 10);

	CString w_value(buff);

	if (WritePrivateProfileString(app_name, key_name, w_value, FilePath_))
	{
		return true;
	}
	else
	{
		assert(0);

		return false;
	}
}

string CIniParser::WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen<= 0)
	{
		return std::string("");
	}

	char* pszDst = new char[nLen];

	if (NULL == pszDst)
	{
		return std::string("");
	}

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);

	pszDst[nLen -1] = 0;

	string strTemp(pszDst);

	delete [] pszDst;

	return strTemp;
}