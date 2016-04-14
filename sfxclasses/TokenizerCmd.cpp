// ************************************************************************ 
//   TokenizerCmd.cpp - v 0.2
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
	while ( isalpha( (unsigned char)c ) )
	{
		name += c;
		c = GetChar();
	}
	
	if( !isspace( c ) && c !='"' && 0 != c )
	{
		name += c;
		return TOKUNKNOWN;
	}	

	return FindElemCmd( curr_tok, name.c_str() );
}


void TokenizerCmd::SkipToken( void )
{
	char c;	
	if( !StreamEmpty() )
	{
		do
			c = GetChar();
		while( c != '\n' );
	}
	return;
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
				diagmsg.SetMsg( name.c_str(), "the command is unrecognized or invalid in the current block" );
				diagmsg.PrintMsg();
				SkipToken();
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
		case TOKINSTFILE: case TOKINSTEXTRACT: case TOKINSTINTERFACE:
		case TOKINTERFACEPROGRESS: case TOKINTERFACECOLOR:
		case TOKPRODNAME: case TOKPRODVERSION: case TOKPRODCOMPANY:
		case TOKFILNAME: case TOKFILEXTRACT:
		{
			diagmsg.SetMsg( FindElement( curr_tok )->NameOpenTag, "the command is not valid in the current block" );
			diagmsg.PrintMsg();
			SkipToken();
			curr_tok = GetToken();				
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
	string currname;

	currdoc.AddElement( TOKINSTALLER, name.c_str() );

	curr_tok = GetToken();

	while( curr_tok == TOKINSTFILE || curr_tok == TOKINSTEXTRACT || curr_tok == TOKCOMMENT || curr_tok == TOKINSTINTERFACE )
	{
		currname.assign( name );

		if( curr_tok == TOKINSTFILE )
		{//вызов обработчика директивы name
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKINSTFILE, currname.c_str(), value.c_str() );			
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
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKINSTEXTRACT, currname.c_str(), value.c_str() );	
			else
			{
				currdoc.AddElement( TOKINSTEXTRACT, currname.c_str(), value.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKINSTINTERFACE )
		//вызов обработчика блока interface
		{
			InterfaceBlock();
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
	string currname;

	currdoc.AddElement( TOKPRODUCT, name.c_str() );

	curr_tok = GetToken();

	while ( curr_tok == TOKPRODNAME || curr_tok == TOKPRODVERSION || curr_tok == TOKPRODCOMPANY || curr_tok == TOKCOMMENT )
	{
		currname.assign( name );

		if( curr_tok == TOKPRODNAME)
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODNAME, currname.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODNAME, currname.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKPRODVERSION )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODVERSION, currname.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODVERSION, currname.c_str() );
				continue;
			}
		}

		if( curr_tok == TOKPRODCOMPANY )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKPRODCOMPANY, currname.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKPRODCOMPANY, currname.c_str() );
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
	string currname;

	currdoc.AddElement( TOKFILE, name.c_str() );
	currdoc.Movecurrparent();

	curr_tok = GetToken();

	while( curr_tok == TOKFILNAME || curr_tok == TOKFILEXTRACT || curr_tok == TOKCOMMENT )
	{
		currname.assign( name );
		
		if(  curr_tok == TOKFILNAME )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )				
				currdoc.AddElement( currname.c_str(), value.c_str() );
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
				currdoc.AddElement( currname.c_str(), value.c_str() );
			else
			{				
				currdoc.AddElement( currname.c_str(), 0 );
				continue;
			}
		}

		if( curr_tok == TOKCOMMENT )
			CommandComment();

		curr_tok = GetToken();
	}	
	return curr_tok;
}


Tokens TokenizerCmd::InterfaceBlock()
{
	string currname;

	currdoc.AddElement( TOKINSTINTERFACE, name.c_str() );	
	curr_tok = GetToken();
	
	while( curr_tok == TOKINTERFACEPROGRESS || curr_tok == TOKINTERFACECOLOR || curr_tok == TOKCOMMENT )
	{	
		currname.assign( name );

		if( curr_tok == TOKINTERFACEPROGRESS )
		{			
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKINTERFACEPROGRESS, currname.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKINTERFACEPROGRESS, currname.c_str() );
				continue;
			}
		}
		
		if( curr_tok == TOKINTERFACECOLOR )
		{
			curr_tok = GetToken();
			if( curr_tok == TOKPARAM )
				currdoc.AddElement( TOKINTERFACECOLOR, currname.c_str(), value.c_str() );
			else
			{
				currdoc.AddElement( TOKINTERFACECOLOR, currname.c_str() );
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
		diagmsg.SetMsg( "Build start ", 0 );
		diagmsg.PrintMsg();

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