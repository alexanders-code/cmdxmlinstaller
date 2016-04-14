// ************************************************************************ 
//   exestub.cpp - v 0.3
//	 exestub.exe - v 0.0.17.0alpha
// ************************************************************************ 

#include "stdafx.h"

#include <Windows.h>
#include <vector>
#include <memory.h>
#include <string>
#include <iostream>
#include <time.h>

#include "exestub.h"
#include "Interpreter.h"

#include "..\sfxclasses\DataStruct.h"
#include "..\sfxclasses\MsgInfErr.h"
#include "..\sfxclasses\FileOperations.h"
#include "..\sfxclasses\FileOpsSelf.h"

std::vector< DataBlock > datablockarray;
HeaderSelf hs;
std::vector< CommandBlock > commandblockarray;

WORD glcolor;
static int glpercent = 0;
bool firstrun = true;

void CallbackPrintPercent( unsigned int sz, unsigned int r, WORD color );
void CallbackPrintProgress( unsigned int sz, unsigned int r, WORD color );
//pointer to callback function 
typedef void ( *CALLBACKPRINT )( unsigned int, unsigned int, WORD );

void TimetToFileTime( time_t t, LPFILETIME pft )
//function from MSDN
{
    LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
    pft->dwLowDateTime = (DWORD) ll;
    pft->dwHighDateTime = ll >> 32;
}


bool CalculateAllSize( void )
//перебор всех букв дисков, указанныых в путях datablockarray на которые планируется извлекать файлы
//подсчет требуемого места и сравнение со сводобным местом на заданных дисках
// true если места достаточно
{
	struct SizeOnDrive
	{
		char Dr[ _MAX_DRIVE ];
		unsigned long RequiredSize;
	};

	std::vector< SizeOnDrive > AllDrives;
	SizeOnDrive tmp;

	char pathinst[ _MAX_PATH ];	
	char pathdrive [ _MAX_DRIVE ];
	
	char currdrive[ _MAX_DRIVE ];
	unsigned long currsize;

	GetModuleFileNameA(GetModuleHandle(NULL), pathinst, MAX_PATH);
	
	_splitpath_s( pathinst, pathdrive, _MAX_DRIVE, 0, 0, 0, 0, 0, 0 );
	
	strcpy_s( tmp.Dr, _MAX_DRIVE, pathdrive );
	tmp.RequiredSize = 0;	

	for( int i = 0; i < datablockarray.size(); i ++ )
	// суммирование размеров файлов для каждой буквы диска, куда планируется их извлечь
	{
		if( !strlen( datablockarray.at( i ).ExtractTo ) )
			strcpy_s( currdrive, _MAX_DRIVE, pathdrive );
		else
		{
			_splitpath_s( datablockarray.at( i ).ExtractTo , currdrive, _MAX_DRIVE, 0, 0, 0, 0, 0, 0 );
			if( !strlen( currdrive ) )
				strcpy_s( currdrive, _MAX_DRIVE, pathdrive );
		}
		
		currsize = datablockarray.at( i ).FileSize;

		int j;
		for( j = 0; j < AllDrives.size(); j ++ )
		{
			if( !_stricmp( AllDrives.at( j ).Dr, currdrive ) )
			{
				AllDrives.at( j ).RequiredSize += currsize;
				break;
			}
		}

		if( j == AllDrives.size() )
		{
			strcpy_s( tmp.Dr, _MAX_DRIVE, currdrive );
			tmp.RequiredSize = currsize;
			AllDrives.push_back( tmp );
		}
	}

	for( int i = 0; i < AllDrives.size(); i ++ )
	{		

		ULARGE_INTEGER freespace;
		if( !GetDiskFreeSpaceExA( AllDrives.at( i ).Dr, &freespace, 0, 0 ) )
		{			
			diagmsg.SetMsg( "It is impossible to determine the amount of free disk space on ", AllDrives.at( i ).Dr );
			return false;
		}

		if( freespace.QuadPart < AllDrives.at( i ).RequiredSize )
		{
			diagmsg.SetMsg( "There is not enough space on ", AllDrives.at( i ).Dr );			
			return false;
		}
	}
	
	return true;
}


#if _DEBUG
void FillHeaderSelf( FileOperations* fself )
#else
void FillHeaderSelf( FileOpsSelf* fself )
#endif
{
	fself->MoveInFile( 28 );
	fself->ReadInBuf( sizeof( hs ) );
	memcpy_s( (char *)&hs, sizeof( hs ), fself->GetDataBuf(), sizeof( hs ) );
	fself->ClearBuf();
}


#if _DEBUG
void FillDataBlockArrayFromSelf( FileOperations* fself )
#else
void FillDataBlockArrayFromSelf( FileOpsSelf* fself )
#endif
// Заполнить vector datablockarray данными
{	
	DataBlock ds;

	DWORD datablocknumbers = hs.SizeAllDataBlock / sizeof( DataBlock );
	//сколько блоков DataBlock гарантированно поместится в буфер размером sizerwbuf
	DWORD fitinsidebuf = sizerwbuf / sizeof( DataBlock ) - 1;

	fself->MoveInFile( hs.SizeAllCommandBlock + hs.OffsetStartData );

	DWORD blockread = 0;
	while( blockread < datablocknumbers )
	{
		if( blockread + fitinsidebuf > datablocknumbers )
			fitinsidebuf = datablocknumbers - blockread;

		fself->ReadInBuf( fitinsidebuf * sizeof( DataBlock ) );
		
		char *p = fself->GetDataBuf();
		for( int i = 0; i < fitinsidebuf; i ++ )
		{
			memcpy_s( (char *)&ds, sizeof( ds ), p, sizeof( ds ) );
			datablockarray.push_back( ds );
			p += sizeof( ds );
		}

		fself -> ClearBuf();
		blockread += fitinsidebuf;
	}

	return;
}


//заполнить vector commandblockarray данными
#if _DEBUG
void FillCommandBlockArrayFromSelf( FileOperations* fself )
#else
void FillCommandBlockArrayFromSelf( FileOpsSelf* fself )
#endif
{
	CommandBlock cb;

	if( 0 == hs.SizeAllCommandBlock )
		return;

	DWORD commandblocknumbers = hs.SizeAllCommandBlock / sizeof (CommandBlock );
	DWORD fitinsidebuf = sizerwbuf / sizeof( CommandBlock ) - 1;

	fself -> MoveInFile( hs.OffsetStartData );

	DWORD blockread = 0;
	while( blockread < commandblocknumbers )
	{
		if( blockread + fitinsidebuf > commandblocknumbers )
			fitinsidebuf = commandblocknumbers - blockread;

		fself->ReadInBuf( fitinsidebuf * sizeof( CommandBlock ) );
		
		char *p = fself->GetDataBuf();
		for( int i = 0; i < fitinsidebuf; i ++ )
		{
			memcpy_s( (char *)&cb, sizeof( cb ), p, sizeof( cb ) );
			commandblockarray.push_back( cb );
			p += sizeof( cb );
		}

		fself -> ClearBuf();
		blockread += fitinsidebuf;
	}

	return;
}//void FillCommandBlockArrayFromSelf( FileOpsSelf* fself )


void ChangeAttrAndModifyDate( const std::string& fn, const DWORD attr, const time_t modtime )
{
	FILETIME ft;
	
	TimetToFileTime( modtime, &ft );
	
	wchar_t ws[ _MAX_PATH ];
	MultiByteToWideChar( 0, 0, fn.c_str(), -1, ws, _MAX_PATH );
	
	HANDLE hf = CreateFile( ws, GENERIC_WRITE, FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

	if( INVALID_HANDLE_VALUE == hf )
	{
		int r =GetLastError();
		diagmsg.SetMsg( "Error with set dtate modify ", fn.c_str() );
		return;
	}

	SetFileTime( hf, 0, 0, &ft );
	SetFileAttributes( ws, attr );
	if( !CloseHandle( hf ) )	
		diagmsg.SetMsg( "Error close file ", fn.c_str() );

	return;
}


#if _DEBUG
bool ExtractFilesFromSelf( FileOperations* fself, CALLBACKPRINT cbp )
#else
bool ExtractFilesFromSelf( FileOpsSelf* fself, CALLBACKPRINT cbp )
#endif
{
	FileOperations fo;
	std::string str;
	
	fself -> MoveInFile( hs.OffsetStartData + hs.SizeAllCommandBlock + hs.SizeAllDataBlock );
	for( int i = 0; i < datablockarray.size(); i ++ )
	{
		str.assign( datablockarray.at( i ).ExtractTo );
		str.append( datablockarray.at( i ).NameFile );
		fo.OpenForWrite( str.c_str() );
		if( diagmsg.MsgPresent )
		{
			diagmsg.PrintMsg();
			fo.DeattachFile();
			return false;
		}		

		DWORD bytesread = 0;
		unsigned int szbuf = sizerwbuf;
		
		std::cout << "install " << datablockarray.at( i ).ExtractTo << datablockarray.at( i ).NameFile << " ";

		while( bytesread < datablockarray.at( i ).FileSize )
		{
			if( bytesread + szbuf > datablockarray.at( i ).FileSize )
				szbuf = datablockarray.at( i ).FileSize - bytesread;
			
			fself -> ReadInBuf( szbuf );
			if( diagmsg.MsgPresent )
			{
				diagmsg.PrintMsg();
				return false;
			}			
			
			fo.WriteFromBuf( fself -> GetDataBuf(), szbuf );
			if( diagmsg.MsgPresent )
			{
				diagmsg.PrintMsg();
				fo.DeattachFile();
				return false;
			}
			bytesread += szbuf;
			
			// call callback function on pointer
			if( cbp )
				cbp( datablockarray.at( i ).FileSize, bytesread, glcolor );
			
			fself -> ClearBuf();
		}		

		fo.DeattachFile();
		//set original attributes and datetime write		
		ChangeAttrAndModifyDate( str, datablockarray.at( i ).Attributes, datablockarray.at( i ).ModifyTime );
		if( diagmsg.MsgPresent)
		{
			diagmsg.PrintMsg();
			return false;
		}
		else
			std::cout << "successfully." << endl;
	}

	return true;
}


//прогресс извлечения/записи на диск файла в процентах
// sz размер файла, r - считано всего байт
void CallbackPrintPercent( unsigned int sz, unsigned int r, WORD color )
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD coord;
	
	unsigned int percent;
	char strpercent[ 5 ] = { 0, 0, 0, 0, 0 };	

	if( 0 == sz || r > sz )
		return;
	
	HANDLE conshandle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo( conshandle, &csbi );

	WORD savetextcolor = 0;
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_GREEN );
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_BLUE );
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_RED );
	coord.X = csbi.dwCursorPosition.X;
    coord.Y = csbi.dwCursorPosition.Y;

	percent =  (float)r / (float)sz * 100;
	
	_itoa( percent, strpercent, 10 );
	strpercent[ strlen( strpercent ) ] = '%';	
	
	if( color )
		SetConsoleTextAttribute( conshandle, color );
	else
		SetConsoleTextAttribute( conshandle, savetextcolor );

	std::cout << strpercent;

	SetConsoleTextAttribute( conshandle, savetextcolor );
	
	if( 100 > percent )	
		SetConsoleCursorPosition( conshandle, coord );	
	else
		std::cout << " ";

	return;
}

//прогресс извлечения/записи на диск файла в виде полоски
// sz размер файла, r - считано всего байт
void CallbackPrintProgress( unsigned int sz, unsigned int r, WORD color )
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD coord;
	int percentoneblock = 2;
	unsigned int percent;
	WORD savetextcolor = 0;

	if( 0 == sz ) 
		return;

	HANDLE conshandle = GetStdHandle(STD_OUTPUT_HANDLE);	
	GetConsoleScreenBufferInfo( conshandle, &csbi );
	coord.X = csbi.dwCursorPosition.X;
	coord.Y = csbi.dwCursorPosition.Y;		
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_GREEN );
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_BLUE );
	savetextcolor = savetextcolor | ( csbi.wAttributes & FOREGROUND_RED );	

	if( firstrun )
	{
		//при первом запуске проверка на возможную ошибку
		if( r > sz )
			return;		

		std::cout << '\n';	
		GetConsoleScreenBufferInfo( conshandle, &csbi );
		coord.X = csbi.dwCursorPosition.X;
		coord.Y = csbi.dwCursorPosition.Y;		

		for( int i = 0; i < 100 / percentoneblock; i++ )
			WriteConsole( conshandle, L"█", 1, 0, 0 );

		SetConsoleCursorPosition( conshandle, coord );

		firstrun = false;
	}
	
	percent =  (float)r / (float)sz * 100;
	if( percentoneblock == ( percent - glpercent ) )
	{
		
		SetConsoleTextAttribute( conshandle, color );		
			WriteConsole( conshandle, L"█", 1, 0, 0 );
		SetConsoleTextAttribute( conshandle, savetextcolor );
		glpercent = percent;
		
	}

	if( 100 <= percent )
	{
		glpercent = 0;		
		std::cout << '\n';
	}
}


int _tmain(int argc, _TCHAR* argv[])
{	
	setlocale( LC_ALL, ".ACP" );	
#if _DEBUG
	FileOperations fo;
#else
	FileOpsSelf fo;
#endif	
	FillHeaderSelf( &fo );
	FillCommandBlockArrayFromSelf( &fo );

	Interpreter interpr( &commandblockarray );
	glcolor = interpr.GetColor();
	
	CALLBACKPRINT pf = 0;
	if( interpr.GetProgress() )
	{
		if( 0 == strcmp( interpr.GetProgress(), "percent" ) )
			pf = &CallbackPrintPercent;	
		if( 0 == strcmp( interpr.GetProgress(), "bar" ) )
			pf = &CallbackPrintProgress;
	}

	FillDataBlockArrayFromSelf( &fo );

	if( CalculateAllSize() )
	{
		ExtractFilesFromSelf( &fo, pf );		
		fo.ClearDelBuf();
		fo.DeattachFile();
	}	
	return 0;
}

