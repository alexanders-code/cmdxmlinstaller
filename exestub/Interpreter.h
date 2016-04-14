// ************************************************************************ 
//   interpreter.h - v 0.1
// ************************************************************************ 

#pragma once

#ifndef _CMDINST_INTERPRETER_H
#define _CMDINST_INTERPRETER_H

#include <vector>

#include "exestub.h"

class Interpreter
{
public:
	Interpreter( const std::vector< CommandBlock >* cb );
	virtual ~Interpreter(void);

	DWORD GetColor( void );
	
	const char* GetProgress( void );

private:

	char* GetToken( const char* s );
	
	static const int _maxtokenlength = 20;
	char token[ _maxtokenlength ];
	int _currpos;

	const std::vector< CommandBlock >* _cb;
};

#endif //_CMDINST_INTERPRETER_H
