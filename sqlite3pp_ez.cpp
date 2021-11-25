/*
	GNU General Public License

	Copyright (C) 2021 David Maisonave (www.axter.com)
	The RegexAssistant source code is free software. You can redistribute it and/or modify it under the terms of the GNU General Public License.
	This source code is distributed in the hope that it will be useful,	but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	For usage examples see  https://github.com/David-Maisonave/sqlite3pp_EZ
							or sqlite3pp_ez.h
*/
#include <windows.h>
#include <stringapiset.h>
#include <tchar.h>

#include "sqlite3pp_ez.h"
#include <vector>
#include <string>
#include <fstream>
#include <direct.h>
#include <cassert>

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
			char const** a;
			void const ** v;
		}myw2v = {&tail_ };
		int rc = sqlite3_prepare16_v2( db_.db_, stmt, static_cast<int>(std::wcslen( stmt )), &stmt_, myw2v.v );
		return rc;
	}

	query::query( database& db, wchar_t const* stmt ) : statement( db, stmt )
	{
	}

	///////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////////////////////
	// Added Global functions for global_db usage
	///////////////////////////////////////////////////////////////////////////
	database db_gbl::global_db;
	const char db_gbl::TableArg_PreExecuteArg[] = "PreExecuteArg";
	const char db_gbl::TableArg_WhereClauseArg[] = "WhereClauseArg";
	const char db_gbl::TableArg_InsertArg[] = "InsertArg";
	const char db_gbl::TableArg_DbFileNameArg[] = "DbFileNameArg";
	const char db_gbl::TableArg_ValueArg[] = "ValueArg";

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
	int GetDbErrNo()
	{
		return db_gbl::global_db.error_code();
	}
	int GetDbExtErrNo()
	{
		return db_gbl::global_db.extended_error_code();
	}

	// sqlite_master is used when querying all the tables in a SQLite DB
	class sqlite_master
	{
	public:
		using StrType = std::string;
		static StrType getTableName() { return  "sqlite_master"; }
		static StrType getColumnNames() { return  "type, name, tbl_name, rootpage, sql"; }
		template<class T> void getStreamData(T q) { q.getter() >> type >> name >> tbl_name >> rootpage >> sql; }
		static int getColumnCount() { return 5; }
	public:
		StrType type;
		StrType name;
		StrType tbl_name;
		StrType rootpage;
		StrType sql;
	};

	// Predefined string options
	const StrOptions SQLiteClassBuilder::strOpt_std_string  = { "std::string", "", "", "#include <string>"  };
	const StrOptions SQLiteClassBuilder::strOpt_std_wstring = { "std::wstring", "L", "", "#include <string>" };
	const StrOptions SQLiteClassBuilder::strOpt_std_tstring = { "std::basic_string<TCHAR>", "_T(", ")", "#include <string>\n#include <tchar.h>" };
	const StrOptions SQLiteClassBuilder::strOpt_sql_tstring = { "sqlite3pp::tstring", "_T(", ")", "#include <tchar.h>\n#include \"sqlite3pp_ez.h\"" };
	// Predefined MiscOptions for common settings
	const MiscOptions SQLiteClassBuilder::MiscOpt_max = { ",", false, false, false, false, false, false };
	const MiscOptions SQLiteClassBuilder::MiscOpt_min = { ",", true, true, true, true, true, false, false };
	const MiscOptions SQLiteClassBuilder::MiscOpt_var = { ",", true, true, true, true, true, true, false };
	// Default settings for HeaderOpt
	const HeaderOpt SQLiteClassBuilder::HeaderDefaultOpt = { "SQL\\", "sql_", "" };

	const char *SQLiteClassBuilder::Nill = "#NILL#";
	const char *SQLiteClassBuilder::CreateHeaderForAllTables = "%CreateHeaderForAllTables%";

	std::string SQLiteClassBuilder::GetType(const char* str)
	{
		const char* DefaultType = "StrType";
		if (!str) return DefaultType;
		// There's no pratical method for handling blob or clob other than the Blob and Clob type, so don't even include them  in an option to declare them any other way.
		if (strcmp("BLOB", str) == 0)
			return "Blob";
		if (strcmp("CLOB", str) == 0)	
			return "Clob";

#ifdef SQLITE3PP_CONVERT_TO_RESULTING_AFFINITY // If defined convert types to Resulting Affinity (int, double, or StrType)
		bool UseBaseTypes = true;
#else  // Use SQLite3 sub types and use type names
		bool UseBaseTypes = false;
#endif //SQLITE3PP_CONVERT_TO_RESULTING_AFFINITY
		
		if (UseBaseTypes || m_options.m.use_basic_types_only)
		{
			if (strcmp("INTEGER", str) == 0 || strcmp("INT", str) == 0 || strcmp("TINYINT", str) == 0 || strcmp("SMALLINT", str) == 0 || strcmp("MEDIUMINTSMALLINT", str) == 0 || strcmp("BIGINT", str) == 0 || strcmp("UNSIGNED BIG INT", str) == 0 || strcmp("INT2", str) == 0 || strcmp("INT8", str) == 0)
				return "int";
			if (strcmp("REAL", str) == 0 || strcmp("DOUBLE", str) == 0 || strcmp("DOUBLE PRECISION", str) == 0 || strcmp("FLOAT", str) == 0 || strncmp("DECIMAL", str, 7) == 0 || strcmp("BOOLEAN", str) == 0 || strcmp("DATE", str) == 0 || strcmp("DATETIME", str) == 0 || strcmp("NUMERIC", str) == 0)
				return "double";

			if (m_options.m.use_basic_types_only)
			{
				// String types
				if (strcmp("TEXT", str) == 0 || strncmp("CHARACTER", str, 9) == 0 || strncmp("VARCHAR", str, 7) == 0 || strncmp("VARYING CHARACTER", str, 17) == 0)
					return "std::string";
				if (strncmp("NVARCHAR", str, 8) == 0 || strncmp("NATIVE CHARACTER", str, 16) == 0 || strncmp("NCHAR", str, 5) == 0)	
					return "std::wstring";
			}
		}
		else
		{
			if (strcmp("INTEGER", str) == 0)				
				return "Integer";
			if (strcmp("INT", str) == 0)					
				return "Int";
			if (strcmp("INT2", str) == 0)					
				return "Int2";
			if (strcmp("INT8", str) == 0)					
				return "Int8";
			if (strcmp("TINYINT", str) == 0)				
				return "Tinyint";
			if (strcmp("SMALLINT", str) == 0)				
				return "Smallint";
			if (strcmp("MEDIUMINTSMALLINT", str) == 0)		
				return "Mediumint";
			if (strcmp("BOOLEAN", str) == 0)				
				return "Boolean";
			if (strcmp("BIGINT", str) == 0)					
				return "Bigint";
			if (strcmp("UNSIGNED BIG INT", str) == 0)		
				return "UBigint";
			if (strcmp("DATE", str) == 0)					
				return "Date";
			if (strcmp("DATETIME", str) == 0)				
				return "Datetime";
			if (strcmp("NUMERIC", str) == 0)				
				return "Numeric";
			if (strncmp("DECIMAL", str, 7) == 0)			
				return "Decimal";
			if (strcmp("REAL", str) == 0)					
				return "Real";
			if (strcmp("DOUBLE PRECISION", str) == 0)		
				return "DoublePrcsn";
			if (strcmp("DOUBLE", str) == 0)					
				return "Double";
			if (strcmp("FLOAT", str) == 0)					
				return "Float";

			// String types
			if (strcmp("TEXT", str) == 0)					
				return "ST::Text";
			if (strncmp("CHARACTER", str, 9) == 0)			
				return "Character";
			if (strncmp("VARYING CHARACTER", str, 17) == 0
				|| strncmp("VARCHAR", str, 7) == 0)			
				return "Varchar";
			if (strncmp("NATIVE CHARACTER", str, 16) == 0
				|| strncmp("NCHAR", str, 5) == 0)			
				return "Nchar";
			if (strncmp("NVARCHAR", str, 8) == 0)			
				return "Nvarchar";
		}

		assert(0); // Always assert, because the code should not reach this point.
		return DefaultType; // Handle it gracefully for release mode.
	}
	
	bool SQLiteClassBuilder::dir_exists(const std::string& foldername)
	{
		struct stat st = { 0 };
		stat(foldername.c_str(), &st);
		return st.st_mode & S_IFDIR;
	}

	void SQLiteClassBuilder::Init(const std::string & TableOrView_name, const std::string & PostFixWhereClause, const StrOptions & stroptions, const MiscOptions & miscoptions, const HeaderOpt & headeropt)
	{
		m_options = { stroptions , miscoptions , headeropt};
		if (TableOrView_name == CreateHeaderForAllTables)
			CreateAllHeaders(m_options, PostFixWhereClause);
		else if (!TableOrView_name.empty() && TableOrView_name != Nill)
			CreateHeader(TableOrView_name);
	}
		
	SQLiteClassBuilder::~SQLiteClassBuilder()
	{
		m_db.disconnect();
	}

	bool SQLiteClassBuilder::CreateAllHeaders(const std::string &PostFixWhereClause)
	{
		return CreateAllHeaders(m_options, PostFixWhereClause);
	}

	bool SQLiteClassBuilder::CreateAllHeaders(const TblClassOptions &strtype, const std::string &PostFixWhereClause)
	{
		m_options = strtype;
		const std::string OrgPrefix = m_options.h.header_prefix;
		using SQLiteMaster = Table<sqlite_master>;
		SQLiteMaster tbl(m_db, SQLiteMaster::WhereClauseArg("where (type = 'table' or type = 'view') " + PostFixWhereClause));
		for (auto t : tbl)
		{
			m_options.h.header_prefix = OrgPrefix + t.type + "_";
			CreateHeader(t.tbl_name);
		}
		m_options.h.header_prefix = OrgPrefix;
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
		if (!m_options.h.dest_folder.empty() && !dir_exists(m_options.h.dest_folder))
			_mkdir(m_options.h.dest_folder.c_str());
		const std::string& ClassName = m_options.h.header_prefix + TableName + m_options.h.header_postfix;
		std::ofstream myfile(( m_options.h.dest_folder + ClassName + ".h").c_str(), openMode);
		if (!myfile.is_open())
			return false;
		char HeaderUpper[256] = { 0 };
		strcpy_s(HeaderUpper, (ClassName + "_H").c_str());
		_strupr_s(HeaderUpper);
		// Add includes needed to support specified m_options.str_type
		myfile << "#ifndef " << HeaderUpper << std::endl;
		myfile << "#define " << HeaderUpper << std::endl;
		myfile << m_options.s.str_include << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////
		// Create Table/View class, and create a define type for strings
		myfile << "\nclass " << ClassName << ": public sqlite3pp::db_gbl\n{\npublic:" << std::endl;
		myfile << "\tusing StrType = " << m_options.s.str_type << ";\n\tusing ST = StrTypes<StrType>;" << std::endl;

		if (!m_options.m.exclude_table_interface)
		{
			if (!m_options.m.exclude_comments)
				myfile << "\n\t// getTableName, getColumnNames, and getStreamData are required for sqlite3pp::Table template class" << std::endl;
			// Create getTableName member function. It's needed for sqlite3pp::Table template class
			myfile << "\tstatic StrType getTableName() { return " << m_options.s.str_pre << " \"" << TableName << "\" " << m_options.s.str_post << "; }" << std::endl;
			// Create getColumnNames member function. It's needed for sqlite3pp::Table template class
			myfile << "\tstatic StrType getColumnNames() { return " << m_options.s.str_pre << " \"";
			for (auto c : columns_with_comma)
				myfile << c.second << c.first;
			myfile << "\"" << m_options.s.str_post << "; }" << std::endl;
			// Create getStreamData member function. It's needed for sqlite3pp::Table template class
			myfile << "\ttemplate<class T> void getStreamData( T q ) { q.getter() ";
			for (auto c : columns)
				myfile << " >> " << c.first;
			myfile << ";}" << std::endl;

			// Miscellaneous functions
			if (!m_options.m.exclude_comments)
				myfile << "\n\t// Miscellaneous functions" << std::endl;
			myfile << "\tstatic int getColumnCount() { return " << qry.column_count() << "; }" << std::endl;
		}

		// Define get function for each data member variable. Always create these functions if member variables are protected.
		if (m_options.m.exclude_get_functions != true || m_options.m.is_public_var_members != true)
		{
			if (!m_options.m.exclude_comments)
				myfile << "\n\t// A get_ function for each field in the table";
			if (m_options.m.is_public_var_members != true)
				myfile << ", which allows read-only access to protected member variables";
			myfile << "." << std::endl;
			for (auto c : columns)
				myfile << "\tconst " << c.second << "& get_" << c.first << "() const {return " << c.first << ";}" << std::endl;
		}

		// Define set function for each data member variable.
		if (m_options.m.exclude_set_functions != true)
		{
			if (!m_options.m.exclude_comments)
				myfile << "\n\t// A set_ function for each field in the table." << std::endl;
			for (auto c : columns)
				myfile << "\tvoid set_" << c.first << "(const " << c.second << "& data__) {" << c.first << " = data__;}" << std::endl;
		}

		if (!m_options.m.exclude_comments)
			myfile << "\n\t// A member variable for each field in the table" << std::endl;
		// Define if data member variables are protected or public
		const char* publicOrPrivate = m_options.m.is_public_var_members ? "public" : "protected";
		myfile << publicOrPrivate << ":" << std::endl;
		// Define data member variables associated with the table/view
		for (auto c : columns)
			myfile << "\t" << c.second << " " << c.first << ";" << std::endl;

		if (m_options.m.exclude_ostream_operator != true)
		{
			if (!m_options.m.exclude_comments)
				myfile << "\n\t// Optional operator<< declarations. Set exclude_ostream_operator to true to exclude this operator when this class is created." << std::endl;
			// Declare operator<< friend
			myfile << "\ttemplate<class T> friend T& operator<<(T& os, const " << ClassName << "& t);" << std::endl;
			// Create getDelimiter member function. It's needed for operator<<
			myfile << "\tstatic StrType getDelimiter() { return " << m_options.s.str_pre << " \"" << m_options.m.delimiter << "\" " << m_options.s.str_post << "; }" << std::endl;
		}


		// End of Table/View class
		myfile << "};" << std::endl;
		////////////////////////////////////////////////////////////////////////////////////////////

		if (m_options.m.exclude_ostream_operator != true)
		{
			if (!m_options.m.exclude_comments)
				myfile << "\n// Optional operator<< function for class " << ClassName << ". Set exclude_ostream_operator to true to exclude this operator when this class is created." << std::endl;
			////////////////////////////////////////////////////////////////////////////////////////////
			// Create associated opereator<<
			myfile << "template<class T> T& operator<<(T& os, const " << ClassName << "& t)\n{" << std::endl;
			myfile << "\tstatic const " << ClassName << "::StrType delimiter = t.getDelimiter();" << std::endl;
			myfile << "\tos";
			std::string delimiter_tmp;
			for (auto c : columns)
			{
				myfile << delimiter_tmp << " << t." << c.first;
				if (delimiter_tmp.empty())
					delimiter_tmp = " << delimiter";
			}
			myfile << ";\n\treturn os;\n}" << std::endl << std::endl;
			////////////////////////////////////////////////////////////////////////////////////////////
		}

		myfile << "\n#endif // !" << HeaderUpper << std::endl;

		//Done
		myfile.close();
		return true;
	}

	// Additional implementation for SQLite3pp
#ifndef SQLITE3PP_CONVERT_TO_RESULTING_AFFINITY

#ifndef SQLITE3PP_NO_UNICODE
	wchar_t const* query::rows::get(int idx, wchar_t const*) const
	{
		return reinterpret_cast<wchar_t const*>(sqlite3_column_text16(stmt_, idx));
	}

	std::wstring query::rows::get(int idx, const std::wstring&) const
	{
		std::wstring value;
		const char * strtype = sqlite3_column_decltype(stmt_, idx);
		wchar_t const* Val = get(idx, (wchar_t const*)0);
#ifndef SQLITE3PP_ALLOW_NULL_STRING_RETURN
		if (!Val)
			return value;
#endif  // !SQLITE3PP_ALLOW_NULL_STRING_RETURN

		if (strcmp(strtype, "TEXT") == 0 || strncmp("CHARACTER", strtype, 9) == 0 || strncmp("VARYING CHARACTER", strtype, 17) == 0 || strncmp("VARCHAR", strtype, 7) == 0)
			value = to_wstring((char*)(Val));
		else if ( strncmp("NCHAR", strtype, 5) == 0 || strncmp("NVARCHAR", strtype, 8) == 0 || strncmp("NATIVE CHARACTER", strtype, 16) == 0)
			value = Val;
		else
		{
			assert(0);// Code should NOT get here.  If it does something went wrong.
			value = to_wstring((char*)(Val)); // Handle it gracefully in release mode.
		}

		return value; 
	}

#endif// !SQLITE3PP_NO_UNICODE

	Blob query::rows::get(int idx, const Blob&) const
	{
		const int data_len = column_bytes(idx);
		const Tinyint* ptr = static_cast<const Tinyint*>(sqlite3_column_blob(stmt_, idx));
		Blob data(new std::vector<Tinyint>(ptr, ptr + data_len));
		return data;
	}

	Clob query::rows::get(int idx, const Clob&) const
	{
		const int data_len = column_bytes(idx);
		const char* ptr = static_cast<const char*>(sqlite3_column_blob(stmt_, idx));
		Clob data(new std::vector<char>(ptr, ptr + data_len));
		return data;
	}

	Tinyint query::rows::get(int idx, const Tinyint&) const
	{
		return static_cast<Tinyint>(get(idx, int()));
	}

	Smallint query::rows::get(int idx, const Smallint&) const
	{
		return static_cast<Smallint>(get(idx, int()));
	}

	Boolean query::rows::get(int idx, const Boolean&) const
	{
		return static_cast<Boolean>(get(idx, int()));
	}

	UBigint query::rows::get(int idx, const UBigint&) const
	{
		return static_cast<UBigint>(sqlite3_column_int64(stmt_, idx));
	}

	Date query::rows::get(int idx, const Date&) const
	{
		const char* s = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, idx));
		std::tm d = { 0 };
		Date data = { 0 };
		int rc = sscanf_s(s, "%d-%d-%d", &d.tm_year, &d.tm_mon, &d.tm_mday);
		if (rc < 1 || !d.tm_mday)
			return data;
		if (d.tm_year > 1900)
		{
			d.tm_year -= 1900;
			d.tm_mon -= 1;
		}
		data.t = mktime(&d);
		if (data.t == -1)
			return Date();
		return data;
	}

	Datetime query::rows::get(int idx, const Datetime&) const
	{
		Datetime data = { 0 };
		const char* s = reinterpret_cast<const char*>(sqlite3_column_text(stmt_, idx));
		int rc = sscanf_s(s, "%d-%d-%d %d:%d:%d", &data.tm_struct.tm_year, &data.tm_struct.tm_mon, &data.tm_struct.tm_mday, &data.tm_struct.tm_hour, &data.tm_struct.tm_min, &data.tm_struct.tm_sec);
		if (rc < 1 || !data.tm_struct.tm_mday)
		{
			return Datetime();
		}

		if (data.tm_struct.tm_year > 1900)
		{
			data.tm_struct.tm_year -= 1900;
			data.tm_struct.tm_mon -= 1;
		}
		return data;
	}

	std::wostream& operator<<(std::wostream& os, const Character& t)
	{
		os << to_wstring(t);
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const Nchar& t)
	{
		os << to_string(t);
		return os;
	}

	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Blob& t)
	{
		std::string data(t->data(), t->data() + t->size());
		std::wstring wdata = to_wstring(data);
		os.write(wdata.c_str(), wdata.size());
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Blob& t)
	{
		const char* ptr = (const char*)t->data();
		os.write(ptr, t->size());
		return os;
	}

	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Clob& t)
	{
		std::string data(t->data(), t->data() + t->size());
		std::wstring wdata = to_wstring(data);
		os.write(wdata.c_str(), wdata.size());
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Clob& t)
	{
		const char* ptr = (const char*)t->data();
		os.write(ptr, t->size());
		return os;
	}

	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Datetime& t)
	{
		if (t.tm_struct.tm_mday)
		{
			wchar_t buf[256] = { 0 };
			wcsftime(buf, sizeof(buf), L"%Y-%m-%d %H:%M:%S", &t.tm_struct);
			os << buf;
		}
		else
			os << L"0000-00-00 00:00:00";
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Datetime& t)
	{
		if (t.tm_struct.tm_mday)
		{
			char buf[256] = { 0 };
			strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &t.tm_struct);
			os << buf;
		}
		else
			os << "0000-00-00 00:00:00";
		return os;
	}

	std::wostream& operator<<(std::wostream& os, const sqlite3pp::Date& t)
	{
		if (t.t > 0)
		{
			wchar_t buf[256] = { 0 };
			std::tm tm_struct = { 0 };
			gmtime_s(&tm_struct, &t.t);
			wcsftime(buf, sizeof(buf), L"%Y-%m-%d", &tm_struct);
			os << buf;
		}
		else
			os << L"0000-00-00";
		return os;
	}

	std::ostream& operator<<(std::ostream& os, const sqlite3pp::Date& t)
	{
		if (t.t > 0)
		{
			char buf[256] = { 0 };
			std::tm tm_struct = { 0 };
			gmtime_s(&tm_struct, &t.t);
			strftime(buf, sizeof(buf), "%Y-%m-%db", &tm_struct);
			os << buf;
		}
		else
			os << "0000-00-00";
		return os;
	}
#endif// !SQLITE3PP_CONVERT_TO_RESULTING_AFFINITY
};
