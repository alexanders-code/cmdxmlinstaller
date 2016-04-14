// ************************************************************************ 
//   interpreter.cpp - v 0.1
// ************************************************************************ 

#include "StdAfx.h"
#include "Interpreter.h"

#include <WinCon.h>
#include <string.h>


Interpreter::Interpreter( const std::vector< CommandBlock >* cb )
{
	_cb = cb;
	_currpos = 0;
}


Interpreter::~Interpreter(void)
{
}

// получить цвет из элемента interface/color
DWORD Interpreter::GetColor( void )
{
	DWORD color = 0;

	for( size_t i = 0; i < _cb -> size(); i ++ )
	{
		if( !strcmp( _cb -> at( i ).Command, "interface/color" ) )
		{
			while( strlen( GetToken( _cb -> at( i ).Parameter ) ) )
			{
				if( !strcmp( token, "blue" ) )
					color = color | FOREGROUND_BLUE;
				if( !strcmp( token, "green" ) )
					color = color | FOREGROUND_GREEN;
				if( !strcmp( token, "red" ) )
					color = color | FOREGROUND_RED;
			}

			return color;
		}
	}

	return color;
}


// получить тип вывода информации о процессе установки из элемента interface/progress
const char* Interpreter::GetProgress( void )
{
	for( size_t i = 0; i < _cb -> size(); i ++ )
	{
		if( 0 == strcmp( _cb -> at( i ).Command, "interface/progress" )  )		
			return _cb -> at( i ).Parameter;		
	}

	return 0;
}


char* Interpreter::GetToken( const char* s )
{
	int pos =0;

	memset( token, '\0', _maxtokenlength );
	
	while( s[ _currpos ] != '\0' && pos < _maxtokenlength )
	{
		while( isspace( s[ _currpos ] ) || '|' == s[ _currpos ] )
			_currpos ++;

		while( isalpha( s[ _currpos ] ) )
			token[ pos ++ ] = s[ _currpos ++ ];
		
		if( pos )
			return token;
	}
	
	if( '\0' == s[ _currpos ] )
		_currpos = 0;

	return token;
}