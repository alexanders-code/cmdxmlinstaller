// ************************************************************************ 
//   exestub.cpp - v 0.0.1
// ************************************************************************ 

#include "stdafx.h"

#include <Windows.h>
#include <vector>
#include <memory.h>
#include <string>
#include <iostream>


#include "..\sfxclasses\DataStruct.h"
#include "..\sfxclasses\MsgInfErr.h"
#include "..\sfxclasses\FileOperations.h"
#include "..\sfxclasses\FileOpsSelf.h"

std::vector< DataBlock > datablockarray;
HeaderSelf hs;

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
	// суммирование размеров файлов для каждой букры диска, куда планируется их извлечь
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
		std::cout << AllDrives.at( i ).Dr << " " << AllDrives.at( i ).RequiredSize << endl;

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

void FillDataBlockArrayFromSelf( FileOpsSelf* fself )
// Заполнить vector datablockarray данными
{	
	DataBlock ds;
	fself->MoveInFile( 28 );
	fself->ReadInBuf( sizeof( hs ) );
	memcpy_s( (char *)&hs, sizeof( hs ), fself->GetDataBuf(), sizeof( hs ) );
	fself->ClearBuf();

	DWORD datablocknumbers = hs.SizeAllDataBlock / sizeof( DataBlock );
	//сколько блоков DataBlock гарантированно поместится в буфер размером sizerwbuf
	DWORD fitinsidebuf = sizerwbuf / sizeof( DataBlock ) - 1;

	fself->MoveInFile( hs.OffsetStartData );

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

bool ExtractFilesFromSelf( FileOpsSelf* fself )
{
	FileOperations fo;
	std::string str;
	
	fself -> MoveInFile( hs.OffsetStartData + hs.SizeAllDataBlock );
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
			fself -> ClearBuf();
		}		
		fo.DeattachFile();		
		std::cout << "install " << datablockarray.at( i ).ExtractTo << datablockarray.at( i ).NameFile << " successfully." << endl;
	}

	return true;
}


int _tmain(int argc, _TCHAR* argv[])
{
	FileOpsSelf fo;

	FillDataBlockArrayFromSelf( &fo );
	
	if( CalculateAllSize() )
	{
		ExtractFilesFromSelf( &fo );		
		fo.ClearDelBuf();
		fo.DeattachFile();
	}	

	return 0;
}

