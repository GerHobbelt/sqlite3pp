// ToDo:  Find a portable way to check if UNICODE and to convert UTF8 to UTF16
#include <tchar.h>


#include "sqlite3pp_templates.h"

namespace sqlite3pp
{
	database Table_Base::global_db;

	void Table_Base::setGlobalDB( const std::string& db_filename )
	{
		global_db = database( db_filename.c_str() );
	}

	void Table_Base::setGlobalDB( const std::wstring& db_filename )
	{
		global_db = database(db_filename.c_str());
	}
};
