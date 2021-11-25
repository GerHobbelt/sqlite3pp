/*	sqlite3pp_ez.h
	GNU General Public License
	
	Copyright (C) 2021 David Maisonave (www.axter.com)
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This source code is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	# Summary
	This repository is a fork of sqlite3pp, and it includes additional code to add UNICODE support, a template Table class and a SQLite class builder.
	The sqlite3pp::Table class along with the sqlite3pp::SQLiteClassBuilder class allows C++ developers to use type safe variables assocaited with the table column types.

	This package contains all the files required to use SQLite3, SQLite3pp, and SQLite3pp_EZ.Only minor modifications have been made to SQLite3 C code and SQLite3pp where needed for UNICODE support.Then bulk of the sqlite3pp_EZ implementation is in sqlite3pp_EZ.h and sqlite3pp_EZ.cpp.

	For usage examples see  https://github.com/David-Maisonave/sqlite3pp_EZ
							or sqlite3pp_ez.h
*/

#ifndef SQLITE3PP_EZ_H
#define SQLITE3PP_EZ_H
#include "sqlite3pp.h"
#include <vector>
#include <string>
#include <ostream>
#include <istream>

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
		friend int GetDbErrNo();
		friend int GetDbExtErrNo();


	protected:
		static sqlite3pp::database global_db; // To be used as global DB
		static const char TableArg_PreExecuteArg[];
		static const char TableArg_WhereClauseArg[];
		static const char TableArg_InsertArg[];
		static const char TableArg_DbFileNameArg[];
		static const char TableArg_ValueArg[];
	};

	template <class T_STR>
	struct StrTypes
	{
		// SQLite3 string types
		using Text		= T_STR;
	};

	

	template <class T_STR, char const *TypeName>
	class TableArg
	{
		const T_STR m_Str;
	public:
		TableArg(const T_STR& Str = T_STR()) :m_Str(Str) {}
		const T_STR& get_Str() { return m_Str; }
		char const * get_TypeName() { return TypeName; }
	};

	template <class T, class PARENT_TYPE = T>  // Having ability to change PARENT_TYPE to a base class allows for merging queries (like a union, but faster)
	class Table : public db_gbl
	{

	public:
		// Defined types to be used
		using T_STR = typename T::StrType; // Allows Table class to work with different string types (std::string, std::wstring, sqlite3pp::tstring)
		using DataType = PARENT_TYPE;
		using VectType = std::vector<DataType>;
		using PreExecuteArg = TableArg<T_STR, TableArg_PreExecuteArg>;
		using WhereClauseArg = TableArg<T_STR, TableArg_WhereClauseArg>;
		using InsertArg = TableArg<T_STR, TableArg_InsertArg>;
		using DbFileNameArg = TableArg<T_STR, TableArg_DbFileNameArg>;
		using ValueArg = TableArg<T_STR, TableArg_ValueArg>;

	private:
		// All member variables
		VectType m_VectType;
		sqlite3pp::database &m_db;

	public:
		// Constructors
		Table(PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery(  m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		Table(InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		Table(DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		Table(sqlite3pp::database &db, PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		Table(sqlite3pp::database &db, InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		Table(sqlite3pp::database &db, DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		Table( sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table( const VectType &VectTypes ) :m_db( global_db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table(WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg())
			:m_db( global_db ) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, insertarg, dbfilenamearg); }
		Table( sqlite3pp::database &db, WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg()) 
			:m_db( db ) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, insertarg, dbfilenamearg); }
		
		// Public methods
		const VectType& Get() const { return m_VectType; }
		void Insert( const VectType& Data ) { for (auto d : Data) Insert(ValueArg(d.GetValues()), T_STR()); }
		int Execute(const T_STR& strExecute){return m_db.execute(strExecute);}
		auto begin() { return m_VectType.begin(); }
		auto end() { return m_VectType.end(); }
		size_t size() const { return m_VectType.size(); }
		const DataType& operator[]( int i ) { return m_VectType[i]; }
		void push_back( const DataType &datatype ) { return m_VectType.push_back( datatype ); }
		void append( const VectType &vecttype ) { return m_VectType.push_back( vecttype ); }
		std::string CreateSelectQueryStr(WhereClauseArg whereclausearg, std::string) { return "SELECT " + T::getColumnNames() + " FROM " + T::getTableName() + " " + whereclausearg.get_Str(); }
		std::wstring CreateSelectQueryStr(WhereClauseArg whereclausearg, std::wstring) { return L"SELECT " + T::getColumnNames() + L" FROM " + T::getTableName() + L" " + whereclausearg.get_Str(); }

	protected:
		// Protected methods
		void Insert(const ValueArg& valuearg, std::string){	m_db.execute("INSERT INTO " + T::getTableName() + " (" + T::getColumnNames() + ") VALUES (" + valuearg.get_Str() + ")");}
		void Insert(const ValueArg& valuearg, std::wstring)	{m_db.execute(L"INSERT INTO " + T::getTableName() + L" (" + T::getColumnNames() + L") VALUES (" + valuearg.get_Str() + L")");}
		void PopulateVect(sqlite3pp::database &db, sqlite3pp::query &qry )
		{
			for ( auto q : qry )
			{
				DataType temp_var;
				temp_var.getStreamData( q );
				m_VectType.push_back( temp_var );
			}
		}
		void PrepareQuery(sqlite3pp::database &db, const T_STR& strQuery, PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg())
		{
			if (!dbfilenamearg.get_Str().empty())
				db = database(dbfilenamearg.get_Str().c_str());
			if (!preexecutearg.get_Str().empty())
				Execute(preexecutearg.get_Str());
			if (!insertarg.get_Str().empty())
				Execute(insertarg.get_Str());
			sqlite3pp::query qry(db, strQuery.c_str() );
			PopulateVect(db, qry );
		}
	};

	std::wostream& operator<<(std::wostream& os, const Character& t);
	std::ostream& operator<<(std::ostream& os, const Nchar& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Blob& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Blob& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Clob& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Clob& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Datetime& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Datetime& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Date& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Date& t);

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
	int GetDbErrNo();
	int GetDbExtErrNo();

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
	}; // Create a custom defined TblClassOptions variable, or used one of the SQLiteClassBuilder predefined types, or use the default type which is automatically set by the SQLiteClassBuilder constructor

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

#endif //SQLITE3PP_EZ_H
