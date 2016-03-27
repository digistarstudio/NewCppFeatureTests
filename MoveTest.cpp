//
// MSDN article:
// Rvalue Reference Declarator: &&
// https://msdn.microsoft.com/en-us/library/dd293668.aspx
//


#include "stdafx.h"

#include <tchar.h>
#include <malloc.h>
#include <assert.h>
#include <cstring>
#include <iostream>


using namespace std;


class CString
{
public:

	CString()
	{
		cout << "---> Default constructor called!\n";
	}
	CString(const TCHAR* pStrIn, size_t lenIn = 0)
	{
		cout << "---> Constructor called! " << "Src: " << pStrIn << endl;
		StoreString(pStrIn, lenIn);
	}
	~CString()
	{
		if (m_pBuf != nullptr)
			free(m_pBuf);
	}

	CString(CString &&rvalue) // move constructor
	{
		cout << "---> Move constructor called!\n";
		m_pBuf = rvalue.m_pBuf;
		rvalue.m_pBuf = nullptr;
	}

	/*
	CString operator+(const TCHAR *strIn)
	{
	CString out = m_pBuf;
	out.CatString(strIn);
	return out;
	}
	//*/
	CString& operator=(CString &&rstr)
	{
		cout << "---> Move assignment called!\n";
		if (m_pBuf != nullptr)
			free(m_pBuf);

		m_pBuf = rstr.m_pBuf;
		m_len = rstr.m_len;
		rstr.m_pBuf = nullptr; // Must clean the pointer.
		rstr.m_len = 0;

		return *this;
	}
	CString& operator=(CString &src)
	{
		cout << "---> Copy assignment called!\n";
		StoreString(src.m_pBuf, 0);
		return *this;
	}

	void CatString(const TCHAR* pStrIn, uint32_t lenIn = 0)
	{
		if (pStrIn != nullptr && lenIn == 0)
			lenIn = _tcslen(pStrIn);
		if (m_pBuf == nullptr)
		{
			m_len = lenIn + 1;
			m_pBuf = (TCHAR*)malloc(sizeof(TCHAR) * m_len);
			m_pBuf[0] = '\0';
		}
		else
		{
			m_len += lenIn;
			TCHAR *pNewBuf = (TCHAR*)realloc(m_pBuf, sizeof(TCHAR) * m_len);
			if (pNewBuf == NULL)
				return;
			m_pBuf = pNewBuf;
		}
#ifdef  _MSC_VER
		_tcscat_s(m_pBuf, sizeof(TCHAR) * m_len, pStrIn);
#else
		_tcscat(m_pBuf, pStrIn);
#endif
	}
	void StoreString(const TCHAR* pStrIn, uint32_t lenIn)
	{
		if (pStrIn != nullptr && lenIn == 0)
			lenIn = _tcslen(pStrIn);
		if (m_pBuf == nullptr)
		{
			m_len = lenIn + 1;
			m_pBuf = (TCHAR*)malloc(sizeof(TCHAR) * m_len);
		}
		else if (m_len != lenIn + 1)
		{
			m_pBuf = (TCHAR*)realloc(m_pBuf, sizeof(TCHAR) * (lenIn + 1));
			assert(m_pBuf != nullptr);
			m_len = lenIn + 1;
		}
		memcpy(m_pBuf, pStrIn, sizeof(TCHAR) * lenIn);
		m_pBuf[lenIn] = '\0';
	}


	operator const TCHAR*() { return m_pBuf; }


protected:

	friend CString operator+(CString& rstr, const TCHAR *pTChar);

	TCHAR*   m_pBuf = nullptr;
	uint32_t m_len = 0;   // character count.


};


//*
CString operator+(CString& rstr, const TCHAR *pTChar)
{
	CString out(rstr.m_pBuf);
	out.CatString(pTChar);
	return out;
}
CString&& operator+(CString&& rstr, const TCHAR *pTChar)
{
	rstr.CatString(pTChar);
	return (CString&&)rstr;
}
//*/


int main()
{
	CString testA = _T("testA"), testB = _T("testB"), testC, testD, testE;

	testC = testA + _T(" CCC"); // I find that GCC does better here. :p

	testD = CString(_T("TempStr: ")) + _T("DDD") + _T(" DDD2");

	testE = testB;

	cout << _T("testA: ") << testA << endl;
	cout << _T("testB: ") << testB << endl;
	cout << _T("testC: ") << testC << endl;
	cout << _T("testD: ") << testD << endl;
	cout << _T("testE: ") << testE << endl;


	CString testF(_T("testF")), testG;
	testG = std::move(testF);
	assert(testF == nullptr);
	cout << _T("testG: ") << testG << endl;

	testF = (CString&&)testG;
	assert(testG == nullptr);
	cout << _T("testF: ") << testF << endl;

	// Don't do this.
//	TCHAR *pTest = nullptr;
//	cout << pTest;

	return 0;
}


