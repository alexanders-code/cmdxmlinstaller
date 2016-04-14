// ************************************************************************ 
//   Tokenizer.cpp - v 0.1
// ************************************************************************
#include "StdAfx.h"

#include "Tokenizer.h"

Tokenizer::Tokenizer( charstream& cs ): _input( cs )
{
}


Tokenizer::~Tokenizer()
{
}


char Tokenizer::GetChar()
{
	return _input.get_char();
}


void Tokenizer::PutBackChar()
{
	_input.putback_char();
}


bool Tokenizer::StreamEmpty( void )
{
	return _input.CharStreamEmpty();
}


charstream& Tokenizer::GetInput()
{
	return _input;
}

char Tokenizer::SkipWhitespace()
{
	while ( char c = GetChar() )
	{
		if ( c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\f' )
			return c;
	}
	return 0;
}

