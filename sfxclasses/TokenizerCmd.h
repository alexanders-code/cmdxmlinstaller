// ************************************************************************ 
//   TokenizerCmd.h - v 0.0.1
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
	Tokens CommandBlock();

	Tokens XmlBlock();
	Tokens InstallerBlock();
	Tokens ProductBlock();
	Tokens FileBlock();	

	Tokens CommandOpen();
	Tokens CommandSave();
	Tokens CommandView();
	Tokens CommandTest();
	Tokens CommandBuild();
	Tokens CommandComment();
};

#endif //_CMDINST_TOKENIZERCMD_H