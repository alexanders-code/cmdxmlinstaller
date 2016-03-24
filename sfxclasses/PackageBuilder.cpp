// ************************************************************************ 
//   PackageBuilder.cpp - v 0.0.1
// ************************************************************************ 
#include "StdAfx.h"

#include <Windows.h>
#include <string>
#include <vector>
#include <string.h>

#include "PackageBuilder.h"
#include "FileOperations.h"
#include "ResourceChange.h"
#include "MsgInfErr.h"


PackageBuilder::PackageBuilder( DocInstaller* di )
{
	_installerfile = di;
	ClearCurrDataBlock();
}


PackageBuilder::~PackageBuilder(void)
{
}


bool PackageBuilder::CalculateAllSize()
// true если свободного места на диске достаточно 
{
	std::string currname;
	std::string currval;

	ULARGE_INTEGER sz;
	sz.QuadPart = 0;
	char* reourceid = "IDU_SAMPLEEXE";
	
	sz = CalculateAllFileSize();

	HRSRC hres = FindResourceA( 0, reourceid, "BINARY" );	
	sz.QuadPart += SizeofResource(0, hres);
	
	//При обновлении ресурсов размер файла почему то увеличивается на 1024 байт
	sz.QuadPart += 1024;
	//

	FileOperations fo;
	ULARGE_INTEGER szd = fo.FreeSpaceOnDrive( 0 );

	if( sz.QuadPart < szd.QuadPart )
		return true;
	else
		return false;
}


ULARGE_INTEGER PackageBuilder::CalculateAllFileSize( void )
{
	ULARGE_INTEGER sz;
	sz.QuadPart = 0;

	for( size_t i = 0; i < _datablockarray.size(); i ++ )
		sz.QuadPart += _datablockarray.at( i ).FileSize;
	
	return sz;
}


bool PackageBuilder::BuildSelfInstaller( void )
{
	if( !CalculateAllSize() )
	{
		diagmsg.SetMsg( "There is not enough space on the disk", 0 );
		return false;
	}

	GetNameInstaller();

	ExtractStub();

	SetInfoResource();

	FileOperations fo;
	fo.OpenExistingFile( _fullnameinstaller.c_str() );
	if( diagmsg.MsgPresent )
		return false;

	fo.CreateBuf();
	WriteDataInStub( &fo );
	
	if( !AddAllFileToStub( &fo ) )
	{
		fo.ClearDelBuf();
		fo.CloseFile();
		return false;
	}
	
	fo.ClearDelBuf();
	fo.CloseFile();

	return true;
}


void PackageBuilder::FillDataBlockArr( void )
{
	std::string currname;
	std::string currval;
	std::string nameval;	
	std::string extractval;
	std::string defaultextractpath;

	FileOperations fo;

	_datablockarray.clear();

	_installerfile->Movecurrparent( TOKINSTALLER );

	if( !_installerfile->GoFirstNode( "installer" ) )
		return;

	if( !_installerfile->GetChildNameValue( currname, currval ) )
		return;

	do
	{
		if( currname == "extract" )
		{
			defaultextractpath.assign( currval );
			break;
		}
	}
	while( _installerfile ->GetChildNameValue( currname, currval, false ) );
		
	_installerfile->Movecurrparent( TOKFILE );

	if( !_installerfile->GoFirstNode( "file" ) )
		return;
		
	do
	{
		nameval.clear();		
		extractval.clear();		

		if( !_installerfile->GetChildNameValue( currname, currval ) )
			return ;
		do
		{
			if( currname == "name" && !currval.empty() )
				nameval.assign( currval );
			if( currname == "extract" && !currval.empty() )
				extractval.assign( currval );			
		}
		while (	_installerfile ->GetChildNameValue( currname, currval, false ) );
		
		if( extractval.empty() )
			extractval.assign( defaultextractpath );
		
		if( fo.PathItsFileMask( nameval.c_str() ) )
		{
			fo.GetInfoFirstFileMask( nameval.c_str() );
			do
			{				
				strcpy_s( _currblock.NameFile, _MAX_PATH, fo.GetFullName()->c_str() );
				strcpy_s( _currblock.ExtractTo, _MAX_PATH, extractval.c_str() );
				_currblock.FileSize = fo.GetSizeFile();
				_datablockarray.push_back( _currblock );
			}
			while ( fo.GetInfoNextFileMask() );

		}
		else
		{
			
			
			strcpy_s( _currblock.NameFile, _MAX_PATH, nameval.c_str() );
			strcpy_s( _currblock.ExtractTo, _MAX_PATH, extractval.c_str() );
			_currblock.FileSize = fo.SizeSingleFile( nameval.c_str() );
			_datablockarray.push_back( _currblock );			
		}
	}
	while ( _installerfile->GoNextNode( "file" ) );

	ClearCurrDataBlock();

	return;		
}


void PackageBuilder::ClearCurrDataBlock( void )
{
	memset( _currblock.NameFile, 0, _MAX_PATH );
	memset( _currblock.ExtractTo, 0, _MAX_PATH );
	_currblock.FileSize = 0;
}


void PackageBuilder::FillHeaderSelf( void )
// заполнить структуру HeaderSelf для запписи в заголовок инсталлятора
{
	struct _stat stbuf;
	_stat( _fullnameinstaller.c_str(), &stbuf );
	_hs.OffsetStartData = stbuf.st_size;

	_hs.SizeAllDataBlock = _datablockarray.size() * sizeof( DataBlock );

}


void PackageBuilder::GetNameInstaller( void )
{
	std::string currname;
	std::string currval;

	_installerfile->Movecurrparent( TOKINSTALLER );

	if( !_installerfile->GoFirstNode( "installer" ) )
		return;

	if( !_installerfile->GetChildNameValue( currname, currval ) )
		return;

	do
	{
		if( currname == "name" )
		{
			_fullnameinstaller.assign( currval );
			break;
		}
	}
	while( _installerfile ->GetChildNameValue( currname, currval, false ) );

	return;
}

void PackageBuilder::ExtractStub( void )
//извлекает из ресурсов файл заглушку
{
	FileOperations fo;

	fo.ExtractFileStub( _fullnameinstaller.c_str() );
	fo.CloseFile();
}


bool PackageBuilder::SetInfoResource( void )
{
	std::string currname;
	std::string currval;
	ResourceChange rc( _fullnameinstaller.c_str() );

	if( diagmsg.MsgPresent )
	{
		diagmsg.PrintMsg();
		return false;
	}

	_installerfile->Movecurrparent( TOKPRODUCT );

	if( _installerfile->GoFirstNode( "product" ) )	
	{
		if( !_installerfile->GetChildNameValue( currname, currval ) )
			return true;
		do
		{
			if( !currname.empty() && !currval.empty() )
				if( !rc.AddResToChange( currname.c_str(), currval.c_str() ) )
					return false;		
		}
		while( _installerfile ->GetChildNameValue( currname, currval, false ) );

		if ( !rc.UpdateResourceInFile() )
		{
			diagmsg.PrintMsg();
			return false;
		}
	}

	return true;
}


void PackageBuilder::WriteDataInStub( FileOperations* f )
// записать структуру _hs в заголовок, смещение 28 dex 1C hex
// _datablockarray в конец файла
{	
	FillDataBlockArr();
	FillHeaderSelf();	

	vector< DataBlock > tmpblock = _datablockarray;

	f->ArrayToDataBuf( (char *)&_hs, sizeof( _hs ) );
	f->MoveInFile( 28 );
	f->WriteFromBuf( f->GetDataBuf(), sizeof( _hs ) );
	f -> ClearBuf();

	char fp[ _MAX_PATH ];	
	char fn[ _MAX_FNAME ];
	char fext[ _MAX_EXT ];
	
	for( int i = 0; i < tmpblock.size(); i ++ )
	{
		_splitpath_s( tmpblock.at( i ).NameFile, 0, 0, 0, 0, fn, _MAX_FNAME, fext, _MAX_EXT );
		_makepath_s( fp, _MAX_PATH, 0, 0, fn, fext );
		strcpy_s( tmpblock.at( i ).NameFile, _MAX_PATH, fp );
	}

	f->MoveInFile( _hs.OffsetStartData );
	
	for( int i = 0; i < tmpblock.size(); i ++ )
	{
		DataBlock* t = &tmpblock.at( i );		
		f -> ArrayToDataBuf( (char *)t, sizeof( DataBlock ) );
		if( ( f->GetActualSizeBuf() + sizeof( DataBlock ) ) > sizerwbuf )
		{
			f->WriteFromBuf( f->GetDataBuf(), f->GetActualSizeBuf() );
			f->ClearBuf();
		}		
	}

	if( f -> GetActualSizeBuf() )
	{
		f->WriteFromBuf( f->GetDataBuf(), f->GetActualSizeBuf() );
		f->ClearBuf();
	}

	if( diagmsg.MsgPresent )
	{
		diagmsg.PrintMsg();
		return;
	}
	
	return;
}


bool PackageBuilder::AddAllFileToStub(  FileOperations* f )
// добавить последовательно все файлы из _datablockarray к exe
{
	FileOperations file;
	
	f->MoveInFile( _hs.OffsetStartData + _hs.SizeAllDataBlock );

	for( int i = 0; i < _datablockarray.size(); i ++ )
	{
		file.OpenForRead( _datablockarray.at( i ).NameFile );
		if( diagmsg.MsgPresent )
			{
				diagmsg.PrintMsg();
				return false;
			}

		while(	file.ReadInBuf() )
		{
			f ->WriteFromBuf( file.GetDataBuf(), file.GetActualSizeBuf() );
			if( diagmsg.MsgPresent )
			{
				diagmsg.PrintMsg();
				return false;
			}
		}

		file.ClearDelBuf();
		file.DeattachFile();
	}

	return true;
}