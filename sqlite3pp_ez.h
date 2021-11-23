/*	sqlite3pp_ez.h
	GNU General Public License
	
	Copyright (C) 2021 David Maisonave (www.axter.com)
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This source code is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/
///////////////////////////////////////////////////////////////////////////
// Summary: sqlite3pp_EZ extends sqlite3pp by adding the following:
//			1. Unicode support.
//				Example:
//					sqlite3pp::setGlobalDB(L"Exchange_€To$_database.db");
//					sqlite3pp::Execute(_T("INSERT OR REPLACE INTO FileName VALUES ('") + sfileName + _T("', '") + sfileExt + _T("');"));
//					SQLiteClassBuilder	createMyClasses(
//							"Exchange_€To$_database.db"									// Use UTF8 to open file.
//							, SQLiteClassBuilder::std_wstring_protected_members			// This option creates a class with std::wstring as the default string, and protected member variables.
//							, ""														// Set this to a specific table or view, in which one class is created, or set it to empty to create a class for each table and view in the database.
//						);
//			2. Generic template Table class having the following features:
//				a. Type safety for table column fields; 
//					Table column of type INTEGER has variable member of type int
//					Table column of type TEXT has variable member of type Table::T_STR, which is alias for the type of string defined by the class (std:string, std::wstring, sqlite3pp::tstring, etc...)
//					Table column of type REAL has variable member of type double
//					Table column of type FLOAT has variable member of type float
//					Table column of type BOOLEAN has variable member of type bool
//					Table column of type TINYINT has variable member of type byte
//					Table column of type BIGINT has variable member of type __int64
//					Table column of type UNSIGNED BIG INT has variable member of type unsigned __int64
//					Table column of type DATE and DATETIME has variable member of type time_t
//					Table column of type DOUBLE, DECIMAL, or NUMERIC, have variable member of type double
//				b. Automatically populate the Table class with associated table
//					Example:
//						sqlite3pp::Table<sql_table_MyTableFoo> tbl;  // One line create and populates tbl with all content of table named MyTableFoo.
//				c. Can iterate each row using (C++11) Range-based loop, C+ stye iteration, or C style iteration
//					Example:
//							sqlite3pp::Table<sql_table_MyTableFoo> tbl;
//
//							for ( auto row : tbl )											// (C++11) Range-based loop
//								std::cout << row.get_Wigets() << row.get_MyColumn() << row.get_AnotherFooColumn() << std::endl;
//
//							for (auto row = tbl.begin(); row != tbl.end(); ++row)			// C++ style iteration
//								std::cout << row->get_Wigets() << row->get_MyColumn() << row->get_AnotherFooColumn() << std::endl;
//
//							for (int row = 0; row < tbl.size(); ++row)						// C style iteration
//								std::cout << tbl[row].get_Wigets() << tbl[row].get_MyColumn() << tbl[row].get_AnotherFooColumn() << std::endl;
//				d. All sqlite3pp::Table objects can optionally share the same sqlite3pp::database, so the sqlite3pp::Table constructor doesn't have to take sqlite3pp::database input argument
//			3. A SQLiteClassBuilder class which can be used to create a class for each table or view in a SQLite database.
//				a. The class created by SQLiteClassBuilder is type safe IAW the column defined type.
//				b. The created class can be used with the template Table class. Example:  sqlite3pp::Table<MyBuilderCreatedClass> tbl;
//			4. For most common requirements, the default settings can be used. Unless otherwise specified, SQLiteClassBuilder uses predefined setting (std_string_protected_members) as the default settings.
//			5. For advanced developers/usage:
//				a. When creating a class, SQLiteClassBuilder has the following options:
//					(1). Set created class to have a specific default string type (std:string, std::wstring, sqlite3pp::tstring, etc...)
//					(2). Specifiy a subfolder in which to create headers for each class
//					(3). Specify a prefix and/or a postfix for the header file name.
//					(4). Specify if column associated member variables are public or protected.
//					(5). Specify if class has a get_* function for each column associated member variable.
//					(6). Specify if class has a set_* function for each column associated member variable.
//					(7). Specify if class gets an associated ostream (operator<<) function.
//					(8). Specify if class gets created with comments
//				b. Developers can create a custom set of settings with class TblClassOptions, or use one of the 8 predefined settings.
//				c. There are 8 predefined settings for common choices.  The following are just 3 of the 8.
//					(1). std_string_protected_members (default)	= Creates a class that has member variables as protected, and it has get_* and set_* functions for each column associated variable. The default string type is std::string. String literals are define as-is. Example: foo = "some foo";
//					(2). sql_tstring_minimal					= Creates a minimal class, having no comments, get_* functions, and no set_* functions. Member variables are public and the default string type is sqlite3pp::tstring. String literals are wrap with _T() macro. Example: foo = _T("some foo");
//					(2). std_wstring_protected_members			= Creates protected member variables class with get_* and set_* functions. The default string type is std::wstring. String literals are prefixed with L. Example: foo = L"some foo";
//			6. There are API's for using a single global sqlite3pp::database.  These API's are not associated with a class, and can be called directly. It
//				. setGlobalDB(db_filename)
//				. Execute(SQL_statment)
//				. Connect(dbname, flags, vfs)
//				. Attach(dbname,dbname)
//				. Detach(dbname)
//				. Backup(dbname, destdb,destdbname, backup_handler, step_page)
//				. GetDbErrMsg()
//				. GetDbErrMsgW()
//				. GetDbErrNo()
//				. GetDbExtErrNo()
//				. getGlobalDB()
//
///////////////////////////////////////////////////////////////////////////

/*
	Usage Instructions:

*/

#ifndef SQLITE3PP_EZ_H
#define SQLITE3PP_EZ_H
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

	template <class T_STR, char const *TypeName>
	class TableArg
	{
		const T_STR m_Str;
	public:
		explicit TableArg(const T_STR& Str = T_STR()) :m_Str(Str) {}
		const T_STR& get_Str() { return m_Str; }
		char const * get_TypeName() { return TypeName; }
	};

	template <class T, class PARENT_TYPE = T>  // Having ability to change PARENT_TYPE to a base class allows for merging queries (like a union, but faster)
	class Table : public db_gbl
	{

	public:
		// Defined types to be used
		using T_STR = typename T::Str_DataType; // Allows Table class to work with different string types (std::string, std::wstring, sqlite3pp::tstring)
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
		explicit Table(PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery(  m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		explicit Table(InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		explicit Table(DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		explicit Table(sqlite3pp::database &db, PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		explicit Table(sqlite3pp::database &db, InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		explicit Table(sqlite3pp::database &db, DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		Table( sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table( const VectType &VectTypes ) :m_db( global_db ) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		explicit Table(WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg())
			:m_db( global_db ) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, insertarg, dbfilenamearg); }
		explicit Table( sqlite3pp::database &db, WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg()) 
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

#endif //SQLITE3PP_EZ_H
