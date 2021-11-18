
#include "sqlite3pp_templates.h"

namespace sqlite3pp
{
	database Table_Base::db;
	void Table_Base::setGlobalDB( database &db_ )
	{
		db = db_;
	}
};
