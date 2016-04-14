// ************************************************************************ 
//   structure.h - v 0.3
// ************************************************************************

#pragma once

#ifndef _CMDINST_STUCTURE_H
#define _CMDINST_STUCTURE_H

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
	TOKINSTINTERFACE,
	TOKINTERFACEPROGRESS,
	TOKINTERFACECOLOR,
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
	mandatory Param;//обязательность наличия параметра
	Tokens Tn;
};


static const int EtalonSz = 14;

extern Element Etalon[ EtalonSz ];

struct Commands
{
	char* Name;
	Tokens Tn;
};

static const int CommandListSz = 7;
extern Commands CommandList[ CommandListSz ];

extern Tokens FindElemCmd( const char* str );
extern Tokens FindElemCmd( Tokens t, const char* str );
extern Element* FindElement( Tokens t );

//список допустимых значений переменных
struct DefVariable
{
	char* Name;
	Tokens Tn;
	char* ValidValues;
	int CountVal;
};

static const int DefVariableArrSz = 2;
extern DefVariable DefVariableArr[ DefVariableArrSz ];

extern bool VariableValControl( Tokens tn, const char* val );

#endif // _CMDINST_STUCTURE_H