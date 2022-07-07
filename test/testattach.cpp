#include <iostream>
#include "sqlite3pp.h"

#include "monolithic_examples.h"

using namespace std;


#if defined(BUILD_MONOLITHIC)
#define main	sqlite3pp_attach_test_main
#endif

int main(void)
{
  try {
    sqlite3pp::database db("foods.db");

    db.attach("test.db", "test");
    {
      sqlite3pp::transaction xct(db);
      {
	sqlite3pp::query qry(db, "SELECT epi.* FROM episodes epi, test.contacts con WHERE epi.id = con.id");

	for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
	  for (int j = 0; j < qry.column_count(); ++j) {
	    cout << (*i).get<char const*>(j) << "\t";
	  }
	  cout << endl;
	}
	cout << endl;
      }
    }
  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
  return 0;
}
