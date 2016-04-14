// sfxclasses.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FileOperations.h"
//#include "structure.h"

#include <io.h>
#include <cstdio>
#include <iostream>
#include <istream>
#include <Windows.h>
#include <stdlib.h>

#include "Tokenizer.h"
#include "TokenizerCmd.h"
#include "DocInstaller.h"
#include "MsgInfErr.h"
#include "ResourceChange.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, "" );
		
	charstream cs;
	TokenizerCmd tc( cs );
	tc.RunTokenizer();
	
}


