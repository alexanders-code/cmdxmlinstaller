// ************************************************************************ 
//   TokenizerCmd.cpp - v 0.0.1
// ************************************************************************ 

#include "StdAfx.h"

#include "TokenizerCmd.h"
#include "FileOperations.h"
#include "DocInstaller.h"
#include "MsgInfErr.h"
#include "structure.h"



TokenizerCmd::TokenizerCmd( charstream& cs ): Tokenizer( cs )
{	

}

TokenizerCmd::~TokenizerCmd()
{
}

void TokenizerCmd::RunTokenizer()
{
	curr_tok = GetToken();
	while ( curr_tok != TOKEXIT )
	{
		CommandBlock();
	}
}

Tokens TokenizerCmd::GetToken()
{
	char c;
	
	c = SkipWhitespace();	

	if ( 0 == c )
		return TOKEOS;
	
	value.clear();
	if ( '"' == c )
	{
		//value += c;
		do
		{
			c = GetChar();
			if ( 0 == c )
				return TOKEOS;
			if ( c != '"' )
				value += c;
		}
		while ( c != '"' );		
		return TOKPARAM;
	}

	name.clear();
	while ( isalpha( c ) )
	{
		name += c;
		c = GetChar();
	}
	
	if( !isspace( c ) && c !='"' && 0 != c )
	{
		name += c;
		return TOKUNKNOWN;
	}	

	return FindElemCmd( name.c_str() );
}

extern DocInstaller currdoc;


Tokens TokenizerCmd::CommandBlock()
{
	switch( curr_tok )
	{
		case TOKOPEN:
			{
				CommandOpen();
				break;
			}
		case TOKSAVE: 
			{
				CommandSave();
				break;
			}
		case TOKVIEW:
			{
				CommandView();
				break;
			}
		case TOKTEST: 
			{
				CommandTest();
				break;
			}
		case TOKBUILD:
			{
				CommandBuild();
				break;
			}
		case TOKCOMMENT:
			{
				CommandComment();
				break;
			}
		case TOKEXIT: break;
		case TOKUNKNOWN: 
			{
				diagmsg.SetMsg("Command unknown ", 0 );
				diagmsg.PrintMsg();
				curr_tok = GetToken();
				break;
			}
		default:
			{
				XmlBlock();
			}
	}
	
	return curr_tok;
}


Tokens TokenizerCmd::XmlBlock()
{
	switch( curr_tok )
	{
		case TOKINSTALLER:
		{
			InstallerBlock();
			break;
		}
		case TOKPRODUCT:
		{
			ProductBlock();
			break;
		}
		case TOKFILE:		
		{
			FileBlock();
			break;
		}		
		case TOKUNKNOWN: 
		{
			break;
		}
		case TOKCOMMENT:
		{
			CommandComment();
			break;
		}
		default:
			break;
	}
	

	if( curr_tok == TOKPARAM )
		curr_tok = GetToken();

	return curr_tok;
}

Tokens TokenizerCmd::InstallerBlock()
{
	currdoc.AddElement( TOKINSTALLER, name.c_str() );

	curr_tok = GetToken();

	while( curr_tok == TOKINSTFILE || curr_tok == TOKINSTEXTRACT || curr_tok == TOKCOMMENT )
	{
		if( curr_tok == TOKINSTFILE )
		{//вызов обработчика директивы file
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
			{
				currdoc.AddElement( TOKINSTFILE, name.c_str(), value.c_str() );
			}
			else
			{
				diagmsg.SetMsg( "Parametrs file name absent", 0 );
				diagmsg.PrintMsg();
				continue;
			}
		}

		if( curr_tok ==  TOKINSTEXTRACT )
		//вызов обработчика директивы extract
		{
			curr_tok = GetToken();
			currdoc.AddElement( TOKINSTEXTRACT, name.c_str(), value.c_str() );			
			continue;
		}

		if( curr_tok == TOKCOMMENT )
			CommandComment();

		curr_tok = GetToken();
	}

	return curr_tok;
}


Tokens TokenizerCmd::ProductBlock()
{
	currdoc.AddElement( TOKPRODUCT, name.c_str() );

	curr_tok = GetToken();

	while ( curr_tok == TOKPRODNAME || curr_tok == TOKPRODVERSION || curr_tok == TOKPRODCOMPANY || curr_tok == TOKCOMMENT )
	{
		if( curr_tok == TOKPRODNAME)
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODNAME, name.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODNAME, name.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKPRODVERSION )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODVERSION, name.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODVERSION, name.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKPRODCOMPANY )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODCOMPANY, name.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODCOMPANY, name.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKCOMMENT )
			CommandComment();

		curr_tok = GetToken();
	}	

	return curr_tok;
}


Tokens TokenizerCmd::FileBlock()
{
	currdoc.AddElement( TOKFILE, name.c_str() );

	curr_tok = GetToken();

	while( curr_tok == TOKFILNAME || curr_tok == TOKFILEXTRACT || curr_tok == TOKCOMMENT )
	{
		
		if(  curr_tok == TOKFILNAME )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKFILNAME, name.c_str(), value.c_str() );
			else
			{
				diagmsg.SetMsg( "\nFor file name is missing \"value\"", 0 );
				diagmsg.PrintMsg();
				continue;
			}
		}

		if(  curr_tok == TOKFILEXTRACT )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKFILEXTRACT, name.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKFILEXTRACT, name.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKCOMMENT )
			CommandComment();

		curr_tok = GetToken();
	}	
	
	return curr_tok;
}


Tokens TokenizerCmd::CommandOpen()
// команда Open
{
	curr_tok = GetToken();

	if( curr_tok == TOKPARAM )
	{
		FileOperations xmlopen;
		xmlopen.OpenForRead( value.c_str() );
		
		if( diagmsg.MsgPresent )
			diagmsg.PrintMsg();
		else
		{
			xmlopen.ReadInBuf();
			xmlopen.CloseFile();
			GetInput().ResizeCurrString( xmlopen.GetDataBuf(), xmlopen.GetActualSizeBuf() );
		}
	}

	return curr_tok;
}


Tokens TokenizerCmd::CommandSave()
{
	curr_tok = GetToken();

	if( curr_tok == TOKPARAM )
	{
		FileOperations fts;
				
		fts.OpenForWrite( value.c_str() );
		if ( diagmsg.MsgPresent )
		{
			diagmsg.PrintMsg();			
			return curr_tok;
		}
		fts.CreateBuf();

		vector< string > tv;
		currdoc.AllDocToVector( &tv );
		int i = 0;
		while( i < tv.size() )
		{
			fts.StringToDataBuf( tv.at( i ).c_str() );
			i++;
		}
		fts.WriteFromBuf( fts.GetDataBuf(), fts.GetActualSizeBuf() );
		if ( diagmsg.MsgPresent )
			diagmsg.PrintMsg();			
			
		fts.CloseFile();
		fts.ClearDelBuf();
		tv.clear();
	}

	curr_tok = GetToken();

	return curr_tok;
}


Tokens TokenizerCmd::CommandView()
{
	cout << "\n";
	vector< string > tv;
	currdoc.AllDocToVector( &tv );
	int i = 0;
	while( i < tv.size() )
		cout << tv.at( i ++ );
	cout << "\n";

	curr_tok = GetToken();

	return curr_tok;
}


Tokens TokenizerCmd::CommandTest()
{
	currdoc.Test();
	curr_tok = GetToken();

	return curr_tok;
}


Tokens TokenizerCmd::CommandBuild()
{
	PackageBuilder module( &currdoc );

	if( currdoc.Test() )
	{
		module.BuildSelfInstaller();
		
		if( diagmsg.MsgPresent )
			diagmsg.PrintMsg();
		else
		{
			diagmsg.SetMsg( "Build complete ", 0 );
			diagmsg.PrintMsg();
		}
	}

	curr_tok = GetToken();

	return curr_tok;
}


Tokens TokenizerCmd::CommandComment()
{
	curr_tok = GetToken();

	if( curr_tok == TOKPARAM )	
		currdoc.AddComment( name.c_str(), value.c_str() );
	else
	{
		diagmsg.SetMsg( "Text comment absent", 0 );
		diagmsg.PrintMsg();
	}

	return curr_tok;
}