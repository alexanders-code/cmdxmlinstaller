// ************************************************************************ 
//   DocInstaller.h - v 0.4
// ************************************************************************ 

#pragma once

#ifndef _CMDINST_DOCINSTALLER_H
#define _CMDINST_DOCINSTALLER_H

#include "structure.h"

#include <string>
#include <vector>

class DocInstaller
{
public:	

	DocInstaller(void);

	virtual ~DocInstaller(void);
	
	void AddElement( const char* name, const char* val ); 
	
	void AddElement( Tokens t, const char* name, const char* val = 0 );

	void AddComment( const char* name, const char* val );

	void DelElement( const char* name, const char* val );
	
	void Movecurrparent( Tokens t );

	void Movecurrparent( void );

	int GetValueFromParent( int pos, const char* name, std::string& val );

	void DeleteAllVector( void );

	void AllDocToVector( std::vector< std::string >* pv );	

	bool GoFirstNode( const char* name );

	bool GoNextNode( const char* name );

	bool GetChildNameValue( std::string& name, std::string& val, bool first = true );	

	bool Test( void );

private:
	
	struct Docnode
	{
		std::string Namenode;
		std::string Value;
		Element* Petalon;
		std::vector< Docnode > ChildNode ;		

		void ClearAllChild( std::vector< Docnode >& p )
		{
			for ( size_t i = 0; i < p.size(); i ++ )
			{
				if ( !p.at( i ).ChildNode.empty() )
					ClearAllChild( p.at( i ).ChildNode );
			}
			p.clear();
		}
		Docnode& operator = ( const Docnode& d )
		{
			if ( this != &d )
			{
				if ( !ChildNode.empty() )
					ClearAllChild( ChildNode );
				Namenode.assign( d.Namenode );
				Value.assign( d.Value );
				Petalon = d.Petalon;
				ChildNode = d.ChildNode;
			}
			return *this;
		}
	};

	Docnode _currnode;
	std::vector< Docnode > _structuredxml;
	Docnode* _currparent;
	Docnode* _lastinsertnode; //указатель на последний добавленный элемент
	int _numcurrpos;// номер текущей позиции при переборе элементов вектора
	int _numcurrnode; //указатель на текущий узел для GoFirstNode GoNextNode

	
	void FillCurrDocnode( const char* name, const char* val );

	void InsertNode( Docnode* parent, Docnode* toinsert );

	void InsertNode( Docnode* toinsert );
	
	int OffsetFromParent( const char* name, const Element* pparent ) const;

	int OffsetFromParent( Tokens t, const Element* pparent ) const;

	Element* ElementFromParent( const char* name, const Element* pparent ) const;

	Docnode* FindParentOnTok( Tokens t );

	Docnode* FindNodeOnTok( std::vector< Docnode >* p, Tokens t );	

	void AllDocToVector( std::vector< Docnode >* doc, std::vector< std::string >* pv );

	bool RequiredElementPresent( void );

	bool RequiredValuePresent( void );

	bool VariableControl( void );

	bool TagExtractControl( void );
};

static DocInstaller currdoc;

#endif //_CMDINST_DOCINSTALLER_H
