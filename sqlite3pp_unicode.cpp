///////////////////////////////////////////////////////////////////////////
// Additional functions to handle UNICODE support
///////////////////////////////////////////////////////////////////////////
#include "sqlite3pp.h"

// ToDo:  Find a portable way to check if UNICODE and to convert UTF8 to UTF16
#include <windows.h>
#include <stringapiset.h>
#include <TCHAR.h>

namespace sqlite3pp
{
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

	std::wstring query::rows::get( int idx, std::wstring ) const
	{
		wchar_t const* Val = get( idx, (wchar_t const*)0 );
		if ( !Val )
			return  std::wstring();

		int Test = IS_TEXT_UNICODE_SIGNATURE;// IS_TEXT_UNICODE_ASCII16;
		if ( ::IsTextUnicode( Val, wcslen( Val ), NULL ) )
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
		int rc = sqlite3_prepare16_v2( db_.db_, stmt, std::wcslen( stmt ), &stmt_, myw2v.v );
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


};
