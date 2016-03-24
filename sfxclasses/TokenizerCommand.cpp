#include "StdAfx.h"
//#include "TokenizerCommand.h"
//#include "structure.h"
#include <string>
#include <istream>
#include <iostream>

using namespace std;

//TokenizerCommand::TokenizerCommand(void)
//{
//	length_command = 0;	
//}
//
//
//TokenizerCommand::~TokenizerCommand(void)
//{
//}
//
//bool TokenizerCommand::IsWhitespace( char c )
//{
//   return ( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f');
//}
//
//Tokens TokenizerCommand::GetToken( istream* stream )
//{
//	char c;
//	do
//		stream->get( c );
//	while ( isspace( c ) );
//	
//	do
//	{
//		command[ length_command ++ ] = c;
//		stream->get( c );
//	}
//	while( !isspace( c ) );
//	command[ length_command ] = 0;
//
//	if ( 0 == strcmp( CommandList[ 0 ].Name, command ) )
//	{
//		curr_function = &TokenizerCommand::OpenCommand;
//		(this->*curr_function)();
//		//RunCommand();
//		return TOKOPEN;
//	}
//	if ( 0 == strcmp( CommandList[ 6 ].Name, command ) )
//	{
//		curr_function = &TokenizerCommand::ExitCommand;
//		return TOKEXIT;
//	}
//	return TOKUNKNOWN;
//}
//
//char* TokenizerCommand::GetCommand()
//{
//	command[ length_command ] = '\0';
//	return command;
//}
//
//Tokens TokenizerCommand::OpenCommand()
//{
//	return TOKOPEN;
//}
//
//Tokens TokenizerCommand::ExitCommand()
//{
//	return TOKEXIT;
//}
//
//char TokenizerCommand::GetChar( char c )
//{
//	if( _fromcin )
//	{
//		cin.get( c );
//		return c;
//	}
//	if ( _frombuf )
//		return c;
//}