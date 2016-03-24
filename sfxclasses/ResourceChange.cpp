#include "StdAfx.h"

#include <string.h>
#include <Windows.h>
#include <cvt/wstring>
#include <codecvt>‎


#include "ResourceChange.h"
#include "MsgInfErr.h"

#pragma comment(lib,"Version.lib")


ResourceChange::ResourceChange(void)
{
	CurrIndex = 0;
}

ResourceChange::ResourceChange( const char* fname )
{
	CurrIndex = 0;

	if( strlen( fname ) + 1 > _MAX_PATH )
		diagmsg.SetMsg( "fname too length", fname );
	else
	{
		size_t cc;
		mbstowcs_s( &cc, _fname, _MAX_PATH, fname, strlen(fname)+1 );		
		_sizeresource = GetFileVersionInfoSize( _fname , 0 );
		
		if( !_sizeresource )
			diagmsg.SetMsg( "Error find resource in ", fname ) ;
	}
}


bool ResourceChange::AddResToChange( const char* rn, const char* rv )
// добавить в массив имен ресурсов и значений
{
	if ( 0 == strcmp( "name", rn ) )
	{
		if ( AddNameValToArray( "ProductName", rv ) )
			return true;

	}

	if ( 0 == strcmp( "ProductName", rn ) )
	{
		if ( AddNameValToArray( rn, rv ) )
			return true;
	}

	if ( 0 == strcmp( "version", rn ) && ParseVersion( rv ) )
	{
		if ( AddNameValToArray( "ProductVersion", rv ) )
			return true;
	}

	if ( 0 == strcmp( "ProductVersion", rn ) && ParseVersion( rv ) )
	{
		if ( AddNameValToArray( rn, rv ) )
			return true;
	}

	if ( 0 == strcmp( "company", rn ) )
	{
		if ( AddNameValToArray( "CompanyName", rv ) )
			return true;

	}

	if ( 0 == strcmp( "ProductName", rn ) )
	{
		if ( AddNameValToArray( rn, rv ) )
			return true;
	}

	return false;
}


bool ResourceChange::UpdateResourceInFile()
//Изменить значения ресурсов заданных в массиве
{
	WORD mshi, mslo, lshi, lslo;
	VS_FIXEDFILEINFO *pp;
	//string for VerQueryValue function
	std::wstring resstr = L"\\StringFileInfo\\040904b0\\";
	UINT ulen;
	
	char* resarr = new char[ _sizeresource ];
	if ( !resarr )
			throw MsgInfErr( "Not enough memory" );	

	bool rc = GetFileVersionInfo( _fname, 0, _sizeresource, resarr );
	if ( !rc )
		return false;

	for ( int i = 0; i < CurrIndex; i ++ )
	{
		if ( 0 == wcscmp( ResName[ i ].c_str(), L"ProductVersion" ) )
		{// update number version in VS_FIXEDFILEINFO			
			if( !VerQueryValue( resarr, L"\\", (LPVOID*)&pp, &ulen ) )
			{
				diagmsg.SetMsg( "Error read resource", 0  );
				return false;
			}
			ParseVersion( ValName[ i ].c_str(), &mshi, &mslo, &lshi, &lslo );
			pp->dwFileVersionMS = MAKELONG( mslo, mshi );
			pp->dwFileVersionLS = MAKELONG( lslo, lshi ); 

		}
		//update string resource
		std::wstring respathpar = resstr + ResName[ i ];
		wchar_t* pt;

		if( !VerQueryValue( resarr, respathpar.c_str(), (LPVOID*)&pt, &ulen ) )
			{
				diagmsg.SetMsg( "Error read resource", 0  );
				return false;
			}
		wmemset( pt, 0, ulen );
		wmemcpy_s( pt, ValName[ i ].length(), ValName[ i ].c_str(), ValName[ i ].length() );
		respathpar.clear();
	}
		
	HANDLE hres = BeginUpdateResource( _fname, FALSE);
	if( !hres )
	{
		diagmsg.SetMsg( "Error update resource", 0 );
		return false;
	}

	bool upd = UpdateResource( hres, RT_VERSION, MAKEINTRESOURCE(1), LANG_NEUTRAL, resarr, _sizeresource );
	
	if( !upd )
	{
		diagmsg.SetMsg( "Error update resource", 0 );
		return false;
	}

	upd = EndUpdateResource(hres, FALSE);
	if( !upd )
	{
		diagmsg.SetMsg( "Error end update resource", 0 );
		return false;
	}

	delete[] resarr;

	return true;
}


void ResourceChange::ClearResToUpdate()
{
	for( int i = 0; i < CurrIndex; i ++ )
	{
		ResName[ i ].clear();
		ValName[ i ].clear();
	}
}

ResourceChange::~ResourceChange(void)
{
	ClearResToUpdate();
}


bool ResourceChange::AddNameValToArray( const char* rn, const char* rv )
{
	if ( strlen( rn ) > MaxLenStrValue ) 
		return false;

	if ( ( 0 == strcmp( rn, "ProductVersion" ) || 0 == strcmp( rn, "FileVersion" ) ) && strlen( rn ) > MaxLenStrVersion ) 
		return false;

	if ( CurrIndex >= MaxCountRes )
		return false;
		
	stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	
	ResName[ CurrIndex ].assign( conv.from_bytes( rn ) );
	ValName[ CurrIndex ].assign( conv.from_bytes( rv ) );
	CurrIndex ++;

	return true;
}

bool ResourceChange::ParseVersion( const char* version, WORD *mshi, WORD* mslo, WORD* lshi, WORD *lslo )
// correct version format "0,0,0,0" delimiters , . or space
{
	WORD vr[4] = { 0, 0, 0, 0 };

	int ivr = sizeof( vr )/sizeof( WORD ) - 1;
	
	if( strlen( version ) + 1 > MaxLenStrVersion  )
		diagmsg.SetMsg( "string too length", version );
	else
	{

		const char* tmpv = version + strlen(version) - 1;
		const char* pint;
		
		while( tmpv >= version )
		{
			if( isdigit( *tmpv ) )
				pint = tmpv;

			if( *tmpv == '.' || *tmpv == ',' || *tmpv == ' ' || tmpv == version )
			{
				if ( ivr < 0  )
				{
					diagmsg.SetMsg( "Error format ", version );
					return false;
				}
				vr[ ivr ] = atoi( pint );
				ivr --;
			}

			if ( *tmpv != '.' && *tmpv != ',' && *tmpv != ' ' && !isdigit( *tmpv ) )
			{
				diagmsg.SetMsg( "Error format ", version );
				return false;
			}
		tmpv --;
		}
	}

	*mshi = vr[ 0 ];
	*mslo = vr[ 1 ];
	*lshi = vr[ 2 ];
	*lslo = vr[ 3 ];
	return true;
}


bool ResourceChange::ParseVersion( const wchar_t* version, WORD *mshi, WORD* mslo, WORD* lshi, WORD *lslo )
// correct version format "0,0,0,0" delimiters , . or space
{
	stdext::cvt::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::string versionstr;
	versionstr.assign( conv.to_bytes( version ) );

	return ParseVersion( versionstr.c_str(), mshi, mslo,lshi, lslo );	
}


bool ResourceChange::ParseVersion( const char* version )
{
	WORD vr[4] = { 0, 0, 0, 0 };

	return ParseVersion( version, &vr[0], &vr[1], &vr[2], &vr[3] );
}