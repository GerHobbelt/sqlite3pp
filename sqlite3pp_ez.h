/*	sqlite3pp_ez.h
	GNU General Public License
	
	Copyright (C) 2021 David Maisonave (www.axter.com)
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This source code is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

///////////////////////////////////////////////////////////////////////////
// Summary: 
///////////////////////////////////////////////////////////////////////////

/*
	Usage Instructions:

*/

#ifndef SQLITE3PP_TEMPLATES_H
#define SQLITE3PP_TEMPLATES_H
#include "sqlite3pp.h"
#include <vector>
#include <string>

namespace sqlite3pp
{
	class db_gbl
	{
	public:
		friend void setGlobalDB( const std::string& db_filename );
		friend void setGlobalDB( const std::wstring& db_filename );
		friend database& getGlobalDB();
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

	template <class T, class BT = T>  // Having ability to change BT to a base class allows for merging queries (like a union, but faster)
	class Table : public db_gbl
	{
		using T_STR = typename T::Str_DataType;
		using DataType = BT;
		using VectType = std::vector<DataType>;
		VectType m_VectType;
		sqlite3pp::database &m_db;
		void PopulateVect( sqlite3pp::database &db, sqlite3pp::query &qry )
		{
			for ( auto q : qry )
			{
				DataType temp_var;
				temp_var.getStreamData( q );
				m_VectType.push_back( temp_var );
			}
		}
		void PrepareQuery( sqlite3pp::database &db, std::string WhereClause )
		{
			const std::string strQuery = "SELECT " + T::getColumnNames() + " FROM " + T::getTableName() + " " + WhereClause;
			sqlite3pp::query qry( m_db, strQuery.c_str() );
			PopulateVect( db, qry );
		}
		void PrepareQuery( sqlite3pp::database &db, std::wstring WhereClause )
		{
			const std::wstring strQuery = L"SELECT " + T::getColumnNames() + L" FROM " + T::getTableName() + L" " + WhereClause;
			sqlite3pp::query qry( m_db, strQuery.c_str() );
			PopulateVect( db, qry );
		}
	public:
		Table( T_STR WhereClause = T_STR() ) :m_db( global_db ) { PrepareQuery( m_db, WhereClause ); }
		Table( sqlite3pp::database &db, T_STR WhereClause = T_STR() ) :m_db( db ) { PrepareQuery( m_db, WhereClause ); }
		Table( sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table( const VectType &VectTypes ) :m_db( global_db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		const VectType& Get() const { return m_VectType; }
		void Insert( const VectType& Data )
		{
			for ( auto d : Data )
				m_db.execute( _T( "INSERT INTO " ) + T::getTableName() + _T( " (" ) + T::getColumnNames() + _T( ") VALUES (" ) + d.GetValues() + _T( ")" ) );
		}

		auto begin() { return m_VectType.begin(); }
		auto end() { return m_VectType.end(); }
		size_t size() const { return m_VectType.size(); }
		const DataType& operator[]( int i ) { return m_VectType[i]; }
		void push_back( const DataType &datatype ) { return m_VectType.push_back( datatype ); }
		void append( const VectType &vecttype ) { return m_VectType.push_back( vecttype ); }
	};

	std::string to_string( const wchar_t* src );
	std::wstring to_wstring( const char* src );
	std::string to_string( const std::wstring &src );
	std::wstring to_wstring( const std::string &src );
	void setGlobalDB( const std::string& db_filename );
	void setGlobalDB( const std::wstring& db_filename );
	database& getGlobalDB();
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

	// TblClassOptions can be ignored when using the default settings, which can work with sqlite3pp::Table
	struct TblClassOptions
	{
		std::string str_type;			// "std::string", "std::wstring", "sqlite3pp::tstring"
		std::string str_pre;			//  "", "L", "_T("
		std::string str_post;			//  "", "", ")"
		std::string str_include;		//  include needed to support str_type. ie:("#include <string>", "#include \"sqlite3pp_ez.h\"", "#include <string>\n#include <tchar.h>")
		std::string dest_folder;		// Can be empty, but if folder is specified, it must end with "\\".  Otherwise it will be treated as part of the file name.
		std::string delimiter;			// Only used with opereator<<, and can be and desired string value to mark seperation between field output. ie:  ",", ", ", " ", ";", ""
		std::string header_prefix;		// Can be empty, or can specify a desired prefix for headers created.
		std::string header_postfix;		// Can be empty, or can specify a desired postfix for header created.
		bool is_public_var_members;		// True to make data members public, and false to make data members protected.
		bool exclude_get_functions;		// If true, no get function. If false, a get function is created for each data member variable.
		bool exclude_set_functions;		// If true, no set function. If false, a set function is created for each data member variable.
		bool exclude_ostream_operator;	// If true, no operator<<. If false, a creates friend declaration, getDelimiter function, and global operator<< for the class
		bool exclude_comments;			// If true, excludes comments and additional spaces.
	}; // Create a custom defined TblClassOptions variable, or used one of the following predefined type, or use the default type which is automatically set by the SQLiteClassBuilder constructor

	// Use SQLiteClassBuilder to create C++ header files for tables or views
	class SQLiteClassBuilder
	{
		sqlite3pp::database m_db;
		TblClassOptions m_strtype;
		bool m_AppendTableToHeader;
		std::string GetType(const char* str);
		static bool dir_exists(const std::string& foldername);
	public:
		SQLiteClassBuilder(const std::string& Db_filename					// Only Required Field
			, const TblClassOptions &strtype = std_string_protected_members	// The default option is commonly used. This argument is used to define options in creating headers and content.  Can use a custom defined TblClassOptions, or one of the 8 predefined types (std_string_protected_members, std_wstring_protected_members, std_tstring_protected_members, sql_tstring_protected_members,std_string_minimal, std_wstring_minimal, std_tstring_minimal, sql_tstring_minimal).
			, const std::string& TableOrView_name = Nill					// If equal to "#NILL#", no header file is created. If empty, a header for each table and view is created. If equal to table or view name, it will create single header for associated table or view.
			, const std::string &PostFixWhereClause = ""					// Used when creating multiple tables.  Can specify which tables/views to include.
		);
		~SQLiteClassBuilder();
		bool CreateAllHeaders(const std::string &PostFixWhereClause = "");
		bool CreateAllHeaders(const TblClassOptions &strtype, const std::string &PostFixWhereClause = "");
		bool CreateHeader(const std::string& ClassName, std::string QueryStr = "");

		// Create a class having protected member variables, and a get_* and set_* function for each column in the table/view
		static const TblClassOptions std_string_protected_members;
		static const TblClassOptions std_wstring_protected_members;
		static const TblClassOptions std_tstring_protected_members;
		static const TblClassOptions sql_tstring_protected_members;
		// Create minimalist (bare-bone) class having public member variables which can be fetch or set directly
		static const TblClassOptions std_string_minimal;
		static const TblClassOptions std_wstring_minimal;
		static const TblClassOptions std_tstring_minimal;
		static const TblClassOptions sql_tstring_minimal;

		static const char *Nill; // = "#NILL#"
	};

} // namespace sqlite3pp

#endif //SQLITE3PP_TEMPLATES_H
