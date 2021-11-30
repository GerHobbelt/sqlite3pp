/*	sqlite3pp_ez.h
	GNU General Public License
	
	Copyright (C) 2021 David Maisonave (www.axter.com)
	The sqlite3pp_ez source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
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
#include <iostream>

// For portability and to avoid conflicts, using T_() macro instead of windows _T().
#ifdef _UNICODE
#define T_(x)      L ## x
#else
#define T_(x)      x
#endif // _UNICODE


namespace sqlite3pp
{
	class sql_base
	{
	public:
		// SQLite3 types (Excluding string types)
		using Integer = int;
		using Int = int;
		using Int2 = int;
		using Int8 = int;
		using Tinyint = unsigned char;
		using Smallint = short int;
		using Mediumint = int;
		using Boolean = bool;
		using Bigint = long long int;
		using UBigint = unsigned long long int;
		using Numeric = double;
		using Decimal = double;
		using Real = double;
		using DoublePrcsn = double;
		using Double = double;
		using Float = double;
		using Blob = sqlite3pp::Blob;// Stores binary data
		using Clob = sqlite3pp::Clob;	// Stores strings that can have multiple NULL terminators
		using Date = sqlite3pp::Date;
		using Datetime = sqlite3pp::Datetime;
		using Nchar = std::wstring;
		using Nvarchar = std::wstring;
		using Character = std::string;
		using Varchar = std::string;

		static std::string to_string(const std::wstring &src);
		inline std::string to_string(const std::string &src) const { return src; } // For template usage
		static std::wstring to_wstring(const std::string &src);
		inline std::wstring to_wstring(const std::wstring &src) const { return src; } // For template usage
#ifdef _UNICODE
		static  std::wstring to_tstring(const std::string &src);
		inline  std::wstring to_tstring(const std::wstring &src) const { return src; } // For template usage
#else
		static std::string to_tstring(const std::wstring &src);
		inline std::string to_tstring(const std::string &src) const { return src; } // For template usage
#endif // _UNICODE

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
		static sqlite3pp::query* CreateQuery(database& db, const std::string& QueryStr);

	protected:
		static sqlite3pp::database global_db; // To be used as global DB
		static bool bIsGlblDbOpen; // To be used as global DB
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
		TableArg(const T_STR& Str = T_STR()) :m_Str(Str) {}
		const T_STR& get_Str() { return m_Str; }
		char const * get_TypeName() { return TypeName; }
	};

	class TableBase : public sql_base
	{
	public:
		virtual std::ostream& out(std::ostream& os) const = 0;
		virtual std::wostream& out(std::wostream& os) const = 0;
		virtual sqlite3pp::tstring GetTableName() const = 0;
		virtual sqlite3pp::tstring GetColumnNames() const = 0;
		virtual int GetColumnCount() const = 0;
	};

	template <class T, class PARENT_TYPE = T>  // Having ability to change PARENT_TYPE to a base class allows for merging queries (like a union, but faster)
	class Table : public TableBase
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
		const T_STR m_TableName;	// Mainly here for debugging purposes
		const T_STR m_ColumnNames;	// Mainly here for debugging purposes
		const int m_ColumnCount;	// Mainly here for debugging purposes
	public:
		// There are 2 constructor sets with each having 4 types of constructs. There are 4 types purely for the sake of convenience. Determine which constructors to use by which arguments are needed.
		// Set of constructors needing a sqlite3pp::database instance in constructor argument. These constructors automatically populate the object using data from the database db instance.
		Table(sqlite3pp::database &db, WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg()) 	:m_db( db ), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, insertarg, dbfilenamearg); }
		Table(sqlite3pp::database &db, PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		Table(sqlite3pp::database &db, InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		Table(sqlite3pp::database &db, DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		
		// Same as above set, but this set uses the single global database instance, and so db does not need to be pass to the constructor. These constructors automatically populate the object using data from the global database instance.
		Table(WhereClauseArg whereclausearg = WhereClauseArg(), PreExecuteArg preexecutearg = PreExecuteArg(), InsertArg insertarg = InsertArg(), DbFileNameArg dbfilenamearg = DbFileNameArg()):m_db( global_db ), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, insertarg, dbfilenamearg); }
		Table(PreExecuteArg preexecutearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery(  m_db, CreateSelectQueryStr(whereclausearg, T_STR()), preexecutearg, InsertArg(), DbFileNameArg()); }
		Table(InsertArg insertarg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), insertarg, DbFileNameArg()); }
		Table(DbFileNameArg dbfilenamearg, WhereClauseArg whereclausearg = WhereClauseArg()) :m_db(global_db), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { PrepareQuery( m_db, CreateSelectQueryStr(whereclausearg, T_STR()), PreExecuteArg(), InsertArg(), dbfilenamearg); }
		
		// Set of constructors which do NOT populate itself using the database.  Instead the constructors takes an argument which is used to automatically populate itself
		Table(sqlite3pp::database &db, const VectType &VectTypes ) :m_db( db ), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }
		Table( const VectType &VectTypes ) :m_db( global_db ), m_TableName(T::getTableName()), m_ColumnNames(T::getColumnNames()), m_ColumnCount(T::getColumnCount()) { for ( auto v : VectTypes )  m_VectType.push_back( v ); }

		// Public methods
		const VectType& Get() const { return m_VectType; }
		void Insert( bool DeleteAllBeforeInsert = false)
		{ 
			if (DeleteAllBeforeInsert)
				DeleteAll();  // ToDo: Add logic to have delete use where clause if the constructor received one
			for (auto d : m_VectType)
				Insert(ValueArg(d.GetValues()), T_STR()); 
		}
		void UpdateDb(bool DeleteAllBeforeUpdate = false)
		{
			if (DeleteAllBeforeUpdate)
				DeleteAll(); // ToDo: Add logic to have delete use where clause if the constructor received one
			for (auto d : m_VectType) 
				UpdateDb(ValueArg(d.GetValues()), T_STR());
		}
		int Execute(const T_STR& strExecute){return m_db.execute(strExecute);}
		void Insert(const DataType &d) { push_back(d); Insert(ValueArg(d.GetValues()), T_STR()); }
		void UpdateDb(const DataType &d) { UpdateDb(ValueArg(d.GetValues()), T_STR()); }
		void DeleteAll(){ DeleteAll(T_STR()); }
		auto begin() { return m_VectType.begin(); }
		auto end() { return m_VectType.end(); }
		size_t size() const { return m_VectType.size(); }
		const DataType& operator[]( int i ) { return m_VectType[i]; }
		void push_back( const DataType &datatype ) { return m_VectType.push_back( datatype ); }
		void append( const VectType &vecttype ) { return m_VectType.push_back( vecttype ); }
		std::string CreateSelectQueryStr(WhereClauseArg whereclausearg, std::string) { return "SELECT " + T::getSelecColumnNames() + " FROM \"" + T::getTableName() + "\" " + whereclausearg.get_Str(); }
		std::wstring CreateSelectQueryStr(WhereClauseArg whereclausearg, std::wstring) { return L"SELECT " + T::getSelecColumnNames() + L" FROM \"" + T::getTableName() + L"\" " + whereclausearg.get_Str(); }
		
		// Use this set of functions with a table instance
		virtual sqlite3pp::tstring GetTableName() const { return to_tstring(m_TableName); }
		virtual sqlite3pp::tstring GetColumnNames() const { return to_tstring(m_ColumnNames); }
		virtual int GetColumnCount() const { return m_ColumnCount; }
		
		// Use this set of functions when there's NO table instance
		static T_STR getTableName() { return T::getTableName(); }
		static T_STR getColumnNames() { return T::getColumnNames(); }
		static int getColumnCount() { return T::getColumnCount(); }


		virtual std::ostream& out(std::ostream& os) const
		{
			//for (auto d : m_VectType)
			//	os << d << std::endl;
			return os;
		}

		virtual std::wostream& out(std::wostream& os) const
		{
			//for (auto d : m_VectType)
			//	os << d << std::endl;
			return os;
		}

	protected:
		// Protected methods
		void Insert(const ValueArg& valuearg, std::string) { m_db.execute("INSERT INTO " + T::getTableName() + " (" + T::getColumnNames() + ") VALUES (" + valuearg.get_Str() + ")"); }
		void Insert(const ValueArg& valuearg, std::wstring) { m_db.execute(L"INSERT INTO " + T::getTableName() + L" (" + T::getColumnNames() + L") VALUES (" + valuearg.get_Str() + L")"); }
		void UpdateDb(const ValueArg& valuearg, std::string) { m_db.execute("INSERT OR REPLACE INTO " + T::getTableName() + " (" + T::getColumnNames() + ") VALUES (" + valuearg.get_Str() + ")"); }
		void UpdateDb(const ValueArg& valuearg, std::wstring) { m_db.execute(L"INSERT OR REPLACE INTO " + T::getTableName() + L" (" + T::getColumnNames() + L") VALUES (" + valuearg.get_Str() + L")"); }
		void DeleteAll(std::string) { m_db.execute("DELETE FROM  " + T::getTableName()); }
		void DeleteAll(std::wstring) { m_db.execute(L"DELETE FROM  " + T::getTableName()); }
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

	////////////////////////////////////////////////////////////////////////////////////////////
	// ostream_w, ostream_a, to_string, and to_wstring are used to help out stream of different charater type.
	std::string to_string(const wchar_t* src);
	std::wstring to_wstring(const char* src);
	struct ostream_w
	{
		std::wostream &os;
		const std::wstring d; // Delimiter
		ostream_w(std::wostream &o, const std::string& sep) :os(o), d(to_wstring(sep.c_str())) {}
		ostream_w(std::wostream &o, const std::wstring& sep) :os(o), d(sep){}
		std::wstring str(const std::string& str) const { return to_wstring(str.c_str()); }
		inline std::wstring str(const std::wstring& str) const { return str; }
	};
	struct ostream_a
	{
		std::ostream &os;
		const std::string d; // Delimiter
		ostream_a(std::ostream &o, const std::string& sep) :os(o), d(sep) {}
		ostream_a(std::ostream &o, const std::wstring& sep) :os(o), d(to_string(sep.c_str())) {}
		inline std::string str(const std::string& str) const { return str; }
		std::string str(const std::wstring& str) const { return to_string(str.c_str()); }
	};

	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Blob& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Blob& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Clob& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Clob& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Datetime& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Datetime& t);
	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Date& t);
	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Date& t);

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
	struct StrOptions
	{
		std::string str_type;			// "std::string", "std::wstring", "sqlite3pp::tstring"
		std::string str_pre;			//  "", "L", "_T("
		std::string str_post;			//  "", "", ")"
		std::string str_include;		//  include needed to support str_type. ie:("#include <string>", "#include \"sqlite3pp_ez.h\"", "#include <string>")
	};
	struct HeaderOpt
	{
		std::string dest_folder;		// Can be empty, but if folder is specified, it must end with "\\".  Otherwise it will be treated as part of the file name.
		std::string header_prefix;		// Can be empty, or can specify a desired prefix for headers created.
		std::string header_postfix;		// Can be empty, or can specify a desired postfix for header created.
	};
	struct MiscOptions
	{
		std::string delimiter;			// Only used with opereator<<, and can be and desired string value to mark seperation between field output. ie:  ",", ", ", " ", ";", ""
		bool is_public_var_members;		// True to make data members public, and false to make data members protected.
		bool exclude_get_functions;		// If true, no get function. If false, a get function is created for each data member variable.
		bool exclude_set_functions;		// If true, no set function. If false, a set function is created for each data member variable.
		bool exclude_ostream_operator;	// If true, no operator<<. If false, a creates friend declaration, getDelimiter function, and global operator<< for the class
		bool exclude_comments;			// If true, excludes comments and additional spaces.
		bool exclude_table_interface;	// If true, excludes sqlite3pp::Table interface functions ( getTableName, getColumnNames, and getStreamData), and excludes Miscellaneous function(s).
		bool use_basic_types_only;		// If true, only int, double, std::string, and std::wstring are used
		bool exclude_main_hdr_example;	// If true, excludes example code added to sql_All_Headers.h
		bool exclude_comment_out_exampl;// If true, does NOT comment out example code
	}; // Create a custom defined TblClassOptions variable, or used one of the SQLiteClassBuilder predefined types, or use the default type which is automatically set by the SQLiteClassBuilder constructor

	struct TblClassOptions
	{
		StrOptions s;
		MiscOptions m;
		HeaderOpt h;
	};

	// Use SQLiteClassBuilder to create C++ header files for tables or views
	class SQLiteClassBuilder
	{
		sqlite3pp::database m_db;
		TblClassOptions m_options;
		const TblClassOptions m_options_org;
		bool m_AppendTableToHeader;
		std::vector<std::string> m_HeadersCreated;
		std::vector<std::string> m_ClassNames;
		std::string GetType(const char* str);
		static bool dir_exists(const std::string& foldername);
		TblClassOptions Init(const StrOptions & stroptions, const MiscOptions & miscoptions, const HeaderOpt & headeropt);
		void Init(
			  const std::string& TableOrView_name
			, const std::string &WhereClause
		);
		bool ProcessClassCreation(const std::string& ClassName, std::string QueryStr = "");
		bool CreateHeaderPrefix(const std::string& TableName, std::ofstream &myfile, std::string& ClassName, std::string& HeaderUpper, std::string FirstColumnName = "", std::string LastColumnName = "", bool AppendToVect = true);
	public:
		// This constructor is best to use when creating a header for all tables in the constructor.  (Headers can also be created by calling CreateHeader or CreateAllHeaders)
		SQLiteClassBuilder(const std::string& Db_filename						
			, const StrOptions &stroptions										// StrOptions is used to define the default string type.  Can be set to a custom define StrOptions, or to one of the predefined common options (strOpt_std_string, strOpt_std_wstring, strOpt_std_tstring, strOpt_sql_tstring)
			, const std::string &WhereClause = ""								// Used when creating multiple tables.  Can specify which tables/views to include via where clause
			, const MiscOptions &miscoptions = MiscOpt_max						// MiscOptions is used to define miscellaneous options.  Can be set to a custom define MiscOptions, or to one of the predefined common options (MiscOpt_max, MiscOpt_min, MiscOpt_var)
			, const HeaderOpt &headeropt = HeaderDefaultOpt						// HeaderOpt is used to define the naming convention to use when creating the header file(s).
			, const std::string& TableOrView_name = CreateHeaderForAllTables	// If equal to "%CreateHeaderForAllTables%", a header for each table and view is created. If equal to table or view name, a single header for associated table or view is created. If empty or equal to "#NILL#", the constructor does not create any headers.
		) :m_db(Db_filename.c_str()), m_options( Init(stroptions, miscoptions, headeropt) ), m_options_org(m_options), m_AppendTableToHeader(false){Init(TableOrView_name, WhereClause);}

		// This constructor is best when crating a single header or no headers at all in the contructor. (Headers can also be created by calling CreateHeader or CreateAllHeaders)
		SQLiteClassBuilder(const std::string& Db_filename						// Only Required Field
			, const std::string& TableOrView_name = ""							// If equal to "%CreateHeaderForAllTables%", a header for each table and view is created. If equal to table or view name, a single header for associated table or view is created. If empty or equal to "#NILL#", the constructor does not create any headers.
			, const std::string &WhereClause = ""								// Used when creating multiple tables.  Can specify which tables/views to include via where clause
			, const StrOptions &stroptions = strOpt_std_string					// StrOptions is used to define the default string type.  Can be set to a custom define StrOptions, or to one of the predefined common options (strOpt_std_string, strOpt_std_wstring, strOpt_std_tstring, strOpt_sql_tstring)
			, const MiscOptions &miscoptions = MiscOpt_max						// MiscOptions is used to define miscellaneous options.  Can be set to a custom define MiscOptions, or to one of the predefined common options (MiscOpt_max, MiscOpt_min, MiscOpt_var)
			, const HeaderOpt &headeropt = HeaderDefaultOpt						// HeaderOpt is used to define the naming convention to use when creating the header file(s).
		) :m_db(Db_filename.c_str()), m_options( Init(stroptions, miscoptions, headeropt) ), m_options_org(m_options), m_AppendTableToHeader(false){Init(TableOrView_name, WhereClause);}

		~SQLiteClassBuilder();
		bool CreateAllHeaders(const std::string &WhereClause = "");
		bool CreateAllHeaders(const TblClassOptions &strtype, const std::string &WhereClause = "");
		bool CreateHeader(const std::string& TableName, const TblClassOptions *strtype = NULL, std::string QueryStr = "");

		// Predefined string options
		static const StrOptions strOpt_std_string ;		// TEXT type defaults to std::string
		static const StrOptions strOpt_std_wstring;		// TEXT type defaults to std::wstring
		static const StrOptions strOpt_sql_tstring;		// TEXT type defaults to sqlite3pp::tstring, and uses T_() macro for portability and to avoid conflicts with tchar.h
		static const StrOptions strOpt_sql_tstring_T;	// Same as strOpt_sql_tstring, but uses windows _T() macro
		// Predefined MiscOptions for common settings
		static const MiscOptions MiscOpt_max;	// sqlite3pp:Table compatible. Includes the full implementation, and it creates protected data members. It includes get_* functions, set_* functions, comments, and operator<<.
		static const MiscOptions MiscOpt_min;	// sqlite3pp:Table compatible. Creates a minimalist class, with bare essentials to interface with sqlite3pp:Table. It creates public member variables, and it excludes get_* functions, set_* functions, comments, and operator<<
		static const MiscOptions MiscOpt_var;	// *NOT* compatible with sqlite3pp:Table. It creates a class with data members only, and it's an option to be used for other interfaces.
		// Default settings for HeaderOpt
		static const HeaderOpt HeaderDefaultOpt;

		static const char *Nill; // = "#NILL#"
		static const char *CreateHeaderForAllTables; // = "%CreateHeaderForAllTables%"
	};

	using SqlBld = SQLiteClassBuilder; // Short alias

} // namespace sqlite3pp

#endif //SQLITE3PP_EZ_H
