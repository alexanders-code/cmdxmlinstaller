// ************************************************************************ 
//   FileOperations.cpp - v 0.0.1
// ************************************************************************ 
#include "StdAfx.h"

#include <Windows.h>

#include "FileOpsSelf.h"

FileOpsSelf::FileOpsSelf(void)
{	
	char c[ MAX_PATH ];
	memset( c, 0, MAX_PATH );
	GetModuleFileNameA(GetModuleHandle(NULL), c, MAX_PATH);	
	_fullname.assign( c );
	OpenForRead( _fullname.c_str() );
}


FileOpsSelf::~FileOpsSelf(void)
{
	if( _fileworking )
		fclose( _fileworking );	
}
