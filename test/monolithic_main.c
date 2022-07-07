
#include "monolithic_examples.h"

#define USAGE_NAME   "sqlite3pp_tests"

#include "monolithic_main_internal_defs.h"

// declare your own monolith dispatch table:
MONOLITHIC_CMD_TABLE_START()
	{ "aggregate", { .f = sqlite3pp_aggregate_test_main } },
	{ "attach", { .f = sqlite3pp_attach_test_main } },
	{ "backup", { .f = sqlite3pp_backup_test_main } },
	{ "callback", { .f = sqlite3pp_callback_test_main } },
	{ "disconnect", { .f = sqlite3pp_disconnect_test_main } },
	{ "function", { .f = sqlite3pp_function_test_main } },
	{ "insert_all", { .f = sqlite3pp_insert_all_test_main } },
	{ "insert", { .f = sqlite3pp_insert_test_main } },
	{ "select", { .f = sqlite3pp_select_test_main } },
MONOLITHIC_CMD_TABLE_END();

#include "monolithic_main_tpl.h"
