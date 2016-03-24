// ************************************************************************ 
//   MsgInfErr.h - v 0.0.1
// ************************************************************************ 


#pragma once

#ifndef _CMDINST_MSGINFERR_H
#define _CMDINST_MSGINFERR_H

#include <string>

class MsgInfErr
{

public:
	MsgInfErr(const char* msg);

	MsgInfErr( const char* msg, const char* inf );

	MsgInfErr();

	virtual ~MsgInfErr(void);

	void SetMsg( const char* msg, const char* inf );

	void PrintMsg();

	bool MsgPresent;
	
private:
	
	std::string _msginferr;


};

//static MsgInfErr diagmsg;

extern MsgInfErr diagmsg;

#endif //_CMDINST_MSGINFERR_H