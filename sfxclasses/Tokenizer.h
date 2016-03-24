#pragma once

#ifndef _CMDINST_TOKENIZER_H
#define _CMDINST_TOKENIZER_H

#include <string>
#include <iostream>

#include "structure.h"

using namespace std;

struct charstream
{
	string currstr;
	const char* start;
	const char* p;	
	const char* end;

  charstream()
  {	 
	  start = p = end = 0;
  }

  virtual char get_char()
  {
	  char ch;

	  if( p == end )
	  {
		  currstr.clear();		 
		  do
		  {
			  cin.get( ch );
			  currstr += ch;
		  }
		  while ( ch != '\n' );

		  p = currstr.c_str();
		  start = p;
		  end = p + currstr.length();
	  }

	  return p < end? *p++: 0;
  }


  virtual void putback_char()
  {
	  if ( !(p == start) )
		  p --;
  }

  void ResizeCurrString( const char* pf, unsigned int sz )
  {
	  int i = 0;
	  const char* tmp;
	  tmp = start;
	  
	  while ( tmp != p )
	  {
		  tmp ++;
		  i ++;
	  }

	  currstr.insert( i, pf, sz );

	  ReplaceCommentTag();
	  ReplaceClosedTag();
	  ReplaceQuote( "<" );
	  ReplaceQuote( ">" );

	  p = currstr.c_str() + i;
	  start = p;
	  end = p + strlen( pf );
  }


 void ReplaceQuote( const char* p )
// заменить в currstr заданную подстроку пробелами
// для удаления  < > </ из currstr
{
	size_t i = 0;	
	
	i = currstr.find( p, i );
	while ( i != string::npos )
	{
		currstr.replace( i, strlen( p ), strlen( p ), ' ' );
		i = currstr.find( p, i );
	}
}


void ReplaceClosedTag()
{
	size_t i = 0;	

	i = currstr.find( "</", i, 2 );
	while ( i != string::npos )
	{
		size_t j = currstr.find( ">", i, 1 );
		for( size_t k = i; k <= j; k ++ )
			currstr.at( k ) = ' ';
		i = currstr.find( "</", i, 2 );
	}
}

void ReplaceCommentTag()
{
	size_t i = 0;	
	char* open = "<!--";
	char* close = "-->";
	char* nametext = "comment";

	i = currstr.find( open, i );
	while ( i != string::npos )
	{
		for( size_t k = i; k < i + strlen( open ); k ++ )
			currstr.at( k ) = ' ';

		currstr.insert( i, nametext, 0, strlen( nametext ) );

		i = currstr.find( close, i );
		for( size_t k = i; k < i + strlen( close ); k ++ )
			currstr.at( k ) = ' ';

		i = currstr.find( open, i );
	}
}

};


//static const int MAXNAMESIZE = 128;
//static const int MAXVALUESIZE = 128;

class Tokenizer
{
public:

	Tokenizer( charstream& cs );
	virtual ~Tokenizer( void );

	charstream& GetInput();

protected:

	char GetChar();
	char SkipWhitespace();
	void PutBackChar();
	
	string name;
	string value;
	Tokens curr_tok;

	

private:	
	
	charstream& _input;
	
};


#endif //_CMDINST_TOKENIZER_H