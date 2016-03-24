#pragma once

#include <istream>

using namespace std;

//
//enum Tokens
//{
//	TOKERR = -1,
//	TOKEOF = 0,
//	TOKOPEN = 1,
//	TOKSAVE,
//	TOKVIEW,
//	TOKTEST,
//	TOKBUILD,
//	TOKCOMMEND,
//	TOKEXIT,
//	TOKUNKNOWN
//};

//class TokenizerCommand
//{
//public:
//	TokenizerCommand(void);
//	virtual ~TokenizerCommand(void);
//	char GetChar( char c );
//	Tokens GetToken(istream* stream);
//	void SetCurrInputCin() { _fromcin = true; _frombuf = false; };
//	void SetCurrInputBuf() { _fromcin = false; _frombuf = true; };
//	char* GetCommand();
//	void RunCommand()
//	{
//		(this->*curr_function)();
//	}
//
//private:
//
//	Tokens _currTokens;
//	typedef Tokens ( TokenizerCommand::*pointerfunc )();
//	pointerfunc curr_function;
//
//	//Tokens GetCommand();
//	Tokens OpenCommand();
//	Tokens SaveCommand();
//	Tokens ViewCommand();
//	Tokens TestCommand();
//	Tokens BuildCommand();
//	Tokens Installercommand();
//	Tokens ProductCommand();
//	Tokens CommentCommand();
//	Tokens ExitCommand();
//	bool IsWhitespace( char c );
//
//	char command[ 30 ];
//	int length_command;
//	bool _fromcin;
//	bool _frombuf;
//	
//};

