

#ifndef SQLITE3PP_TEMPLATES_H
#define SQLITE3PP_TEMPLATES_H
#include "sqlite3pp.h"
#include <vector>
#include <string>

namespace sqlite3pp
{
  class Table_Base
  {
  public:
	  static void setGlobalDB( const std::string& db_filename );
	  static void setGlobalDB( const std::wstring& db_filename );
  protected:
	static sqlite3pp::database global_db; // To be used as global DB
  };

  template <class T, class T2 = tstring, class BT = T>  // Having ability to change BT to a base class allows for merging queries
  class Table : public Table_Base
  {
	  using DataType = BT;
	  using VectType = std::vector<DataType>;
	  VectType m_VectType;
	  sqlite3pp::database &m_db;
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
	  Table( T2 WhereClause = T2() ) :m_db( global_db ) { PrepareQuery( m_db, WhereClause ); }
	  Table( sqlite3pp::database &db, T2 WhereClause = T2() ) :m_db( db ) {PrepareQuery( m_db, WhereClause );}
	  Table( sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
	  Table( const VectType &VectTypes ) :m_db( global_db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
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
