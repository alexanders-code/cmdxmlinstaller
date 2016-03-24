// ************************************************************************ 
//   FileOperations.h - v 0.0.1
// ************************************************************************ 

#pragma once

#ifndef _CMDINST_FILEOPERATIONS_H
#define _CMDINST_FILEOPERATIONS_H

#include <string>
#include <io.h>
#include<sys/stat.h>
#include <Windows.h>

using namespace std;

static const unsigned int sizerwbuf = 262144;
// size buffer 256 kylobytes

class FileOperations
{
public:
	FileOperations(void);
	//FileOperations( const char* path );
	virtual ~FileOperations(void);

	bool PathItsDir();
	bool PathItsFile();
	bool PathItsFileMask();
	bool PathItsFileMask( const char* p );

	void OpenForRead( const char* path );

	int ReadInBuf();

	int ReadInBuf( unsigned int sz );

	char* GetDataBuf();

	void ClearDelBuf();

	void ClearBuf();

	bool CreateDirForFile( const char* path );

	void OpenForWrite( const char* path );

	void OpenExistingFile( const char* path );

	void WriteFromBuf( char* pbuf, unsigned int sz );

	void MoveInFile( long offset = 0 );	
	
	unsigned int GetActualSizeBuf();

	void CloseFile();

	ULARGE_INTEGER FreeSpaceOnDrive( const char* path );

	void DeattachFile();

	void OpenForReadFirstFileMask( const char* path );

	bool OpenForReadNextFileMask();

	bool ExtractFileStub( const char* path );

	void StringToDataBuf( const char* s );

	void ArrayToDataBuf( const char* p, unsigned int sz );

	void CreateBuf( void );
	
	long SizeSingleFile( const char* p );

	unsigned long SizeMaskFile( const char* p );

	bool GetInfoFirstFileMask( const char* path );

	bool GetInfoNextFileMask( void );

	string* GetFullName();

	unsigned long GetSizeFile();

protected:

	string _fullname;

	FILE* _fileworking;

private:

	char* _rwbuf;
	
	unsigned int _sizedatainbuf;	

	intptr_t _hmaskfile;

	_finddata_t _filemaskinfo;

	struct _stat _bufwithinfo;

	void AboutFile( const char* s );	
	
	//void PreparePath();
};

#endif //_CMDINST_FILEOPERATIONS_H