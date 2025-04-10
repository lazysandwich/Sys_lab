
#ifndef CONST_SP_1
#define CONST_SP_1

#define MAX_PIN 100000
#define MIN_PIN 0
#define PIN_CHAR_SIZE 6
#define LOGIN_SIZE 6
#define LIMIT_CHAR_SIZE 6
#define RECORD_SIZE LOGIN_SIZE + 1 + PIN_CHAR_SIZE + 1 + LIMIT_CHAR_SIZE + 1
#define PASSWORD_FILE_NAME "password.txt"
#define COMMAND_BUFF_SIZE 40
#define COMMAND_COUNT 5
#define MAX_LIMIT 999999
#define MIN_LIMIT -1
#define CONF_CODE 12345
#define EXIT_CHAR '-'


// Return codes
#define PASS_FILE_EMPUTY -1000
#define OPEN_FILE_ERR_R -999
#define READ_ERR_F -998
#define LSEEK_ERR -997
#define ZERO_LEN -996
#define TOO_LONG -995
#define INCORRECT_CHAR -994
#define READ_ERR_I -993
#define SPO_TWO_ARGS -992
#define OPEN_FILE_ERR_W -991
#define WRITE_ERR_F -990
#define PCC_WRONG_FORMAT -989
#define PCC_TOO_BIG_PIN -988
#define CPM_WRONG_PASS -987
#define CI_BUFF_OVERFLOW -986
#define CI_NEW_LINE -985
#define CS_COM_NOT_FOUND -984
#define LOGOUT_CODE -983
#define UNNECESSARY_ARGS -982
#define WRONG_ARGS -981
#define HC_FAILURE_CONVERSION -980
#define SSCANF_ERR -979
#define SC_INVALID_CODE -978
#define EXIT_CODE_SP -977

#endif
