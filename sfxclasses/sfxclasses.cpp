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

//struct proba
//{
//	int number;
//	vector< proba > pvec;
//	proba(int i )
//	{ number = i; };
//};
//
//proba* rerucobh( int n, vector< proba >* p  )
//{
//	static proba* fi = 0;
//
//	for(int i =0; i < p->size(); i ++ )
//	{
//		if ( n == p->at( i ).number )
//		{	
//			fi = &p->at(i);
//			return fi;
//		}
//		else
//			rerucobh( n, &p->at(i).pvec );
//	}
//	return fi;
//}

void func( vector<string >* pv )
{
	pv->push_back( "one" );
	pv->push_back("two");
}


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale( LC_ALL, ".ACP" );
		
	charstream cs;
	TokenizerCmd tc( cs );
	tc.RunTokenizer();
	
	
}


