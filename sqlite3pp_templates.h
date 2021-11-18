

#ifndef SQLITE3PP_TEMPLATES_H
#define SQLITE3PP_TEMPLATES_H
#include "sqlite3pp.h"
#include <vector>

namespace sqlite3pp
{
  class Table_Base
  {
  public:
	static void setGlobalDB( database &db_ );
  protected:
	static database db; // To be used as global DB
	sqlite3pp::database &m_db;
  };

  template <class T, class BT = T, class T2 = tstring>  // Having ability to change BT to a base class allows for merging queries
  class Table : public Table_Base
  {
	  using DataType = BT;
	  using VectType = std::vector<DataType>;
	  VectType m_VectType;
	  void PopulateVect( sqlite3pp::database &db, sqlite3pp::query &qry )
	  {
		  for ( auto q : qry )
		  {
			  DataType temp_var;
			  temp_var.GetStreamData( q );
			  m_VectType.push_back( temp_var );
		  }
	  }
	  void PrepareQuery( sqlite3pp::database &db, std::string WhereClause )
	  {
		  sqlite3pp::query qry( m_db, ("SELECT " + T::GetSelectNames() + " FROM " + T::GetTableName() + " " + WhereClause).c_str() );
		  PopulateVect( db, qry );
	  }
	  void PrepareQuery( sqlite3pp::database &db, std::wstring WhereClause )
	  {
		  sqlite3pp::query qry( m_db, (L"SELECT " + T::GetSelectNames() + L" FROM " + T::GetTableName() + L" " + WhereClause).c_str() );
		  PopulateVect( db, qry );
	  }
  public:
	  Table( T2 WhereClause = T2() ) :Table_Base( Table_Base::db ) { PrepareQuery( m_db, WhereClause ); }
	  Table( sqlite3pp::database &db, T2 WhereClause = T2() ) :Table_Base( db ) {PrepareQuery( m_db, WhereClause );}
	  Table( sqlite3pp::database &db, const VectType &VectTypes ) :Table_Base( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
	  Table( const VectType &VectTypes ) :Table_Base( Table_Base::db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
	  const VectType& Get() const { return m_VectType; }
	  void Insert( const VectType& Data )
	  {
		  for ( auto d : Data )
			  m_db.execute( _T("INSERT INTO ") + T::GetTableName() + _T(" (") + T::GetSelectNames() + _T(") VALUES (") + d.GetValues() + _T(")") );
	  }

	  auto begin() { return m_VectType.begin(); }
	  auto end() { return m_VectType.end(); }
	  size_t size() const { return m_VectType.size(); }
	  const DataType& operator[]( int i ) { return m_VectType[i]; }
	  void push_back( const DataType &datatype ) { return m_VectType.push_back(datatype); }
	  void append( const VectType &vecttype ) { return m_VectType.push_back( vecttype ); }
  };

} // namespace sqlite3pp

#endif //SQLITE3PP_TEMPLATES_H
