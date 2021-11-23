/*
	GNU General Public License

	Copyright (C) 2021 David Maisonave (www.axter.com)
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This source code is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*/

///////////////////////////////////////////////////////////////////////////
// Summary: Additional functions to handle UNICODE support
///////////////////////////////////////////////////////////////////////////
// ToDo:  Find a portable way to check if UNICODE and to convert UTF8 to UTF16

/*
	Usage Instructions:

*/
#include <windows.h>
#include <stringapiset.h>
#include <tchar.h>

#include "sqlite3pp_ez.h"
#include <vector>
#include <string>
#include <fstream>
#include <direct.h>

namespace sqlite3pp
{
	///////////////////////////////////////////////////////////////////////////
	// Added functions for UNICODE support
	///////////////////////////////////////////////////////////////////////////
	std::wstring to_wstring( const char* src )
	{
		int nchars = MultiByteToWideChar( CP_ACP, 0, src, -1, NULL, 0 );
		wchar_t* wsource = new wchar_t[nchars + 2]();
		if ( wsource == NULL )
			return std::wstring();
		MultiByteToWideChar( CP_ACP, 0, src, -1, wsource, nchars );
		std::wstring retrnVal = wsource;
		delete[] wsource;
		return retrnVal;
	}

	std::string to_string( const wchar_t* src )
	{
		int nchars = WideCharToMultiByte( CP_ACP, 0, src, -1, NULL, 0, NULL, NULL );
		char* source = new char[nchars + 2]();
		if ( source == NULL )
			return std::string();
		WideCharToMultiByte( CP_ACP, 0, src, -1, source, nchars, NULL, NULL );
		std::string retrnVal = source;
		delete[] source;
		return retrnVal;
	}

	std::wstring to_wstring( const std::string &src )
	{
		return to_wstring( src.c_str() );
	}

	std::string to_string( const std::wstring &src )
	{
		return to_string( src.c_str() );
	}

	std::wstring query::rows::get( int idx, std::wstring ) const
	{
		wchar_t const* Val = get( idx, (wchar_t const*)0 );
		if ( !Val )
			return  std::wstring();

		//int Test = IS_TEXT_UNICODE_SIGNATURE;// IS_TEXT_UNICODE_ASCII16;
		if ( ::IsTextUnicode( Val, static_cast<int>(wcslen( Val )), NULL ) )
			return Val;

		const char* s = (char*)(Val);
		return to_wstring( s );
	}

	int database::connect( wchar_t const * dbname, int flags, const wchar_t * vfs )
	{
		if ( !borrowing_ )
		{
			disconnect();
		}
		return sqlite3_open16( dbname, &db_ );
	}

	database::database( wchar_t const* dbname, int flags, wchar_t const* vfs ) : db_( nullptr ), borrowing_( false )
	{
		if ( dbname )
		{
			auto rc = connect( dbname, flags, vfs );
			if ( rc != SQLITE_OK )
				throw database_error( "can't connect database" );
		}
	}

	int database::execute( const std::string& sql )
	{
		return execute( sql.c_str() );
	}
	
	int database::execute( const std::wstring& sql )
	{
		return execute( to_string(sql) );
	}

	statement::statement( database& db, wchar_t const* stmt ) : db_( db ), stmt_( 0 ), tail_( 0 )
	{
		if ( stmt )
		{
			auto rc = prepare( stmt );
			if ( rc != SQLITE_OK )
				throw database_error( db_ );
		}
	}

	int statement::prepare( wchar_t const* stmt )
	{
		auto rc = finish();
		if ( rc != SQLITE_OK )
			return rc;

		return prepare_impl( stmt );
	}

	int statement::prepare_impl( wchar_t const* stmt )
	{
		union
		{
			wchar_t const** s;
			void const ** v;
		}myw2v = {&wcstail_};
		int rc = sqlite3_prepare16_v2( db_.db_, stmt, static_cast<int>(std::wcslen( stmt )), &stmt_, myw2v.v );
		return rc;
	}

	wchar_t const* query::rows::get( int idx, wchar_t const* ) const
	{
		return reinterpret_cast<wchar_t const*>(sqlite3_column_text( stmt_, idx ));
	}

	query::query( database& db, wchar_t const* stmt ) : statement( db, stmt )
	{
	}

	///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	// Added Global functions for global_db usage
	///////////////////////////////////////////////////////////////////////////
	database db_gbl::global_db;

	void setGlobalDB( const std::string& db_filename )
	{
		db_gbl::global_db = database( db_filename.c_str() );
	}

	void setGlobalDB( const std::wstring& db_filename )
	{
		db_gbl::global_db = database(db_filename.c_str());
	}

	database& getGlobalDB(  )
	{
		return db_gbl::global_db;
	}

	int Execute( const std::string& sql )
	{
		return db_gbl::global_db.execute(sql);
	}

	int Execute( const std::wstring& sql )
	{
		return db_gbl::global_db.execute( to_string( sql).c_str() );
	}
	int Connect( char const * dbname, int flags, const char * vfs )
	{
		return db_gbl::global_db.connect(dbname, flags, vfs);
	}
	int Connect( wchar_t const * dbname, int flags, const wchar_t * vfs )
	{
		return db_gbl::global_db.connect( to_string( dbname ).c_str(), flags, to_string( vfs ).c_str() );;
	}
	int Attach( char const * dbname, char const * name )
	{
		return  db_gbl::global_db.attach(dbname, name);
	}
	int Attach( wchar_t const * dbname, wchar_t const * name )
	{
		return  db_gbl::global_db.attach( to_string( dbname ).c_str(), to_string( name ).c_str() );
	}
	int Detach( char const * name )
	{
		return  db_gbl::global_db.detach(name);
	}
	int Detach( wchar_t const * name )
	{
		return db_gbl::global_db.detach( to_string(name).c_str() );
	}
	int Backup( char const * dbname, database & destdb, char const * destdbname, database::backup_handler h, int step_page )
	{
		return db_gbl::global_db.backup(dbname,destdb,destdbname,h,step_page);
	}
	int Backup( wchar_t const * dbname, database & destdb, wchar_t const * destdbname, database::backup_handler h, int step_page )
	{
		return db_gbl::global_db.backup( to_string( dbname).c_str(), destdb, to_string( destdbname).c_str(), h, step_page );
	}
	std::string GetDbErrMsg()
	{
		return db_gbl::global_db.error_msg();
	}
	std::wstring GetDbErrMsgW()
	{
		return to_wstring(db_gbl::global_db.error_msg());
	}
	int GetDbErr()
	{
		return db_gbl::global_db.error_code();
	}
	int GetDbExtErr()
	{
		return db_gbl::global_db.extended_error_code();
	}

	// sqlite_master is used when querying all the tables in a SQLite DB
	class sqlite_master
	{
	public:
		using Str_DataType = std::string;
		static Str_DataType getTableName() { return  "sqlite_master"; }
		static Str_DataType getColumnNames() { return  "type, name, tbl_name, rootpage, sql"; }
		template<class T> void getStreamData(T q) { q.getter() >> type >> name >> tbl_name >> rootpage >> sql; }
		static int getColumnCount() { return 5; }
	public:
		Str_DataType type;
		Str_DataType name;
		Str_DataType tbl_name;
		Str_DataType rootpage;
		Str_DataType sql;
	};

	// Create a class having protected member variables, and a get_* and set_* function for each column in the table/view
	const TblClassOptions SQLiteClassBuilder::std_string_protected_members = { "std::string", "", "", "#include <string>", "SQL\\", ",", "sql_", "", false, false, false, false, false };
	const TblClassOptions SQLiteClassBuilder::std_wstring_protected_members = { "std::wstring", "L", "", "#include <string>", "SQL\\", ",", "sql_", "", false, false, false, false, false };
	const TblClassOptions SQLiteClassBuilder::std_tstring_protected_members = { "std::basic_string<TCHAR>", "_T(", ")", "#include <string>\n#include <tchar.h>", "SQL\\", ",", "sql_", "", false, false, false, false, false };
	const TblClassOptions SQLiteClassBuilder::sql_tstring_protected_members = { "sqlite3pp::tstring", "_T(", ")", "#include <tchar.h>\n#include \"sqlite3pp_ez.h\"", "SQL\\", ",", "sql_", "", false, false, false, false, false };
	// Create minimalist (bare-bone) class
	const TblClassOptions SQLiteClassBuilder::std_string_minimal = { "std::string", "", "", "#include <string>", "SQL\\", ",", "sql_", "", true, true, true, true, true };
	const TblClassOptions SQLiteClassBuilder::std_wstring_minimal = { "std::wstring", "L", "", "#include <string>", "SQL\\", ",", "sql_", "", true, true, true, true, true };
	const TblClassOptions SQLiteClassBuilder::std_tstring_minimal = { "std::basic_string<TCHAR>", "_T(", ")", "#include <string>\n#include <tchar.h>", "SQL\\", ",", "sql_", "", true, true, true, true, true };
	const TblClassOptions SQLiteClassBuilder::sql_tstring_minimal = { "sqlite3pp::tstring", "_T(", ")", "#include <tchar.h>\n#include \"sqlite3pp_ez.h\"", "SQL\\", ",", "sql_", "", true, true, true, true, true };

	const char *SQLiteClassBuilder::Nill = "#NILL#";

	std::string SQLiteClassBuilder::GetType(const char* str)
	{
		const char* DefaultType = "Str_DataType";
		if (!str) return DefaultType;
		if (strcmp("INTEGER", str) == 0 || strcmp("INT", str) == 0 || strcmp("NUMERIC", str) == 0)
			return "int";
		if (strcmp("REAL", str) == 0 || strcmp("DECIMAL", str) == 0 || strcmp("FLOAT", str) == 0 || strcmp("DECIMAL", str) == 0 || strcmp("DATE", str) == 0 || strcmp("DATETIME", str) == 0)
			return "double";
		if (strcmp("BOOLEAN", str) == 0)
			return "bool";
		return DefaultType;
	}
	
	bool SQLiteClassBuilder::dir_exists(const std::string& foldername)
	{
		struct stat st = { 0 };
		stat(foldername.c_str(), &st);
		return st.st_mode & S_IFDIR;
	}
	
	SQLiteClassBuilder::SQLiteClassBuilder(const std::string& Db_filename	// Only Required Field
		, const TblClassOptions &strtype 									// The default option is commonly used. This argument is used to define options in creating headers and content.  Can use a custom defined TblClassOptions, or one of the 8 predefined types (std_string_protected_members, std_wstring_protected_members, std_tstring_protected_members, sql_tstring_protected_members,std_string_minimal, std_wstring_minimal, std_tstring_minimal, sql_tstring_minimal).
		, const std::string& TableOrView_name								// If equal to "#NILL#", no header file is created. If empty, a header for each table and view is created. If equal to table or view name, it will create single header for associated table or view.
		, const std::string &PostFixWhereClause								// Used when creating multiple tables.  Can specify which set of tables/views to include.
	) :m_db(Db_filename.c_str()), m_strtype(strtype), m_AppendTableToHeader(false)
	{
		if (TableOrView_name.empty())
			CreateAllHeaders(m_strtype, PostFixWhereClause);
		else if (TableOrView_name != Nill)
			CreateHeader(TableOrView_name);
	}
	
	SQLiteClassBuilder::~SQLiteClassBuilder()
	{
		m_db.disconnect();
	}

	bool SQLiteClassBuilder::CreateAllHeaders(const std::string &PostFixWhereClause)
	{
		return CreateAllHeaders(m_strtype, PostFixWhereClause);
	}

	bool SQLiteClassBuilder::CreateAllHeaders(const TblClassOptions &strtype, const std::string &PostFixWhereClause)
	{
		m_strtype = strtype;
		const std::string OrgPrefix = m_strtype.header_prefix;
		Table<sqlite_master> tbl(m_db, "where (type = 'table' or type = 'view') " + PostFixWhereClause);
		for (auto t : tbl)
		{
			m_strtype.header_prefix = OrgPrefix + t.type + "_";
			CreateHeader(t.tbl_name);
		}
		m_strtype.header_prefix = OrgPrefix;
		return true;
	}

	bool SQLiteClassBuilder::CreateHeader(const std::string& TableName, std::string QueryStr)
	{
		if (QueryStr.empty())
			QueryStr = "SELECT * FROM " + TableName;
		sqlite3pp::query qry(m_db, QueryStr.c_str());
		std::vector<std::pair<std::string, std::string> > columns;
		std::vector<std::pair<std::string, std::string> > columns_with_comma;
		for (int i = 0; i < qry.column_count(); ++i)
		{
			if (strstr(qry.column_name(i), ":") != NULL) continue;
			columns.push_back(std::pair<std::string, std::string>(qry.column_name(i), GetType(qry.column_decltype(i))));
			columns_with_comma.push_back(std::pair<std::string, std::string>(qry.column_name(i), i ? ", " : ""));
		}

		std::ios_base::openmode openMode = m_AppendTableToHeader ? std::ios_base::out | std::ios_base::app : std::ios_base::out;
		if (!m_strtype.dest_folder.empty() && !dir_exists(m_strtype.dest_folder))
			_mkdir(m_strtype.dest_folder.c_str());
		const std::string& ClassName = m_strtype.header_prefix + TableName + m_strtype.header_postfix;
		std::ofstream myfile(( m_strtype.dest_folder + ClassName + ".h").c_str(), openMode);
		if (!myfile.is_open())
			return false;
		char HeaderUpper[256] = { 0 };
		strcpy_s(HeaderUpper, (ClassName + "_H").c_str());
		_strupr_s(HeaderUpper);
		// Add includes needed to support specified m_strtype.str_type
		myfile << "#ifndef " << HeaderUpper << std::endl;
		myfile << "#define " << HeaderUpper << std::endl;
		myfile << m_strtype.str_include << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////
		// Create Table/View class, and create a define type for strings
		myfile << "\nclass " << ClassName << "\n{\npublic:" << std::endl;
		myfile << "\tusing Str_DataType = " << m_strtype.str_type << ";" << std::endl;

		if (!m_strtype.exclude_comments)
			myfile << "\n\t// getTableName, getColumnNames, and getStreamData are required for sqlite3pp::Table template class" << std::endl;
		// Create getTableName member function. It's needed for sqlite3pp::Table template class
		myfile << "\tstatic Str_DataType getTableName() { return " << m_strtype.str_pre << " \"" << TableName << "\" " << m_strtype.str_post << "; }" << std::endl;
		// Create getColumnNames member function. It's needed for sqlite3pp::Table template class
		myfile << "\tstatic Str_DataType getColumnNames() { return " << m_strtype.str_pre << " \"";
		for (auto c : columns_with_comma)
			myfile << c.second << c.first;
		myfile << "\"" << m_strtype.str_post << "; }" << std::endl;
		// Create getStreamData member function. It's needed for sqlite3pp::Table template class
		myfile << "\ttemplate<class T> void getStreamData( T q ) { q.getter() ";
		for (auto c : columns)
			myfile << " >> " << c.first;
		myfile << ";}" << std::endl;

		// Miscellaneous functions
		if (!m_strtype.exclude_comments)
			myfile << "\n\t// Miscellaneous functions" << std::endl;
		myfile << "\tstatic int getColumnCount() { return " << qry.column_count() << "; }" << std::endl;

		// Define get function for each data member variable. Always create these functions if member variables are protected.
		if (m_strtype.exclude_get_functions != true || m_strtype.is_public_var_members != true)
		{
			if (!m_strtype.exclude_comments)
				myfile << "\n\t// A get_ function for each field in the table";
			if (m_strtype.is_public_var_members != true)
				myfile << ", which allows read-only access to protected member variables";
			myfile << "." << std::endl;
			for (auto c : columns)
				myfile << "\tconst " << c.second << "& get_" << c.first << "() {return " << c.first << ";}" << std::endl;
		}

		// Define set function for each data member variable.
		if (m_strtype.exclude_set_functions != true)
		{
			if (!m_strtype.exclude_comments)
				myfile << "\n\t// A set_ function for each field in the table." << std::endl;
			for (auto c : columns)
				myfile << "\tvoid set_" << c.first << "(const " << c.second << "& data__) {" << c.first << " = data__;}" << std::endl;
		}

		if (!m_strtype.exclude_comments)
			myfile << "\n\t// A member variable for each field in the table" << std::endl;
		// Define if data member variables are protected or public
		const char* publicOrPrivate = m_strtype.is_public_var_members ? "public" : "protected";
		myfile << publicOrPrivate << ":" << std::endl;
		// Define data member variables associated with the table/view
		for (auto c : columns)
			myfile << "\t" << c.second << " " << c.first << ";" << std::endl;

		if (m_strtype.exclude_ostream_operator != true)
		{
			if (!m_strtype.exclude_comments)
				myfile << "\n\t// Optional operator<< declarations. Set exclude_ostream_operator to true to exclude this operator when this class is created." << std::endl;
			// Declare operator<< friend
			myfile << "\ttemplate<class T> friend T& operator<<(T& os, const " << ClassName << "& t);" << std::endl;
			// Create getDelimiter member function. It's needed for operator<<
			myfile << "\tstatic Str_DataType getDelimiter() { return " << m_strtype.str_pre << " \"" << m_strtype.delimiter << "\" " << m_strtype.str_post << "; }" << std::endl;
		}


		// End of Table/View class
		myfile << "};" << std::endl;
		////////////////////////////////////////////////////////////////////////////////////////////

		if (m_strtype.exclude_ostream_operator != true)
		{
			if (!m_strtype.exclude_comments)
				myfile << "\n// Optional operator<< function for class " << ClassName << ". Set exclude_ostream_operator to true to exclude this operator when this class is created." << std::endl;
			////////////////////////////////////////////////////////////////////////////////////////////
			// Create associated opereator<<
			myfile << "template<class T> T& operator<<(T& os, const " << ClassName << "& t)\n{" << std::endl;
			myfile << "\tstatic const " << ClassName << "::Str_DataType delimiter = t.getDelimiter();" << std::endl;
			myfile << "\tos";
			std::string delimiter_tmp;
			for (auto c : columns)
			{
				myfile << delimiter_tmp << " << t." << c.first;
				if (delimiter_tmp.empty())
					delimiter_tmp = " << delimiter";
			}
			myfile << ";\n\treturn os;\n};" << std::endl << std::endl;
			////////////////////////////////////////////////////////////////////////////////////////////
		}

		myfile << "\n#endif //" << HeaderUpper << std::endl;

		//Done
		myfile.close();
		return true;
	}

};
