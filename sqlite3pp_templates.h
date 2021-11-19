

#ifndef SQLITE3PP_TEMPLATES_H
#define SQLITE3PP_TEMPLATES_H
#include "sqlite3pp.h"
#include <vector>
#include <string>

namespace sqlite3pp
{
	std::string to_string( const wchar_t* src );
	std::wstring to_wstring( const char* src );
	std::string to_string( const std::wstring &src );
	std::wstring to_wstring( const std::string &src );
	void setGlobalDB( const std::string& db_filename );
	void setGlobalDB( const std::wstring& db_filename );
	int Execute( const std::string& sql );
	int Execute( const std::wstring& sql );
	int Connect( char const* dbname, int flags, const char* vfs = nullptr );
	int Connect( wchar_t const* dbname, int flags, const wchar_t* vfs = nullptr );
	int Attach( char const* dbname, char const* name );
	int Attach( wchar_t const* dbname, wchar_t const* name );
	int Detach( char const* name );
	int Detach( wchar_t const* name );
	int Backup( char const* dbname, database& destdb, char const* destdbname, database::backup_handler h, int step_page = 5 );
	int Backup( wchar_t const* dbname, database& destdb, wchar_t const* destdbname, database::backup_handler h, int step_page = 5 );
	std::string GetDbErrMsg();
	std::wstring GetDbErrMsgW();
	int GetDbErr();
	int GetDbExtErr();


	class db_gbl
	{
	public:
		friend void setGlobalDB( const std::string& db_filename );
		friend void setGlobalDB( const std::wstring& db_filename );
		friend int Execute( const std::string& sql );
		friend int Execute( const std::wstring& sql );
		friend int Connect( char const* dbname, int flags, const char* vfs );
		friend int Connect( wchar_t const* dbname, int flags, const wchar_t* vfs );
		friend int Attach( char const* dbname, char const* name );
		friend int Attach( wchar_t const* dbname, wchar_t const* name );
		friend int Detach( char const* name );
		friend int Detach( wchar_t const* name );
		friend int Backup( char const* dbname, database& destdb, char const* destdbname, database::backup_handler h, int step_page);
		friend int Backup( wchar_t const* dbname, database& destdb, wchar_t const* destdbname, database::backup_handler h, int step_page );
		friend std::string GetDbErrMsg();
		friend std::wstring GetDbErrMsgW();
		friend int GetDbErr();
		friend int GetDbExtErr();
	protected:
		static sqlite3pp::database global_db; // To be used as global DB
	};

	template <class T, class T2 = tstring, class BT = T>  // Having ability to change BT to a base class allows for merging queries
	class Table : public db_gbl
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
		Table( sqlite3pp::database &db, T2 WhereClause = T2() ) :m_db( db ) { PrepareQuery( m_db, WhereClause ); }
		Table( sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table( const VectType &VectTypes ) :m_db( global_db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		const VectType& Get() const { return m_VectType; }
		void Insert( const VectType& Data )
		{
			for ( auto d : Data )
				m_db.execute( _T( "INSERT INTO " ) + T::GetTableName() + _T( " (" ) + T::GetSelectNames() + _T( ") VALUES (" ) + d.GetValues() + _T( ")" ) );
		}

		auto begin() { return m_VectType.begin(); }
		auto end() { return m_VectType.end(); }
		size_t size() const { return m_VectType.size(); }
		const DataType& operator[]( int i ) { return m_VectType[i]; }
		void push_back( const DataType &datatype ) { return m_VectType.push_back( datatype ); }
		void append( const VectType &vecttype ) { return m_VectType.push_back( vecttype ); }
	};

} // namespace sqlite3pp

#endif //SQLITE3PP_TEMPLATES_H
