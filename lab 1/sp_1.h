
#ifndef SP_1
#define SP_1

#include "const_sp_1.h"

int err_print(int code);

int login_pin_input(char* buff, int len);
int login_check_corr(const char* login);
long search_login(const char* login, int len);
long pin_check_corr(const char* pin);
int chechk_pass_match(const char* pin, long offset);

int add_user_to_passw_file(const char* login, const char* pin);
int registration(char* login_buff, int len);

int authorization(char* login_buff, int len);

int is_valid_date(int day, int month, int year);
int time_com();
int date_com();
int howmuch_com(const char* args);
int sanctions_com(const char* args);

int get_limit(long offset);
int command_input(char* com_buf, int len);
int command_selector(const char* com_buf, const char com_list[][10], int count);
int command_caller(int command, const char* com_buf);
int command_handler(const char* login);

#endif
