#include "StdAfx.h"

#include "FileOperations.h"
#include "MsgInfErr.h"

#include<sys/stat.h>
#include <string>
#include <io.h>
#include <direct.h>
#include <Windows.h>
#include <string.h>

#include <iostream>
using namespace std;



FileOperations::FileOperations(void)
{
	_rwbuf = 0;
}

//FileOperations::FileOperations( const char* path )
//{	
//	AboutFile( path );
//	if ( !diagmsg.MsgPresent )
//	{
//		_fullname.assign( path );
//		PreparePath();	
//	}
//}

FileOperations::~FileOperations(void)
{
	delete[] _rwbuf;
	_fullname.clear();
}

void FileOperations::AboutFile( const char* s )
{
	if ( -1 == _stat( s, &_bufwithinfo ) )
	{
		if ( errno == ENOENT )
			diagmsg.SetMsg( "No such file or directory", s );
		else
			diagmsg.SetMsg( "Error with file", s );
	}
	else
		_fullname = s;
}

void FileOperations::CreateBuf( void )
{
	if ( !_rwbuf )
	{
		_rwbuf = new char[ sizerwbuf ];
		memset( _rwbuf, '\0', sizerwbuf );
	
		if ( !_rwbuf )
			throw MsgInfErr( "Not enough memory" );		
	}
	_sizedatainbuf  = 0;
}

long FileOperations::SizeSingleFile( const char* p )
{	
	if ( -1 == _stat( p, &_bufwithinfo ) )
	{
		if ( errno == ENOENT )
			diagmsg.SetMsg( "No such file or directory", p );
		else
			diagmsg.SetMsg( "Error with file", p );
		return 0;
	}
	else
		return _bufwithinfo.st_size;
}

unsigned long FileOperations::SizeMaskFile( const char* p )
{
	unsigned long sz = 0;
	_finddata_t filemaskinfo;
	intptr_t hfile;

	if( -1 == (hfile = _findfirst( p, &filemaskinfo ) ) )
	{
		diagmsg.SetMsg( "Error find file ", p );
		return 0;
	}
	do
	{
		if( !( filemaskinfo.attrib & _A_SUBDIR ) )
			sz += filemaskinfo.size;
	}
	while( _findnext( hfile, &filemaskinfo ) == 0 );
	_findclose( hfile );

	return sz;
}


bool FileOperations::GetInfoFirstFileMask( const char* path )
// заполнить структуру _finddata_t и полный путь fullname
// файл не открывается
{
	if ( !PathItsFileMask( path ) )
	{
		_fullname.clear();
		return false;
	}
	else
		_fullname.assign( path );

	_hmaskfile = _findfirst( path, &_filemaskinfo );
	if ( -1 == _hmaskfile )
	{
		diagmsg.SetMsg( "File not found ", path );
		return false;
	}

	while ( _filemaskinfo.attrib & _A_SUBDIR )
	{
		if ( 0 != _findnext( _hmaskfile, &_filemaskinfo ))
		{
			_findclose( _hmaskfile );
			diagmsg.SetMsg( "Directory empty ", path );
			return false;
		}
	}

	char fp[ _MAX_PATH ];
	char dr[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	
	if ( 0 != _splitpath_s( path, dr, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 ) )
	{
		diagmsg.SetMsg( "Error split path", path );
		return false;
	}

	_makepath_s( fp, _MAX_PATH, dr, dir, 0, 0 );

	_fullname.assign( fp ) += _filemaskinfo.name;

	return true;
}


bool FileOperations::GetInfoNextFileMask( void )
// заполнить структуру _finddata_t и полный путь fullname
// файл не открывается
{
	do
	{
		if( 0 != _findnext( _hmaskfile, &_filemaskinfo ) && errno == ENOENT )
		{
			_findclose(  _hmaskfile );
			return false;
		}
	}
	while ( _filemaskinfo.attrib & _A_SUBDIR );
	
	char fp[ _MAX_PATH ];
	char dr[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	
	_splitpath_s( _fullname.c_str(), dr, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 );	

	_makepath_s( fp, _MAX_PATH, dr, dir, 0, 0 );

	_fullname.assign( fp ) += _filemaskinfo.name;	

	return true;
}


string* FileOperations::GetFullName( void )
{
	return &_fullname;
}


unsigned long FileOperations::GetSizeFile( void )
{
	return _filemaskinfo.size;
}


void FileOperations::ClearDelBuf()
{
	delete[] _rwbuf;
	_rwbuf = 0;
	_sizedatainbuf = 0;
}

void FileOperations::ClearBuf()
{
	_sizedatainbuf = 0;
}


bool FileOperations::PathItsDir()
{	
	if ( PathItsFileMask() )
		return false;
	return _bufwithinfo.st_mode & _S_IFDIR;
}

bool FileOperations::PathItsFile()
{	
	if ( PathItsFileMask() )
		return false;
	return _bufwithinfo.st_mode & _S_IFREG;
}

bool FileOperations::PathItsFileMask()
{
	char fn[ _MAX_FNAME ];
	char ext[ _MAX_EXT ];

	_splitpath_s( _fullname.c_str(), 0, 0, 0, 0, fn, _MAX_FNAME, ext, _MAX_EXT  );

	return strchr( fn, '*' ) || strchr( fn, '?' ) || strchr( ext, '*' ) || strchr( ext, '?' );
	return true;
}


bool FileOperations::PathItsFileMask( const char* p )
{
	char fn[ _MAX_FNAME ];
	char ext[ _MAX_EXT ];

	_splitpath_s( p, 0, 0, 0, 0, fn, _MAX_FNAME, ext, _MAX_EXT  );

	return strchr( fn, '*' ) || strchr( fn, '?' ) || strchr( ext, '*' ) || strchr( ext, '?' );
	return true;
}

void FileOperations::OpenForRead( const char* path )
// Открывает файл для чтения, создает буфер
// 
{
	AboutFile( path );

	if ( diagmsg.MsgPresent )
		return;

	if ( 0 != fopen_s( &_fileworking, path, "rb" ) )
	{		
		diagmsg.SetMsg( "File not opened ", path );
		return;
	}	

	if ( !_rwbuf )
		CreateBuf();
	_fullname.assign( path );
}

int FileOperations::ReadInBuf()
// считывает из открытого файла в буфер 
{	
	return ReadInBuf( sizerwbuf );
}


int FileOperations::ReadInBuf( unsigned int sz )
// считывает из открытого файла в буфер блок размера sz 
// размер блока не более sizerwbuf
{
	if ( sz > sizerwbuf )
		sz = sizerwbuf;

	if ( feof ( _fileworking ) )
	{
		fclose( _fileworking );
		_sizedatainbuf = 0;
		return _sizedatainbuf;
	}	
	
	_sizedatainbuf = fread_s( _rwbuf, sz, 1, sz, _fileworking );
	
	if ( ferror( _fileworking ) )
	{
		diagmsg.SetMsg( "Error reading file", _fullname.c_str() );
		return 0;
	}

	return _sizedatainbuf;	
}


char* FileOperations::GetDataBuf()
{
	if ( _rwbuf )
		return _rwbuf;
	else
		return 0;
}

unsigned int FileOperations::GetActualSizeBuf()
{
	return _sizedatainbuf;
}

bool FileOperations::CreateDirForFile( const char* path )
// создает на диске заданный каталог
{
	char fp[ _MAX_PATH ];
	char dr[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];

	if ( 0 == path )
		return true;	
	
	if ( 0 != _splitpath_s( path, dr, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 ) )
	{
		diagmsg.SetMsg( "Error in path", path );
		return false;
	}

	_makepath_s( fp, _MAX_PATH, dr, dir, 0, 0 );
	
	if( strlen( fp ) )
	{
		if ( -1 == _mkdir( fp ) && EEXIST != errno )
		{			
			diagmsg.SetMsg( "Not create directory", path );
			return false;
		}
	}
	return true;
}

void FileOperations::CloseFile()
{
	fclose( _fileworking );
}

void FileOperations::OpenForWrite( const char* path )
{
	if ( !CreateDirForFile( path ) )
		return;

	if ( fopen_s( &_fileworking, path, "wb" ) )
	{	

		std::cout << errno << endl;
		int eee = errno;
		diagmsg.SetMsg( "File not create ", path );
		return;
	}	
}


void FileOperations::OpenExistingFile( const char* path )
//открывает существующий файл для чтения и записи
//использовать для добавления данных в stub selfextractor 
{
	if ( fopen_s( &_fileworking, path, "r+b" ) )
	{	

		std::cout << errno << endl;
		int eee = errno;
		diagmsg.SetMsg( "File not create ", path );
		return;
	}	
}

void FileOperations::WriteFromBuf( char* pbuf, unsigned int sz )
{
	if ( !pbuf )
	{		
		fclose( _fileworking );
		return;
	}
	
	if ( sz != fwrite( pbuf, 1, sz, _fileworking ) )
		diagmsg.SetMsg( "Error wtite in file:", _fullname.c_str() );	
}

void FileOperations::MoveInFile( long offset )
{
	if ( -1 == fseek( _fileworking, offset, SEEK_SET ) )
		diagmsg.SetMsg( "Error move in file", _fullname.c_str() );	
}


ULARGE_INTEGER FileOperations::FreeSpaceOnDrive( const char* path)
{
	ULARGE_INTEGER freeavail;
	GetDiskFreeSpaceExA( path, &freeavail, 0, 0 ); 
	return freeavail;
}


void FileOperations::DeattachFile()
{	
	if( _fileworking )
		fclose( _fileworking );	
	_fileworking = 0;
	_fullname.clear();
	_sizedatainbuf = 0;	
}


void FileOperations::OpenForReadFirstFileMask( const char* path )
{
	_finddata_t filemaskinfo;

	_fullname.assign( path );
	
	if ( !PathItsFileMask() )
	{
		_fullname.clear();
		return;
	}

	_hmaskfile = _findfirst( path, &filemaskinfo );
	if ( -1 == _hmaskfile )
	{
		diagmsg.SetMsg( "File not found ", path );
		return;
	}

	while ( filemaskinfo.attrib & _A_SUBDIR )
	{
		if ( 0 != _findnext( _hmaskfile, &filemaskinfo ))
		{
			_findclose( _hmaskfile );
			diagmsg.SetMsg( "Error in file", path );
			return;
		}
	}

	char fp[ _MAX_PATH ];
	char dr[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	
	if ( 0 != _splitpath_s( path, dr, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 ) )
	{
		diagmsg.SetMsg( "Error split path", path );
		return;
	}

	_makepath_s( fp, _MAX_PATH, dr, dir, 0, 0 );

	_fullname.assign( fp ) += filemaskinfo.name;

	OpenForRead( _fullname.c_str() );

	return;
}


bool FileOperations::OpenForReadNextFileMask()
{
	_finddata_t filemaskinfo;	
	
	do
	{
		if( 0 != _findnext( _hmaskfile, &filemaskinfo ) && errno == ENOENT )
		{
			_findclose(  _hmaskfile );
			return false;
		}
	}
	while ( filemaskinfo.attrib & _A_SUBDIR );
	
	char fp[ _MAX_PATH ];
	char dr[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];
	
	_splitpath_s( _fullname.c_str(), dr, _MAX_DRIVE, dir, _MAX_DIR, 0, 0, 0, 0 );	

	_makepath_s( fp, _MAX_PATH, dr, dir, 0, 0 );

	_fullname.assign( fp ) += filemaskinfo.name;

	OpenForRead( _fullname.c_str() );

	return true;
}

bool FileOperations::ExtractFileStub( const char* path )
// извлекает из ресурсов файл заготовку инсталлятора
{
	char* reourceid = "IDU_SAMPLEEXE";
	
	HRSRC hres = FindResourceA( 0, reourceid, "BINARY" );
	
	unsigned int sizestub = SizeofResource(0, hres);
	
	HGLOBAL hgl = LoadResource( 0, hres );
	
	void* lplock = LockResource(hgl);	
	char* pc = (char* )lplock;
    
	if ( !_rwbuf )
		CreateBuf();

	OpenForWrite( path );
	while ( sizestub > 0 )
	{
		if ( sizestub <= sizerwbuf )
		{
			memcpy( _rwbuf, pc, sizestub );
			_sizedatainbuf = sizestub;
			WriteFromBuf( _rwbuf, sizestub );
			break;
		}
		else
		{
			memcpy( _rwbuf, pc, sizerwbuf );
			_sizedatainbuf = sizerwbuf;
			WriteFromBuf( _rwbuf, sizerwbuf );
			pc += sizerwbuf;
			sizestub = sizestub - sizerwbuf;
		}
	}

	_sizedatainbuf = 0;

	return true;
}


void FileOperations::StringToDataBuf( const char* s )
//строку в буфер данных, для последующей записи в файл
{
	if ( _rwbuf )
	{
		if ( _sizedatainbuf + strlen( s ) + 1 > sizerwbuf )
			return;
		strcpy_s( _rwbuf + _sizedatainbuf , strlen( s ) + 1, s );
		_sizedatainbuf += strlen( s );
	}
}

void FileOperations::ArrayToDataBuf( const char* p, unsigned int sz )
// массив заданной длины в буфер данных, для последующей записи в файл
{
	if ( _rwbuf )
	{
		if ( _sizedatainbuf + sz  > sizerwbuf )
			return;
		memcpy( _rwbuf + _sizedatainbuf, p, sz );		
		_sizedatainbuf += sz;
	}
}