#include <iostream>
#include "sqlite3pp.h"

#include "monolithic_examples.h"

using namespace std;


#if defined(BUILD_MONOLITHIC)
#define main	sqlite3pp_insert_all_test_main
#endif

int main(void)
{
  try {
    sqlite3pp::database db("test.db");
    {
      sqlite3pp::transaction xct(db);
      {
	sqlite3pp::command cmd(db,
			       "INSERT INTO contacts (name, phone) VALUES (:name, '1234');"
			       "INSERT INTO contacts (name, phone) VALUES (:name, '5678');"
			       "INSERT INTO contacts (name, phone) VALUES (:name, '9012');"
			       );
	{
	  cout << cmd.bind(":name", "user", sqlite3pp::copy) << endl;
#if 0 // Disabled due to deprecation in SQLite 
	  cout << cmd.execute_all() << endl;
#else
	  cout << cmd.execute() << endl;
#endif
	}
      }
      xct.commit();
    }
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
  return 0;
}
