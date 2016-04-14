// ************************************************************************ 
//   structure.cpp - v 0.3
// ************************************************************************

#include "StdAfx.h"

#include <string.h>
#include <string>

#include "structure.h"

Element Etalon[ EtalonSz ] = {	{ "installer", "/installer", YES, 1, 0, NO, TOKINSTALLER },
							{ "name", "/name", YES, 1, &Etalon[ 0 ], YES, TOKINSTFILE },
							{ "extract", "/extract", NO, 1, &Etalon[ 0 ], NOORYES, TOKINSTEXTRACT },
							{ "interface", "/interface", NO, 1, &Etalon[ 0 ], NO, TOKINSTINTERFACE },
							{ "progress", "/progress", NO, 1, &Etalon[ 3 ], NOORYES, TOKINTERFACEPROGRESS },
							{ "color" , "/color", NO, 1, &Etalon[ 3 ], NOORYES, TOKINTERFACECOLOR },
						
							{ "product", "/product", NO, 1, 0, NOORYES, TOKPRODUCT },
							{ "name", "/name", NO, 1, &Etalon[ 6 ], NOORYES, TOKPRODNAME }, 
							{ "version", "/version", NO, 1, &Etalon[ 6 ], NOORYES, TOKPRODVERSION },
							{ "company", "/company", NO, 1, &Etalon[ 6 ], NOORYES, TOKPRODCOMPANY },

							{ "file", "/file", YES, -1, 0, NO, TOKFILE },
							{ "name", "/name", YES, 1, &Etalon[ 10 ], YES, TOKFILNAME },
							{ "extract", "/extract", NO, 1, &Etalon[ 10 ], NOORYES, TOKFILEXTRACT },

							{ "!--", "--", NO, -1, 0, NOORYES, TOKCOMMENT }
						};

Commands CommandList[ CommandListSz ] = { { "open", TOKOPEN }, { "save", TOKSAVE }, { "view", TOKVIEW }, { "test", TOKTEST }, { "build", TOKBUILD }, { "comment", TOKCOMMENT }, { "exit", TOKEXIT } };


DefVariable DefVariableArr[ DefVariableArrSz ] = {
													{ "progress", TOKINTERFACEPROGRESS, "bar|percent", 1 },
													{ "color", TOKINTERFACECOLOR, "red|green|blue", 3 }
												 };

static Tokens PrevToken = TOKCOMMENT;

Element* FindElement( Tokens t );
bool ElementHaveChild( Tokens t );


Element* FindElement( Tokens t )
//поиск Tokens только в массиве Etalon
//возвращает указатель на найденный Element или 0 если нет
{	
	for( int i = 0; i < EtalonSz; i ++ )
	{
		if ( Etalon[ i ].Tn == t )
			return &Etalon[ i ];
	}
	return 0;
}


//истина если t имеет дочерние элеметы
bool ElementHaveChild( Tokens t )
{
	Element* tmp = FindElement( t );
	if( !tmp )
		return true;

	for( int i = 0; i < EtalonSz; i ++ )
	{
		if( Etalon[ i ].BaseElement == tmp )
			return true;
	}

	return false;
}


int LevelElement( const Element* pe );


Tokens FindElemCmd( Tokens t, const char* str )
{
	int i;

	if( TOKPARAM == t || TOKUNKNOWN == t )
		t = PrevToken;

	for ( i = 0; i < CommandListSz; i ++ )
	{
		if( 0 == strcmp( str, CommandList[ i ].Name ) )			
		{
			if( TOKCOMMENT == CommandList[ i ].Tn )
				PrevToken = CommandList[ i ].Tn;
			return CommandList[ i ].Tn;
		}
	}

	Element* parentt = FindElement( t );
	if( !parentt )
	{	
		for( i = 0; i < EtalonSz; i ++ )
		{
			if( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) && 0 == Etalon[ i ].BaseElement )
			{
				PrevToken = Etalon[ i ].Tn;
				return Etalon[ i ].Tn;
			}
		}
		return TOKUNKNOWN;
	}

	while( 0 != parentt -> BaseElement )
		parentt = parentt -> BaseElement;

	Element* parentstr = 0;
	Element* elementstr = 0;
	for( i = 0; i < EtalonSz; i ++ )
	{
		if( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) )
		{
			parentstr = &Etalon[ i ];
			while( 0 != parentstr -> BaseElement )
				parentstr = parentstr -> BaseElement;
			
			if( parentstr == parentt )
			{
				elementstr = & Etalon[ i ];
				break;
			}
		}
	}

	if( parentstr == parentt )
	{
		int levelt = LevelElement( FindElement( t ) );
		int levelstr = LevelElement( elementstr );

		if( levelstr < levelt )		
		{
			PrevToken = elementstr -> Tn;
			return elementstr -> Tn;
		}

		if( levelstr == levelt || ( levelstr - levelt ) == 1 )
		{
			PrevToken = elementstr -> Tn;
			return elementstr -> Tn;
		}
	}
	
	for( i = 0; i < EtalonSz; i ++ )
	{
		if( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) && 0 == Etalon[ i ].BaseElement )
		{
			PrevToken = Etalon[ i ].Tn;
			return Etalon[ i ].Tn;
		}
	}	
	
	return TOKUNKNOWN;
}


//уровень Element в иерархии, верхний уровень = 0
int LevelElement( const Element* pe )
{
	int l = 0;
	while( 0 != pe->BaseElement )
	{
		l ++;
		pe = pe -> BaseElement;
	}

	return l;
}


//проверка содержимого val на соответствие допустимым значениям из DefVariableArr
bool VariableValControl( Tokens tn, const char* val )
{
	std::string tmp;

	for( int i = 0; i < DefVariableArrSz; i ++ )
	{
		if( DefVariableArr[ i ].Tn != tn )
			continue;

		while( val )
		{
			tmp.clear();
			while( isspace( *val ) || '\"' == *val || '|' == *val )
				val ++;

			if( '\0' == *val )
				break;

			while( isalpha( *val ) )
			{
				tmp += *val;
				val ++;
			}

			if( !strstr( DefVariableArr[ i ].ValidValues, tmp.c_str() ) )
				return false;
		}
		return true;
	}

	return false;
}

