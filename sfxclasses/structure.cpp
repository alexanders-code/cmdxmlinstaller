#include "StdAfx.h"
#include "structure.h"

Element Etalon[ EtalonSz ] = {	{ "installer", "/installer", YES, 1, 0, NO, TOKINSTALLER },
							{ "name", "/name", YES, 1, &Etalon[ 0 ], YES, TOKINSTFILE },
							{ "extract", "/extract", NO, 1, &Etalon[ 0 ], NOORYES, TOKINSTEXTRACT },
						
							{ "product", "/product", NO, 1, 0, NOORYES, TOKPRODUCT },
							{ "name", "/name", NO, 1, &Etalon[ 3 ], NOORYES, TOKPRODNAME }, 
							{ "version", "/version", NO, 1, &Etalon[ 3 ], NOORYES, TOKPRODVERSION },
							{ "company", "/company", NO, 1, &Etalon[ 3 ], NOORYES, TOKPRODCOMPANY },

							{ "file", "/file", YES, -1, 0, NO, TOKFILE },
							{ "name", "/name", YES, 1, &Etalon[ 7 ], YES, TOKFILNAME },
							{ "extract", "/extract", NO, 1, &Etalon[ 7 ], NOORYES, TOKFILEXTRACT },

							{ "!--", "--", NO, -1, 0, NOORYES, TOKCOMMENT }
						};

Commands CommandList[ CommandListSz ] = { { "open", TOKOPEN }, { "save", TOKSAVE }, { "view", TOKVIEW }, { "test", TOKTEST }, { "build", TOKBUILD }, { "comment", TOKCOMMENT }, { "exit", TOKEXIT } };


static Tokens PrevToken = TOKCOMMENT;

Element* FindElement( Tokens t );

Tokens FindElemCmd( const char* str )
{
	int i;

	for ( i = 0; i < CommandListSz; i ++ )
	{
		if( 0 == strcmp( str, CommandList[ i ].Name ) )
		{
			PrevToken = CommandList[ i ].Tn;
			return CommandList[ i ].Tn; 
		}
	}

	Element *tmp = FindElement( PrevToken );
	if ( 0 != tmp )
	{
		
		for( i = 0; i < EtalonSz; i ++ )
		{
			if ( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) && Etalon[ i ].BaseElement == tmp )
				return Etalon[ i ].Tn;
		}
		
	}

	for( i = 0; i < EtalonSz; i ++ )
	{
		if ( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) && Etalon[ i ].BaseElement == 0 )
		{
			PrevToken = Etalon[ i ].Tn;
			return Etalon[ i ].Tn;
		}
	}
	
	
	/*for( i = 0; i < EtalonSz; i ++ )
	{
		if ( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) && Etalon[ i ].BaseElement == 0 )
		{
			PrevToken = Etalon[ i ].Tn;
			return Etalon[ i ].Tn;
		}
	}*/
		
	return TOKUNKNOWN;
}

//Tokens FindElemCmd( const char* str )
//{
//	int i;
//	for( i = 0; i < EtalonSz; i ++ )
//	{
//		if ( 0 == strcmp( str, Etalon[ i ].NameOpenTag ) || 0 == strcmp( str, Etalon[ i ].NameCloseTag ) )
//			return Etalon[ i ].Tn;
//	}
//
//	for ( i = 0; i < CommandListSz; i ++ )
//	{
//		if( 0 == strcmp( str, CommandList[ i ].Name ) )
//			return CommandList[ i ].Tn; 
//	}
//
//	return TOKUNKNOWN;
//}

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