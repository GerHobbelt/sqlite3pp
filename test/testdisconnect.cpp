#include <iostream>
#include "sqlite3pp.h"

#include "monolithic_examples.h"

using namespace std;


#if defined(BUILD_MONOLITHIC)
#define main	sqlite3pp_disconnect_test_main
#endif

int main(void)
{
  try {
    sqlite3pp::database db("test.db");
    {
      sqlite3pp::transaction xct(db);
      {
	sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES ('AAAA', '1234')");

	cout << cmd.execute() << endl;
      }
    }
    cout << db.disconnect() << endl;

  }
  catch (exception& ex) {
    cout << ex.what() << endl;
  }
  return 0;
}
