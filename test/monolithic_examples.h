
#pragma once

#if defined(BUILD_MONOLITHIC)

#ifdef __cplusplus
extern "C" {
#endif

int sqlite3pp_aggregate_test_main(void);
int sqlite3pp_attach_test_main(void);
int sqlite3pp_backup_test_main(void);
int sqlite3pp_callback_test_main(void);
int sqlite3pp_disconnect_test_main(void);
int sqlite3pp_function_test_main(void);
int sqlite3pp_insert_all_test_main(void);
int sqlite3pp_insert_test_main(void);
int sqlite3pp_select_test_main(void);

#ifdef __cplusplus
}
#endif

#endif
