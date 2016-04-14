// ************************************************************************ 
//   DocInstaller.cpp - v 0.4
// ************************************************************************ 

#include "StdAfx.h"
#include "DocInstaller.h"
#include "structure.h"

#include <vector>
#include <iostream>
#include <string>



DocInstaller::DocInstaller(void)
{
	_currparent = 0;	
	_currnode.Petalon = 0;
}


DocInstaller::~DocInstaller(void)
{
	DeleteAllVector();
}

void DocInstaller::AddElement( const char* name, const char* val = 0 )
{
	FillCurrDocnode( name, val );
	InsertNode( _currparent, &_currnode );
}

void DocInstaller::AddElement( Tokens t, const char* name, const char* val )
{
	Docnode* p = FindParentOnTok( t );
	_currparent = p;
	AddElement( name, val );
}


void DocInstaller::AddComment( const char* name, const char* val )
{
	if( !_currnode.ChildNode.empty() )
		_currnode.ClearAllChild( _currnode.ChildNode );

	Element* tmp = ElementFromParent( "!--", 0 );

	_currnode.Petalon = tmp;		

	_currnode.Namenode.assign( tmp->NameOpenTag );

	if ( 0 != val )
		_currnode.Value.assign( val );
	else
		_currnode.Value.clear();	
	
	if ( !_currparent )
		_structuredxml.push_back( _currnode );
	else
		_currparent->ChildNode.push_back( _currnode );
}


void DocInstaller::DelElement( const char* name, const char* val )
// поиск удаляемого элемента производится относительно _currparent
{
	size_t i;
	
	for( i = 0; i < _currparent->ChildNode.size(); i ++ )
	{
		if ( !strcmp( _currparent->ChildNode.at( i ).Namenode.c_str(), name ) && !strcmp( _currparent->ChildNode.at( i ).Value.c_str(), val ) )
			break;
	}
	
	if( i == _currparent->ChildNode.size() )
		return;

	Docnode* p = &_currparent->ChildNode.at( i );
	p->ClearAllChild( p->ChildNode );
	_currparent->ChildNode.erase( _currparent->ChildNode.begin() + i );
}


void DocInstaller::Movecurrparent( Tokens t )
// устанавливает новое значение _currparent в соответсвии с Tokens
{
	_currparent = FindParentOnTok( t );
}


// _currparent присвоить последнему добавленному элементу
void DocInstaller::Movecurrparent( void )
{
	_currparent = _lastinsertnode;
}


int DocInstaller::GetValueFromParent( int pos, const char* name, std::string& val )
// поиск тега name в векторе ChildNode начиная с позиции pos
// возвращаемое значнеие int индекс найденного элемента в векторе если не найден то -1
{
	std::vector< Docnode >* pv;
	
	if ( _currparent )
		pv = &_currparent -> ChildNode;
	else
		pv = &_structuredxml;

	for( size_t i = pos; i < pv -> size(); i ++ )
	{
		if ( 0 == strcmp( name, pv->at( i ).Namenode.c_str() ) )
		{
			val = pv->at( i ).Value;
			return i;
		}
	}
	return -1;
}


void DocInstaller::AllDocToVector( std::vector< std::string >* pv )
//записывает весь xml в vector< string >
{
	AllDocToVector( &_structuredxml, pv );
}


void DocInstaller::DeleteAllVector( void )
// clear all vector
{	
	for ( size_t i = 0; i < _structuredxml.size(); i ++ )
		_structuredxml.at( i ).ClearAllChild( _structuredxml.at( i ).ChildNode );
	
	_structuredxml.clear();
}

bool DocInstaller::Test( void )
// проверка соответствия документа структуре Etalon
{
	/*if( !TagExtractControl() )
		return false;*/

	if( !VariableControl() )
		return false;

	if( RequiredElementPresent( ) && RequiredValuePresent() )
	{
		std::cout << "\nStructure current document correct\n";
		return true;
	}
	else
	{
		std::cout << "\nStructure current document not correct\n";
		return false;
	}

}


void DocInstaller::FillCurrDocnode( const char* name, const char* val = 0 )
{
	Element* tmp;
	if( !_currnode.ChildNode.empty() )
		_currnode.ClearAllChild( _currnode.ChildNode );
	
	if( !_currparent )
		tmp = ElementFromParent( name, 0 );
	else
		tmp = ElementFromParent( name, _currparent->Petalon );
	if ( !tmp )
	{
		std::cout << "Element " << name << " not defined in current block\n";
		return;
	}

	if ( !val &&  YES == tmp->Param )
	{
		std::cout << " Value for element " << name << " absent\n";
		return;
	}

	_currnode.Namenode.assign( name );
	
	if ( 0 != val )
		_currnode.Value.assign( val );
	else
		_currnode.Value.clear();
	_currnode.Petalon = tmp;
}

void DocInstaller::InsertNode( Docnode* parent, Docnode* toinsert )
{	
	int i;
	if ( !parent )
		i = OffsetFromParent( toinsert->Petalon->Tn, 0 );
	else
		i = OffsetFromParent( toinsert->Petalon->Tn, parent->Petalon );
	
	if ( -1 == i )
	{
		std::cout << "element " << _currnode.Namenode << " not addition\n";
		return;
	}

	std::vector< Docnode >* t;
	
	if ( !parent )
		t = &_structuredxml;
	else
		t = &( parent->ChildNode );

	if ( t->empty() )
	{
		t->push_back( *toinsert );
		_lastinsertnode = &t->back();
	}

	else
	{		
		for ( size_t j = 0; j < t->size(); j ++ )
		{			
			if( ( t->at( j ).Petalon->Tn == TOKCOMMENT ) )
				continue;
			int k;
			if ( !parent )				
				k = OffsetFromParent( t->at( j ).Petalon->Tn, 0 );
			else				
				k = OffsetFromParent( t->at( j ).Petalon->Tn, parent->Petalon );
			
			if ( i < k && -1 != k )
			{
				if( 1 == _currnode.Petalon->Repetition )
				{
					for( size_t i = 0; i < t->size(); i ++ )
					{
						if( t->at( i ).Petalon->Tn == _currnode.Petalon->Tn )
						{
							std::vector< Docnode > tmpchild;
							if( !t->at( i ).ChildNode.empty() )
							{
								tmpchild = t->at( i ).ChildNode;
								t->at( i ).ChildNode.clear();
							}
							t->at( i ) = _currnode;
							t->at( i ).ChildNode = tmpchild;							
							return;
						}
					}
				}				
				t->push_back( _currnode );
				_lastinsertnode = &t->back();
				return;
			}

 			if ( i == k && toinsert->Petalon->Repetition == 1 )
			{
				std::vector< Docnode > tmpchild;					
				for( size_t i = 0; i < t->size(); i ++ )
				{
					if( t->at( i ).Petalon->Tn == _currnode.Petalon->Tn )
					{
						std::vector< Docnode > tmpchild;
						if( !t->at( i ).ChildNode.empty() )
						{
							tmpchild = t->at( i ).ChildNode;
							t->at( i ).ChildNode.clear();
						}
						t->at( i ) = _currnode;
						t->at( i ).ChildNode = tmpchild;							
						return;
					}
				}				
				t->push_back( _currnode );
				_lastinsertnode = &t->back();
				return;
			}

			if ( i == k && toinsert->Petalon->Repetition != 1 )
			{				
				t->push_back( _currnode );
				_lastinsertnode = &t->back();
				return;
			}
		}
		t->push_back( _currnode );
		_lastinsertnode = &t->back();
	}
	return;
}


void DocInstaller::InsertNode( Docnode* toinsert )
{
	InsertNode( _currparent, toinsert );
}


// -1 если pparent не является родительским для name
int DocInstaller::OffsetFromParent( const char* name, const Element* pparent ) const
{	
	int j = 0;
	for( int i = 0; i < EtalonSz; i ++ )
	{
		if ( 0 == strcmp( name, Etalon[ i ].NameOpenTag ) && pparent == Etalon[ i ].BaseElement )
			return j;
		if ( pparent == Etalon[ i ].BaseElement )
			j ++;
	}
	return -1;
}


// -1 если pparent не является родительским для name
int DocInstaller::OffsetFromParent( Tokens t, const Element* pparent ) const
{	
	int j = 0;
	Element* e;

	if( !pparent )
		return j;

	e = FindElement( t );
	if( e )
		e = e->BaseElement;
	
	if( !e )
		return j;
	
	j ++;
	while( e != pparent && 0 != e )
	{
		e = e->BaseElement;
		j ++;
	}

	if( e == pparent )
		return j;
	else
		return -1;
}


Element* DocInstaller::ElementFromParent( const char* name, const Element* pparent ) const
{	
	int j = 0;
	for( int i = 0; i < EtalonSz; i ++ )
	{
		if ( 0 == strcmp( name, Etalon[ i ].NameOpenTag ) && pparent == Etalon[ i ].BaseElement )
			return &Etalon[ i ];		
	}
	return 0;
}


DocInstaller::Docnode* DocInstaller::FindParentOnTok( Tokens t )
{
	Docnode* c;
	Element* e = FindElement( t );
	if ( !e )
	{
		std::cout << "Token " << t << " not foud\n";
		return 0;
	}	
	
	if ( !e->BaseElement )
		return 0;

	c = FindNodeOnTok( &_structuredxml, e->BaseElement->Tn );

	return c;	
}


DocInstaller::Docnode* DocInstaller::FindNodeOnTok( std::vector< Docnode >* p, Tokens t )
{
	static Docnode* r = 0;	

	for( size_t i = 0; i < p->size(); i ++ )
	{
		if ( p->at( i ).Petalon->Tn == t )
		{
			r = &p->at( i );
			return r;
		}
		else
			r = 0;

		FindNodeOnTok( &p->at( i ).ChildNode, t ) ;
		if( r )
			break;
	}

	return r;
}


void DocInstaller::AllDocToVector( std::vector< Docnode >* doc, std::vector< std::string >* pv )
// текущий документ в vector
// к value добавляем кавычки
{
	for( size_t i = 0; i < doc->size(); i ++ )
	{
		if ( doc->at( i ).Petalon->Tn == TOKCOMMENT )
			pv->push_back( "<!--" );
		else
			pv->push_back( "<" + doc->at( i ).Namenode + ">" + "\n" );

		if( !doc->at( i ).Value.empty() )
			pv->push_back( "\"" + doc->at( i ).Value + "\"\n" );
		
		AllDocToVector( &doc->at( i ).ChildNode, pv );
		
		if ( doc->at( i ).Petalon->Tn == TOKCOMMENT )
			pv->push_back( "-->\n" );
		else
			pv->push_back( "</" + doc->at( i ).Namenode + ">" + "\n" );
	}
}


bool DocInstaller::GoFirstNode( const char* name )
{
	std::vector< Docnode >* pv;
	
	_numcurrnode = 0;

	if( !_currparent )
		pv = &_structuredxml;
	else
		pv = &_currparent->ChildNode;

	for( int i = 0; i < pv->size(); i ++ )
	{
		if( 0 == strcmp( pv->at( i ).Namenode.c_str(), name ) )
		{
			_numcurrnode = i;
			return true;
		}
	}

	return false;
}


bool DocInstaller::GoNextNode( const char* name )
{
	std::vector< Docnode >* pv;
	
	if( !_currparent )
		pv = &_structuredxml;
	else
		pv = &_currparent->ChildNode;

	for( int i = _numcurrnode + 1; i < pv->size(); i ++ )
	{
		if( 0 == strcmp( pv->at( i ).Namenode.c_str(), name ) )
		{
			_numcurrnode = i;
			return true;
		}
	}

	return false;
}

bool DocInstaller::GetChildNameValue( std::string& name, std::string& val, bool first )
// возвращает последовательно имена и значения всех дочерних элементов узла
{
	std::vector< Docnode >* pv;
	
	if( !_currparent )
		pv = &_structuredxml.at( _numcurrnode ).ChildNode;
	else
		pv = &_currparent->ChildNode.at( _numcurrnode ).ChildNode;

	if( !pv )
		return false;

	if( first )
		_numcurrpos = 0;

	
	if( _numcurrpos < pv->size() )
	{
		name = pv->at( _numcurrpos ).Namenode;
		val = pv->at( _numcurrpos ).Value;
		_numcurrpos++;
		return true;
	}
	else
		return false;	
}


bool DocInstaller::RequiredElementPresent( void )
// проверка наличия обязательных элеменов в документе
// true если все обязательные элаементы присутствуют
{
	bool retcode = true;
	for( int i = 0; i < EtalonSz; i ++ )
	{
		if( YES == Etalon[ i ].Melem )
		{
			Docnode* p = FindNodeOnTok( &_structuredxml, Etalon[ i ].Tn );
			if ( !p )			
			{
				std::cout << "Required element " << Etalon[ i ].NameOpenTag << " absent\n";
				retcode = false;
			}
		}
	}
	return retcode;
}

bool DocInstaller::RequiredValuePresent( void )
//проверка наличия value в элементах где значение должно быть обязательно
// true если занчение у всех элементов, где оно является обязательным присутсвует
{
	bool retcode = true;
	for( int i = 0; i < EtalonSz; i ++ )
	{
		if( YES == Etalon[ i ].Param )
		{
			Docnode* p = FindNodeOnTok( &_structuredxml, Etalon[ i ].Tn );
			if ( p )			
			{
				if ( p->Value.empty() && YES == p->Petalon->Param )
				{
					std::cout << "Required value " << Etalon[ i ].NameOpenTag << " absent\n";
					retcode = false;
				}
			}
		}
	}

	return retcode;
}


//Проверка, являются ли значения переменных допустимыми
bool DocInstaller::VariableControl( void )
{
	bool rt = true;

	for( int i = 0; i < DefVariableArrSz; i ++ )
	{
		Docnode *p = FindNodeOnTok( &_structuredxml, DefVariableArr[ i ].Tn );
		if( p && !p -> Value.empty() )
		{
			if ( !VariableValControl( DefVariableArr[ i ].Tn, p -> Value.c_str() ) )
			{
				std::cout << "Invalid variable value: " << p->Namenode << '\n';
				rt = false;
			}
		}
	}
	return rt;
}


bool DocInstaller::TagExtractControl( void )
// если в installer нет элемента extract и в каком либо file нет элемента extract возвращает false
{
	bool retcode = true;
	Docnode* t = FindNodeOnTok( &_structuredxml, TOKINSTEXTRACT );

	for( int i = 0; i < _structuredxml.size(); i ++ )
	{
		if( TOKFILE == _structuredxml.at( i ).Petalon->Tn )
		{
			Docnode* c = FindNodeOnTok( &_structuredxml.at( i ).ChildNode, TOKFILEXTRACT );
			if( 0 == c && 0 == t )
			{
				Docnode* j = FindNodeOnTok( &_structuredxml.at( i ).ChildNode, TOKFILNAME );

				if( !j )
				{
					std::cout << "Required element <name>" << " absent\n";
					retcode = false;
				}
				else
				{
					std::cout << "Required element <extract> absent in <file><name>" << j->Value << std::endl;
					retcode = false;
				}
			}
		}
	}

	return retcode;
}