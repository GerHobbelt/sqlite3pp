// ToDo:  Find a portable way to check if UNICODE and to convert UTF8 to UTF16
#include <tchar.h>

#include "sqlite3pp_templates.h"

//Static functions for global db
namespace sqlite3pp
{
	database db_gbl::global_db;

	void db_gbl::setGlobalDB( const std::string& db_filename )
	{
		global_db = database( db_filename.c_str() );
	}

	void db_gbl::setGlobalDB( const std::wstring& db_filename )
	{
		global_db = database(db_filename.c_str());
	}

	int db_gbl::Execute( const std::string& sql )
	{
		return global_db.execute(sql);
	}

	int db_gbl::Execute( const std::wstring& sql )
	{
		return global_db.execute( to_string( sql).c_str() );
	}
	int db_gbl::Connect( char const * dbname, int flags, const char * vfs )
	{
		return global_db.connect(dbname, flags, vfs);
	}
	int db_gbl::Connect( wchar_t const * dbname, int flags, const wchar_t * vfs )
	{
		return global_db.connect( to_string( dbname ).c_str(), flags, to_string( vfs ).c_str() );;
	}
	int db_gbl::Attach( char const * dbname, char const * name )
	{
		return  global_db.attach(dbname, name);
	}
	int db_gbl::Attach( wchar_t const * dbname, wchar_t const * name )
	{
		return  global_db.attach( to_string( dbname ).c_str(), to_string( name ).c_str() );
	}
	int db_gbl::Detach( char const * name )
	{
		return  global_db.detach(name);
	}
	int db_gbl::Detach( wchar_t const * name )
	{
		return global_db.detach( to_string(name).c_str() );
	}
	int db_gbl::Backup( char const * dbname, database & destdb, char const * destdbname, database::backup_handler h, int step_page )
	{
		return global_db.backup(dbname,destdb,destdbname,h,step_page);
	}
	int db_gbl::Backup( wchar_t const * dbname, database & destdb, wchar_t const * destdbname, database::backup_handler h, int step_page )
	{
		return global_db.backup( to_string( dbname).c_str(), destdb, to_string( destdbname).c_str(), h, step_page );
	}
	std::string db_gbl::GetErrMsg()
	{
		return global_db.error_msg();
	}
	std::wstring db_gbl::GetErrMsgW()
	{
		return to_wstring(global_db.error_msg());
	}
	int db_gbl::GetErr()
	{
		return global_db.error_code();
	}
	int db_gbl::GetExtErr()
	{
		return global_db.extended_error_code();
	}
};
