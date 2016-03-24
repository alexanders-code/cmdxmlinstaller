// ************************************************************************ 
//   ResourceChange.h - v 0.0.1
// ************************************************************************ 

#pragma once

#ifndef _CMDINST_RESOURCECHANGE_H
#define _CMDINST_RESOURCECHANGE_H

#include <stdlib.h>
#include <string>

static const int MaxCountRes = 3;
static const int MaxLenStrValue = 50;
static const int MaxLenStrVersion = 15;

class ResourceChange
{
public:
	ResourceChange(void);

	ResourceChange( const char* fname );

	bool AddResToChange( const char* rn, const char* rv );

	bool UpdateResourceInFile();

	void ClearResToUpdate();

	virtual ~ResourceChange(void);
	
	
protected:

	std::wstring ResName[ MaxCountRes ];
	std::wstring ValName[ MaxCountRes ];

	int CurrIndex;

private:

	wchar_t _fname[ _MAX_PATH ];

	unsigned int _sizeresource;

	bool ParseVersion( const char* version, WORD *mshi, WORD* mslo, WORD* lshi, WORD *lslo );

	bool ParseVersion( const wchar_t* version, WORD *mshi, WORD* mslo, WORD* lshi, WORD *lslo );

	bool ParseVersion( const char* version );

	bool AddNameValToArray( const char* rn, const char* rv );

	
};

#endif //_CMDINST_RESOURCECHANGE_H