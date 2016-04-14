// ************************************************************************ 
//   TokenizerCmd.h - v 0.2
// ************************************************************************ 
#pragma once

#ifndef _CMDINST_TOKENIZERCMD_H
#define _CMDINST_TOKENIZERCMD_H

#include <string>
#include <iostream>

#include "structure.h"
#include "Tokenizer.h"
#include "PackageBuilder.h"

using namespace std;


class TokenizerCmd: public Tokenizer
{
public:
	TokenizerCmd( charstream& cs );
	virtual ~TokenizerCmd( void );

	void RunTokenizer();

private:
	Tokens GetToken();
	void SkipToken( void );
	Tokens CommandBlock();

	Tokens XmlBlock();
	Tokens InstallerBlock();
	Tokens ProductBlock();
	Tokens FileBlock();	
	Tokens InterfaceBlock();

	Tokens CommandOpen();
	Tokens CommandSave();
	Tokens CommandView();
	Tokens CommandTest();
	Tokens CommandBuild();
	Tokens CommandComment();
};

#endif //_CMDINST_TOKENIZERCMD_H