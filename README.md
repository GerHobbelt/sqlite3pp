sqlite3pp_EZ
=========
 * Unicode support
 * Support for all new SQLite3 sub types
 * Supports correct fetching DB Date and Datetime types 
 * Template Table class
 * SQL Class Builder

This repository is a fork of sqlite3pp, and it includes additional code to add UNICODE support, a template Table class and a SQLite class builder.
The sqlite3pp::Table class along with the sqlite3pp::SQLiteClassBuilder class allows C++ developers to use type safe variables assocaited with the table column types.

This package contains all the files required to use SQLite3, SQLite3pp, and SQLite3pp_EZ.  Only minor modifications have been made to SQLite3 C code and SQLite3pp where needed for UNICODE support.  Then bulk of the sqlite3pp_EZ implementation is in sqlite3pp_EZ.h and sqlite3pp_EZ.cpp.

## Unicode support.
 * Example:
````cpp
sqlite3pp::setGlobalDB(L"Exchange_€To$_database.db");
sqlite3pp::Execute(_T("INSERT OR REPLACE INTO FileName VALUES ('") + sfileName + _T("', '") + sfileExt + _T("');"));
SQLiteClassBuilder	createMyClasses(
		"Exchange_€To$_database.db"
		, SQLiteClassBuilder::std_wstring_protected_members	// This option creates a class with std::wstring as the default string, and protected member variables.
		, ""							// Set this to a specific table or view, in which one class is created, or set it to empty to create a class for each table and view in the database.
	);
````
## Generic template Table class having the following features:
 * Type safety for table column fields, include additional types added by SQLite3.
   * Table column of type INTEGER has variable member of type Integer which is an alias for type int.
   * Table column of type TEXT has variable member of type Table::T_STR, which is alias for the type of string defined by the class (std:string, std::wstring, sqlite3pp::tstring, etc...)
   * ... REAL has variable member of type Real, which is an alias for double.
   * ... FLOAT has variable member of type Float, which is an alias for double.
   * ... BOOLEAN has variable member of type Bolean, which is an alias for bool.
   * ... TINYINT has variable member of type Tinyint, which is an alias for unisigned char.
   * ... DATE is of type Date which has a time_t member variable.
   * ... DATETIME is of type Datetime, which has an std::tm member variable.
   * ... BIGINT is typed to **long long int** which is equevalent to type __int64, and it's a more portable type
   * ... UNSIGNED BIG INT is typed to  **unsigned long long int**
   * The following types are also supported:
     * 	using Int = int;
     * 	using Int2 = int;
     * 	using Int8 = int;
     * 	using Smallint = short int;
     * 	using Mediumint = int;
     * 	sing Numeric = double;
     * 	using Decimal = double;
     * 	using DoublePrcsn = double;
     * 	using Double = double;
     * 	``using Blob = std::shared_ptr<std::vector<Tinyint> >;	// Stores binary data``
     * 	``using Clob = std::shared_ptr< std::vector<char> >;	// Stores strings that can have multiple NULL terminators``
     * 	using Nchar = **std::wstring**;
     * 	using Nvarchar = **std::wstring**;
     * 	using Character = std::string;
     * 	using Varchar = std::string;
 * Automatically populate the Table class with associated table
   * Example:
````cpp
sqlite3pp::Table<sql_table_MyTableFoo> tbl;  // One line create and populates tbl with all content of table named MyTableFoo.
````
 * Can iterate each row using (C++11) Range-based loop, C+ stye iteration, or C style iteration
   * Example:
````cpp
sqlite3pp::Table<sql_table_MyTableFoo> tbl;

for ( auto row : tbl )											// (C++11) Range-based loop
	std::cout << row.get_Wigets() << row.get_MyColumn() << row.get_AnotherFooColumn() << std::endl;

for (auto row = tbl.begin(); row != tbl.end(); ++row)							// C++ style iteration
	std::cout << row->get_Wigets() << row->get_MyColumn() << row->get_AnotherFooColumn() << std::endl;

for (int row = 0; row < tbl.size(); ++row)								// C style iteration
	std::cout << tbl[row].get_Wigets() << tbl[row].get_MyColumn() << tbl[row].get_AnotherFooColumn() << std::endl;
````
 * All sqlite3pp::Table objects can optionally share the same sqlite3pp::database, so the sqlite3pp::Table constructor doesn't have to take sqlite3pp::database input argument
## SQLiteClassBuilder
 * The SQLiteClassBuilder class can be used to create a class for each table or view in a SQLite database.
 * The class created by SQLiteClassBuilder is type safe IAW the column defined type.
 * The created class can be used with the template Table class. Example:  sqlite3pp::Table<MyBuilderCreatedClass> tbl;
## Common Usage
 * For most common requirements the default settings can be used. 
 * Unless otherwise specified, SQLiteClassBuilder uses predefined setting (std_string_protected_members) as the default settings.
## For advanced developers/usage:
 * When creating a class, SQLiteClassBuilder has the following options:
   * Set created class to have a specific default string type (std:string, std::wstring, sqlite3pp::tstring, etc...)
   * Specifiy a subfolder in which to create headers for each class
   * Specify a prefix and/or a postfix for the header file name.
   * Specify if column associated member variables are public or protected.
   * Specify if class has a get_* function for each column associated member variable.
   * Specify if class has a set_* function for each column associated member variable.
   * Specify if class gets an associated ostream (operator<<) function.
   * Specify if class gets created with comments
 * Developers can create a custom set of settings with class TblClassOptions, or use one of the 8 predefined settings.
 * There are 8 predefined settings for common choices.  The following are just 3 of the 8.
   * std_string_protected_members (default) 	= Creates a class that has member variables as protected, and it has get_* and set_* functions for each column associated variable. The default string type is std::string. String literals are define as-is. Example: foo = "some foo";
   * sql_tstring_minimal			= Creates a minimal class, having no comments, get_* functions, and no set_* functions. Member variables are public and the default string type is sqlite3pp::tstring. String literals are wrap with _T() macro. Example: foo = _T("some foo");
   * std_wstring_protected_members		= Creates protected member variables class with get_* and set_* functions. The default string type is std::wstring. String literals are prefixed with L. Example: foo = L"some foo";
## API's for single global sqlite3pp::database.
 * There are API's for using a single global sqlite3pp::database.
 * These API's are not associated with a class, and can be called directly.
   * setGlobalDB(db_filename)
   * Execute(SQL_statment)
   * Connect(dbname, flags, vfs)
   * Attach(dbname,dbname)
   * Detach(dbname)
   * Backup(dbname, destdb,destdbname, backup_handler, step_page)
   * GetDbErrMsg()
   * GetDbErrMsgW()
   * GetDbErrNo()
   * GetDbExtErrNo()
   * getGlobalDB()

The source code is configured in *.cpp and *.h.  There is no header only support.

# New Usage
Developer's code only needs to include header **"sqlite3pp_ez.h"**. An optional global DB can be created, so that the DB variable doens't have to be pass to every class.
````cpp
#include "sqlite3pp_ez.h"
using namespace sqlite3pp;
````

## database -- Setting global database variable
````cpp
setGlobalDB(_T("my.db"));
````

## Template Usage
Create a table/view query class which definds the table/view name and fields generically
````cpp
class FileExt  // A generic table/view query class
{
	using Str_DataType = sqlite3pp::tstring;  //Defind the string type (std::string, std::wstring, std::tstring)
	static Str_DataType GetTableName() { return _T("FileExt"); } // Specify the table or view name
	Str_DataType Ext; Str_DataType PrgLangName; Str_DataType Notes; // Declare each field the query will use, and defind the type (int or Str_DataType)
	static Str_DataType GetSelectNames() { return _T("Ext, PrgLangName, Notes"); } // Include each field name
	Str_DataType GetValues() { return _T("'") + Ext + _T("', '") + PrgLangName + _T("', '") + Notes + _T("'"); } // Include each field name
	template<class T> void GetStreamData( T q ) { q.getter() >> Ext >> PrgLangName >> Notes; } // Include each field name
};
````

With the above class a table class can be declared and automatically populated with the following code:
````cpp
Table<FileExt> MyAutoPopulatedTable;
for ( auto t : MyAutoPopulatedTable )
	std::wcout << t.GetValues() << std::endl;
````

The same above code can be used with any number of generic table/view query classes, because the details on handling the table/view is in the class.

The above class has a table called **FileExt** and it defines 3 fields (**Ext, PrgLangName, Notes**)



# Original Usage

## database
```cpp
sqlite3pp::database db("test.db");
db.execute("INSERT INTO contacts (name, phone) VALUES ('Mike', '555-1234')");
```

## command
```cpp
sqlite3pp::command cmd(
  db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
cmd.binder() << "Mike" << "555-1234";
cmd.execute();
```

```cpp
sqlite3pp::command cmd(db, "INSERT INTO contacts (name, phone) VALUES (?, ?)");
cmd.bind(1, "Mike", sqlite3pp::nocopy);
cmd.bind(2, "555-1234", sqlite3pp::nocopy);
cmd.execute();
```

```cpp
sqlite3pp::command cmd(
  db, "INSERT INTO contacts (name, phone) VALUES (?100, ?101)");
cmd.bind(100, "Mike", sqlite3pp::nocopy);
cmd.bind(101, "555-1234", sqlite3pp::nocopy);
cmd.execute();
```

```cpp
sqlite3pp::command cmd(
  db, "INSERT INTO contacts (name, phone) VALUES (:user, :phone)");
cmd.bind(":user", "Mike", sqlite3pp::nocopy);
cmd.bind(":phone", "555-1234", sqlite3pp::nocopy);
cmd.execute();
```

```cpp
sqlite3pp::command cmd(
  db,
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-0000');"
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-1111');"
  "INSERT INTO contacts (name, phone) VALUES (:user, '555-2222')");
cmd.bind(":user", "Mike", sqlite3pp::nocopy);
cmd.execute_all();
```

## transaction

```cpp
sqlite3pp::transaction xct(db);
{
  sqlite3pp::command cmd(
    db, "INSERT INTO contacts (name, phone) VALUES (:user, :phone)");
  cmd.bind(":user", "Mike", sqlite3pp::nocopy);
  cmd.bind(":phone", "555-1234", sqlite3pp::nocopy);
  cmd.execute();
}
xct.rollback();
```

## query

```cpp
sqlite3pp::query qry(db, "SELECT id, name, phone FROM contacts");

for (int i = 0; i < qry.column_count(); ++i) {
  cout << qry.column_name(i) << "\t";
}
```

```cpp
for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
  for (int j = 0; j < qry.column_count(); ++j) {
    cout << (*i).get<char const*>(j) << "\t";
  }
  cout << endl;
}
```

```cpp
for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
  int id;
  char const* name, *phone;
  std::tie(id, name, phone) =
    (*i).get_columns<int, char const*, char const*>(0, 1, 2);
  cout << id << "\t" << name << "\t" << phone << endl;
}
```

```cpp
for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
  string name, phone;
  (*i).getter() >> sqlite3pp::ignore >> name >> phone;
  cout << "\t" << name << "\t" << phone << endl;
}
```

```cpp
for (auto v : qry) {
  string name, phone;
  v.getter() >> sqlite3pp::ignore >> name >> phone;
  cout << "\t" << name << "\t" << phone << endl;
}
```

## attach

```cpp
sqlite3pp::database db("foods.db");
db.attach("test.db", "test");

sqlite3pp::query qry(
  db,
  "SELECT epi.* FROM episodes epi, test.contacts con WHERE epi.id = con.id");
```

## backup

```cpp
sqlite3pp::database db("test.db");
sqlite3pp::database backupdb("backup.db");

db.backup(backupdb);
```

```cpp
db.backup(
  backupdb,
  [](int pagecount, int remaining, int rc) {
    cout << pagecount << "/" << remaining << endl;
    if (rc == SQLITE_OK || rc == SQLITE_BUSY || rc == SQLITE_LOCKED) {
      // sleep or do nothing.
    }
  });
```

## callback

```cpp
struct rollback_handler
{
  void operator()() {
    cout << "handle_rollback" << endl;
  }
};

sqlite3pp::database db("test.db");

db.set_commit_handler([]{ cout << "handle_commit\n"; return 0; });
db.set_rollback_handler(rollback_handler());
```

```cpp
int handle_authorize(int evcode, char const* p1, char const* p2,
                     char const* dbname, char const* tvname) {
  cout << "handle_authorize(" << evcode << ")" << endl;
  return 0;
}

db.set_authorize_handler(&handle_authorize);
```

```cpp
struct handler
{
  handler() : cnt_(0) {}

  void handle_update(int opcode, char const* dbname,
                     char const* tablename, int64_t rowid) {
    cout << "handle_update(" << opcode << ", " << dbname << ", "
         << tablename << ", " << rowid << ") - " << cnt_++ << endl;
  }
  int cnt_;
};

using namespace std::placeholders;

db.set_update_handler(std::bind(&handler::handle_update, &h, _1, _2, _3, _4));
```

## function

```cpp
int test0()
{
  return 100;
}

sqlite3pp::database db("test.db");
sqlite3pp::ext::function func(db);

func.create<int ()>("test0", &test0);
```

```cpp
void test1(sqlite3pp::ext::context& ctx)
{
  ctx.result(200);
}

void test2(sqlite3pp::ext::context& ctx)
{
  string args = ctx.get<string>(0);
  ctx.result(args);
}

void test3(sqlite3pp::ext::context& ctx)
{
  ctx.result_copy(0);
}

func.create("test1", &test1);
func.create("test2", &test2, 1);
func.create("test3", &test3, 1);
```

```cpp
func.create<int ()>("test4", []{ return 500; });
```

```cpp
string test5(string const& value)
{
  return value;
}

string test6(string const& s1, string const& s2, string const& s3)
{
  return s1 + s2 + s3;
}

func.create<int (int)>("test5", [](int i){ return i + 10000; });
func.create<string (string, string, string)>("test6", &test6);
```

```cpp
sqlite3pp::query qry(
  db,
  "SELECT test0(), test1(), test2('x'), test3('y'), test4(), test5(10), "
  "test6('a', 'b', 'c')");
```

## aggregate

```cpp
void step(sqlite3pp::ext::context& c)
{
  int* sum = (int*) c.aggregate_data(sizeof(int));

  *sum += c.get<int>(0);
}
void finalize(sqlite3pp::ext::context& c)
{
  int* sum = (int*) c.aggregate_data(sizeof(int));
  c.result(*sum);
}

sqlite3pp::database db("foods.db");
sqlite3pp::ext::aggregate aggr(db);

aggr.create("aggr0", &step, &finalize);
```

```cpp
struct mycnt
{
  void step() {
    ++n_;
  }
  int finish() {
    return n_;
  }
  int n_;
};

aggr.create<mycnt>("aggr1");
```

```cpp
struct strcnt
{
  void step(string const& s) {
    s_ += s;
  }
  int finish() {
    return s_.size();
  }
  string s_;
};

struct plussum
{
  void step(int n1, int n2) {
    n_ += n1 + n2;
  }
  int finish() {
    return n_;
  }
  int n_;
};

aggr.create<strcnt, string>("aggr2");
aggr.create<plussum, int, int>("aggr3");
```

```cpp
sqlite3pp::query qry(
  db,
  "SELECT aggr0(id), aggr1(type_id), aggr2(name), aggr3(id, type_id) "
  "FROM foods");
```



# See also
* http://www.sqlite.org/
* https://github.com/iwongu/sqlite3pp
