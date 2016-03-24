#include "StdAfx.h"
#include "MsgInfErr.h"


#include <string>
#include <iostream>

MsgInfErr::MsgInfErr(const char* msg)
{
	_msginferr.assign( msg );
	MsgPresent = true;
}


MsgInfErr::MsgInfErr( const char* msg, const char* inf )
{
	_msginferr.assign( msg ) + " " + inf;
	MsgPresent = true;
}


MsgInfErr::MsgInfErr()
{
	MsgPresent = false;
}

MsgInfErr::~MsgInfErr(void)
{
}

void MsgInfErr::SetMsg( const char* msg, const char* inf = 0 )
{
	if ( 0 == inf )
		_msginferr.assign( msg );
	else
		_msginferr.assign( msg ) + " " + inf;
	
	MsgPresent = true;
}

void MsgInfErr::PrintMsg()
{
	std::cout << _msginferr << std::endl;
	MsgPresent = false;
}

MsgInfErr diagmsg;