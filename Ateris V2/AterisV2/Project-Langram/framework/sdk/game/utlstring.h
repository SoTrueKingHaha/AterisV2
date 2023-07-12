#pragma once

#include "utlvector.h"

class CUtlString
{
public:
    [[nodiscard]] char* Get() const
    {
        return reinterpret_cast<char*>(m_Memory.m_pMemory);
    }

    void Set(const char* pValue) const
    {
        memset(m_Memory.m_pMemory, reinterpret_cast<int>(pValue), sizeof pValue);
    }

    CUtlString& operator=(const CUtlString& src)
    {
        m_Memory.m_pMemory = src.m_Memory.m_pMemory;
        return *this;
    }

    CUtlString& operator=(const char* src)
    {
        Set(src);
        return *this;
    }

    bool operator==(const CUtlString& src) const
    {
        return m_Memory.m_pMemory == src.m_Memory.m_pMemory;
    }

    bool operator==(const char* src) const
    {
        return strcmp(Get(), src) == 0;
    }

    operator const char* () const
    {
        return Get();
    }

    const char* String() const { return Get(); }

    int Length() const
    {
        return m_nActualLength;
    }

    bool IsEmpty() const
    {
        return Length() == 0;
    }

    CUtlMemory<unsigned char> m_Memory;
    int                       m_nActualLength;
};

template < typename T >
class StringFuncs
{
public:
	static T* Duplicate(const T* pValue);
	static void		 Copy(T* out_pOut, const T* pIn, int iLength);
	static int		 Compare(const T* pLhs, const T* pRhs);
	static int		 Length(const T* pValue);
	static const T* FindChar(const T* pStr, const T cSearch);
	static const T* EmptyString();
};

template < >
class StringFuncs<char>
{
public:
	static char* Duplicate(const char* pValue) { return _strdup(pValue); }
	static void		   Copy(char* out_pOut, const char* pIn, int iLength) { strncpy_s(out_pOut, iLength, pIn, iLength); }
	static int		   Compare(const char* pLhs, const char* pRhs) { return strcmp(pLhs, pRhs); }
	static int		   Length(const char* pValue) { return strlen(pValue); }
	static const char* FindChar(const char* pStr, const char cSearch) { return strchr(pStr, cSearch); }
	static const char* EmptyString() { return ""; }
};

template < >
class StringFuncs<wchar_t>
{
public:
	static wchar_t* Duplicate(const wchar_t* pValue) { return _wcsdup(pValue); }
	static void			  Copy(wchar_t* out_pOut, const wchar_t* pIn, int iLength) { wcsncpy_s(out_pOut, iLength, pIn, iLength); }
	static int			  Compare(const wchar_t* pLhs, const wchar_t* pRhs) { return wcscmp(pLhs, pRhs); }
	static int			  Length(const wchar_t* pValue) { return wcslen(pValue); }
	static const wchar_t* FindChar(const wchar_t* pStr, const wchar_t cSearch) { return wcschr(pStr, cSearch); }
	static const wchar_t* EmptyString() { return L""; }
};

template < typename T = char >
class CUtlConstStringBase
{
public:
	CUtlConstStringBase() : m_pString(NULL) {}
	CUtlConstStringBase(const T* pString) : m_pString(NULL) { Set(pString); }
	CUtlConstStringBase(const CUtlConstStringBase& src) : m_pString(NULL) { Set(src.m_pString); }
	~CUtlConstStringBase() { Set(NULL); }
	void Set(const T* pValue);
	void Clear() { Set(NULL); }
	const T* Get() const { return m_pString ? m_pString : StringFuncs<T>::EmptyString(); }
	operator const T* () const { return m_pString ? m_pString : StringFuncs<T>::EmptyString(); }
	bool IsEmpty() const { return m_pString == NULL; } 
	int Compare(const T* rhs) const;
	bool operator<(const T* rhs) const { return Compare(rhs) < 0; }
	bool operator==(const T* rhs) const { return Compare(rhs) == 0; }
	bool operator!=(const T* rhs) const { return Compare(rhs) != 0; }
	bool operator<(const CUtlConstStringBase& rhs) const { return Compare(rhs.m_pString) < 0; }
	bool operator==(const CUtlConstStringBase& rhs) const { return Compare(rhs.m_pString) == 0; }
	bool operator!=(const CUtlConstStringBase& rhs) const { return Compare(rhs.m_pString) != 0; }
	inline friend bool operator<(const T* lhs, const CUtlConstStringBase& rhs) { return rhs.Compare(lhs) > 0; }
	inline friend bool operator==(const T* lhs, const CUtlConstStringBase& rhs) { return rhs.Compare(lhs) == 0; }
	inline friend bool operator!=(const T* lhs, const CUtlConstStringBase& rhs) { return rhs.Compare(lhs) != 0; }
	CUtlConstStringBase& operator=(const T* src) { Set(src); return *this; }
	CUtlConstStringBase& operator=(const CUtlConstStringBase& src) { Set(src.m_pString); return *this; }
	typedef const T* AltArgumentType_t;
protected:
	const T* m_pString;
};

template < typename T >
void CUtlConstStringBase<T>::Set(const T* pValue)
{
	if (pValue != m_pString)
	{
		free((void*)m_pString);
		m_pString = pValue && pValue[0] ? StringFuncs<T>::Duplicate(pValue) : NULL;
	}
}

template < typename T >
int CUtlConstStringBase<T>::Compare(const T* rhs) const
{
	if (m_pString)
	{
		if (rhs)
			return StringFuncs<T>::Compare(m_pString, rhs);
		else
			return 1;
	}
	else
	{
		if (rhs)
			return -1;
		else
			return 0;
	}
}

typedef	CUtlConstStringBase<char>		CUtlConstString;
typedef	CUtlConstStringBase<wchar_t>	CUtlConstWideString;

struct string_t
{
public:
	bool operator!() const { return (pszValue == NULL); }
	bool operator==(const string_t& rhs) const { return (pszValue == rhs.pszValue); }
	bool operator!=(const string_t& rhs) const { return (pszValue != rhs.pszValue); }
	bool operator<(const string_t& rhs) const { return ((void*)pszValue < (void*)rhs.pszValue); }

	const char* ToCStr() const { return (pszValue) ? pszValue : ""; }

protected:
	const char* pszValue;
};

struct castable_string_t : public string_t 
{
	castable_string_t() { pszValue = NULL; }
	castable_string_t(const char* pszFrom) { pszValue = (pszFrom && *pszFrom) ? pszFrom : 0; }
};

#define NULL_STRING			castable_string_t()