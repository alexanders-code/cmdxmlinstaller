#pragma once

#ifndef _CMDINST_STUCTURE_H
#define _CMDINST_STUCTURE_H

#include <vector>

enum mandatory { NO = 0, YES = 1, NOORYES };

enum Tokens
{
	TOKERR = -1,
	TOKEOF = 0,
	TOKEOS = 1,
	TOKOPEN,
	TOKSAVE,
	TOKVIEW,
	TOKTEST,
	TOKBUILD,
	TOKCOMMENT,	
	TOKEXIT,

	TOKINSTALLER,
	TOKINSTFILE,
	TOKINSTEXTRACT,
	TOKPRODUCT,
	TOKPRODNAME,
	TOKPRODVERSION,
	TOKPRODCOMPANY,
	TOKFILE,
	TOKFILNAME,
	TOKFILEXTRACT,
	
	TOKPARAM,

	TOKUNKNOWN
};

struct Element
{
	char* NameOpenTag;
	char* NameCloseTag;
	mandatory Melem;
	int Repetition;	
	Element* BaseElement;	
	mandatory Param;
	Tokens Tn;
};


static const int EtalonSz = 11;

extern Element Etalon[ EtalonSz ];

struct Commands
{
	char* Name;
	Tokens Tn;
};

static const int CommandListSz = 7;
extern Commands CommandList[ CommandListSz ];

extern Tokens FindElemCmd( const char* str );
extern Element* FindElement( Tokens t );



#endif // _CMDINST_STUCTURE_H