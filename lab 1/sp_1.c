#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "sp_1.h"

int authorization( char* login_buff, int len ) {
    char login[LOGIN_SIZE + 2] = {0};
    char pin[PIN_CHAR_SIZE + 2] = {0};
    long code = 0;
    long offset = 0;
    while( 1 ) {
        printf( "To log in, please enter your login:\n" );
        code = login_pin_input( login, sizeof( login ) / sizeof( char ) );
        if ( code == READ_ERR_I )
            return READ_ERR_I;
        else if ( code == SPO_TWO_ARGS ) {
            printf( "Login must consist of 1 word.\n" );
            continue;
        }
        if ( login[0] == EXIT_CHAR && login[1] == '\0' )
            return EXIT_CODE_SP;
        code = login_check_corr( login );
        if ( code == ZERO_LEN ) {
            printf( "Login must contain at least one character.\n" );
            continue;
        }
        else if ( code == TOO_LONG ) {
            printf( "Login must be no longer than 6 characters.\n" );
            continue;
        }
        else if ( code == INCORRECT_CHAR ) {
            printf( "The login must contain only numbers and Latin letters.\n" );
            continue;
        }
        code = search_login( login, code );
        if ( code == OPEN_FILE_ERR_R || code == READ_ERR_F ) {
            return code;
        }
        else if ( code == PASS_FILE_EMPUTY ) {
            printf( "There is no such login.\n" );
            continue;
        }
        else {
            offset = code;
            printf( "Login accepted.\n" );
            break;
        }
    }
    while( 1 ) {
        printf( "Enter your PIN.\nIt can only contain decimals and represent a number from 0 to 100000.\n" );
        code = login_pin_input( pin, sizeof( pin ) / sizeof( char ) );
        if ( code == READ_ERR_I )
            return READ_ERR_I;
        else if ( code == SPO_TWO_ARGS ) {
            printf( "PIN must consist of 1 word.\n" );
            continue;
        }
        if ( pin[0] == EXIT_CHAR && pin[1] == '\0' )
            return EXIT_CODE_SP;
        code = pin_check_corr( pin );
        if ( code == ZERO_LEN ) {
            printf( "PIN must contain at least one number.\n" );
            continue;
        }
        else if ( code == PCC_TOO_BIG_PIN ) {
            printf( "PIN must not be greater than 100000.\n" );
            continue;
        }
        else if ( code == INCORRECT_CHAR ) {
            printf( "PIN must contain only numbers.\n" );
            continue;
        }
        else if ( code == PCC_WRONG_FORMAT ) {
            printf( "Invalid PIN code format.\n" );
            continue;
        }
        else {
            code = chechk_pass_match( pin, offset );
            if ( code == LSEEK_ERR || code == READ_ERR_F || code == OPEN_FILE_ERR_R ) {
                return code;
            }
            else if ( code == CPM_WRONG_PASS ) {
                printf( "The passwords do not match.\n" );
                continue;
            }
            printf( "PIN code accepted.\n" );
            break;
        }
    }
    strncpy( login_buff, login, len );
    return 0;
}

int registration( char* login_buff, int len ) {
    char login[LOGIN_SIZE + 2] = {0};
    char pin[PIN_CHAR_SIZE + 2] = {0};
    long code = 0;
    while ( 1 ) {
        printf( "Please enter your desired login.\n" );
        printf( "It should be no longer than 6 characters and consist of numbers and Latin letters.\n" );
        code = login_pin_input( login, sizeof( login ) / sizeof( char ) );
        if ( code == READ_ERR_I )
            return READ_ERR_I;
        else if ( code == SPO_TWO_ARGS ) {
            printf( "Login must consist of 1 word.\n" );
            continue;
        }
        if ( login[0] == EXIT_CHAR && login[1] == '\0' )
            return EXIT_CODE_SP;
        code = login_check_corr( login );
        if ( code == ZERO_LEN ) {
            printf( "Login must contain at least one character.\n" );
            continue;
        }
        else if ( code == TOO_LONG ) {
            printf( "Login must be no longer than 6 characters.\n" );
            continue;
        }
        else if ( code == INCORRECT_CHAR ) {
            printf( "The login must contain only numbers and Latin letters.\n" );
            continue;
        }
        code = search_login( login, code );
        if ( code == OPEN_FILE_ERR_R || code == READ_ERR_F ) {
            return code;
        }
        else if ( code == PASS_FILE_EMPUTY ) {
            printf( "Login accepted successfully.\n" );
            break;
        }
        else {
            printf( "The login is taken, please come up with another one.\n" );
        }
    }
    while( 1 ) {
        printf( "Enter your PIN.\nIt can only contain decimals and represent a number from 0 to 100000.\n" );
        code = login_pin_input( pin, sizeof( pin ) / sizeof( char ) );
        if ( code == READ_ERR_I )
            return READ_ERR_I;
        else if ( code == SPO_TWO_ARGS ) {
            printf( "PIN must consist of 1 word.\n" );
            continue;
        }
        if ( pin[0] == EXIT_CHAR && pin[1] == '\0' ) 
            return EXIT_CODE_SP;
        code = pin_check_corr( pin );
        if ( code == ZERO_LEN ) {
            printf( "PIN must contain at least one number.\n" );
            continue;
        }
        else if ( code == PCC_TOO_BIG_PIN ) {
            printf( "PIN must not be greater than 100000.\n" );
            continue;
        }
        else if ( code == INCORRECT_CHAR ) {
            printf( "PIN must contain only numbers.\n" );
            continue;
        }
        else if ( code == PCC_WRONG_FORMAT ) {
            printf( "Invalid PIN code format.\n" );
            continue;
        }
        else {
            printf( "PIN code accepted.\n" );
            break;
        }
    }
    code = add_user_to_passw_file( login, pin );
    if ( code != 0 ) 
        return code;
    strncpy( login_buff, login, len );
    return 0;
}

int login_check_corr( const char* login ) {
    int len = strlen( login );
    if ( len == 0 ) 
        return ZERO_LEN;
    if ( len > LOGIN_SIZE ) 
        return TOO_LONG;
    for ( int i = 0; i < len; i++ ) {
        if ( !( isalpha( login[i] ) || isdigit( login[i] ) ) ) return INCORRECT_CHAR;
    }
    return len;
}

long search_login( const char* login, int len ) {
    char buff[RECORD_SIZE] = {0};
    off_t offset = 0;
    int passwords = open( PASSWORD_FILE_NAME, O_RDONLY | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO );
    if ( passwords == -1 ) 
        return OPEN_FILE_ERR_R;
    while ( 1 ) {
        offset = lseek( passwords, 0, SEEK_CUR );
        if ( offset == -1 ) {
            close( passwords );
            return LSEEK_ERR;
        }
        int code = read( passwords, buff, sizeof( buff ) );
        if ( code == 0 ) {
            close( passwords );
            return PASS_FILE_EMPUTY;
        }
        else if ( code == -1 ) {
            close( passwords );
            return READ_ERR_F;
        }
        int flag = 1;
        for ( int i = 0; i < len; i++ ) {
            if ( login[i] != buff[i] ) {
                flag = 0;
                break;
            }
        }
        if ( flag && ( buff[len] == '-' || buff[len] == ':' ) ) {
            close( passwords );
            return ( long )offset;
        }
    }
}

int login_pin_input( char* buff, int len ) {
    buff[len - 1] = '\0';
    char *ptr = buff;
    char c = '\0';
    int flag = 1;  // 1- start input
    for ( int i = 0; i < len - 1; ++i ) {
        c = getc( stdin );
        if ( feof( stdin ) ) {
            *ptr = '\0';
            return 0;
        }
        else if ( ferror( stdin ) ) 
            return READ_ERR_I;
        while ( flag && c == ' ' ) {
            c = getc( stdin );
            if ( feof( stdin ) ) {
                *ptr = '\0';
                return 0;
            }
            else if ( ferror( stdin ) ) 
                return READ_ERR_I;
        }
        flag = 0;
        if ( !flag && c == ' ' ) {
            while ( !feof( stdin ) ) {
                c = getc( stdin );
                if ( ferror( stdin ) ) 
                    return READ_ERR_I;
                else if ( c != ' ' ) 
                    break;
            }
            *ptr = '\0';
            if ( c == '\n' ) 
                return 0; 
            else {
                flag = SPO_TWO_ARGS;
                break;
            };
        }
        if ( c == '\n' ) {
            *ptr = '\0';
            return 0;
        }
        *ptr++ = c;
    }
    while ( 1 ) {
        c = getc( stdin );
        if ( feof( stdin ) ) 
            return 0;
        else if ( ferror( stdin ) ) 
            return READ_ERR_I;
        if ( c == '\n' ) 
            return flag;
        else 
            flag = SPO_TWO_ARGS;
    }
}

long pin_check_corr( const char* pin ) {
    int len = strlen( pin );
    long pin_n = 0;
    if ( len == 0 ) 
        return ZERO_LEN;
    if ( pin[0] == '0' && isdigit( pin[1] ) ) 
        return PCC_WRONG_FORMAT;
    for ( int i = 0; i < len; ++i ) {
        if ( !isdigit( pin[i] ) ) 
            return INCORRECT_CHAR;
        pin_n = pin_n * 10 + pin[i] - '0';
    }
    if ( pin_n > MAX_PIN ) 
        return PCC_TOO_BIG_PIN;
    return pin_n;
}

int add_user_to_passw_file( const char* login, const char* pin ) {
    FILE* passwords = fopen( PASSWORD_FILE_NAME, "a" );
    if ( passwords == NULL ) {
        return OPEN_FILE_ERR_W;
    }
    const char pad[] = "------";
    int padLen_log = LOGIN_SIZE - strlen( login );
    int padLen_pin = PIN_CHAR_SIZE - strlen( pin );
    if ( fprintf( passwords, "%s%.*s:%s%.*s:%s%.*s\n", login, padLen_log, pad,
        pin, padLen_pin, pad, "-1", LIMIT_CHAR_SIZE - 2, pad ) < 0 ) {
            fclose( passwords );
            return WRITE_ERR_F;
    }
    fclose( passwords );
    return 0;
}

int chechk_pass_match( const char* pin, long offset ) {
    char buff[RECORD_SIZE];
    int passwords = open( PASSWORD_FILE_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
    if ( passwords == -1 ) 
        return OPEN_FILE_ERR_R;
    if ( lseek( passwords, offset, SEEK_SET ) == -1 ) {
        close( passwords );
        return LSEEK_ERR;
    }
    int code = read( passwords, buff, sizeof( buff ) );
    if ( code == 0 || code == -1 ) {
        close( passwords );
        return READ_ERR_F;
    }
    for( char* ptr = buff + LOGIN_SIZE + 1; ptr - buff < RECORD_SIZE - 1; ++ptr ) {
        if ( *ptr == '-' ) 
            break;
        else if ( pin[ptr - buff - LOGIN_SIZE - 1] != *ptr ) {
            close( passwords );
            return CPM_WRONG_PASS;
        }
    }
    close( passwords );
    return 0;
}

int command_handler( const char* login ) {
    char buff[COMMAND_BUFF_SIZE] = {0};
    const char com_list[COMMAND_COUNT][10] = {
        "Time", "Date", "Howmuch", "Logout", "Sanctions"
    };
    long offset = search_login( login, strlen( login ) );
    if ( offset < 0 ) 
        return offset;

    int limit = 0;
    limit = get_limit( offset );
    int limit_flag = 1;
    if ( limit == LSEEK_ERR || limit == READ_ERR_F
        || limit == SSCANF_ERR || limit == OPEN_FILE_ERR_R ) 
            return limit;
    else if ( limit == 0 ) {
        printf( "Your command limit is 0\n" );
        return LOGOUT_CODE;
    }
    else if ( limit > 0 ) {
        printf( "Your command limit is %d\n", limit );
    }
    else 
        limit_flag = 0;
    while ( !limit_flag || limit > 0 ) {
        printf( "%s>", login );
        int code = command_input( buff, COMMAND_BUFF_SIZE );
        if ( code == CI_NEW_LINE ) 
            continue;
        else if ( code == READ_ERR_I ) 
            return code;
        else if ( code == CI_BUFF_OVERFLOW ) {
            printf( "\nBuffer overflow. Some values were discarded.\n" );
            printf( "The resulting command was recognized as:\n%s\n", buff );
        }
        code = command_selector( buff, com_list, COMMAND_COUNT );
        if ( code == CS_COM_NOT_FOUND ) {
            printf( "Command not found.\n" );
            continue;
        }
        code = command_caller( code, buff );
        switch ( code ) {
            case 0:
                break;
            case UNNECESSARY_ARGS:
                printf( "Unnecessary arguments.\n" );
                break;
            case WRONG_ARGS:
            case PASS_FILE_EMPUTY:
                printf( "Wrong args.\n" );
                break;
            case HC_FAILURE_CONVERSION:
                printf( "Failed to convert date.\n" );
                break;
            case SC_INVALID_CODE:
                printf( "Invalid code.\n" );
                break;
            case READ_ERR_F:
            case LSEEK_ERR:
            case OPEN_FILE_ERR_R:
            case OPEN_FILE_ERR_W:
            case WRITE_ERR_F:
            case LOGOUT_CODE:
                return code;
                break;
            default:
                printf( "Unhandled\n" );
                break;
        }
        --limit;
    }
    printf( "The limit has been reached.\n" );
    return 0;
}

int command_selector( const char* com_buf, const char com_list[][10], int count ) {
    int com_num = CS_COM_NOT_FOUND;
    for ( int i = 0; i < count; ++i ) {
        int j = 0;
        while ( com_list[i][j] != '\0' && com_buf[j] != '\0' ) {
            if ( com_list[i][j] != com_buf[j] ) break;
            ++j;
        }
        if ( com_list[i][j] == '\0' ) {
            com_num = i;
            break;
        }
    }
    return com_num;
}

int command_input( char* com_buf, int len ) {
    com_buf[len - 1] = '\0';
    char c = '\0';
    int flag = 1;  // start input indicator
    for ( int i = 0; i < len - 1; ++i ) {
        c = getc( stdin );
        if ( feof( stdin ) ) {
            com_buf[i] = '\0';
            return 0;
        }
        else if ( ferror( stdin ) ) 
            return READ_ERR_I;
        if ( flag == 1 && c == ' ' ) {
            while ( c == ' ' ) {
                c = getc( stdin );
                if ( feof( stdin ) ) {
                    com_buf[i] = '\0';
                    return 0;
                }
                else if ( ferror( stdin ) ) 
                    return READ_ERR_I;
            }
        }
        else if ( flag == 1 && ( c == '\n' ) ) {
            return CI_NEW_LINE;
        }
        flag = ( flag >> 1 ) << 1;
        if ( c == ' ' && flag == 2 ) {
            while ( c == ' ' ) {
                c = getc( stdin );
                if ( feof( stdin ) ) {
                    com_buf[i] = '\0';
                    return 0;
                }
                else if ( ferror( stdin ) ) 
                    return READ_ERR_I;
            }
        }
        else if ( c == ' ' ) {
            flag = 2;  // indicator ' '
        }
        else if ( c == '\n' ) {
            com_buf[i] = '\0';
            return 0;
        }
        else flag = 0;
        com_buf[i] = c;

    }
    while ( 1 ) {
        c = getc( stdin );
        if ( feof( stdin ) ) 
            return CI_BUFF_OVERFLOW;
        else if ( ferror( stdin ) ) 
            return READ_ERR_I;
        if ( c == '\n' ) 
            return CI_BUFF_OVERFLOW;
    }
}

int command_caller( int command, const char* com_buf ) {
    switch ( command ) {
        case 0:  // Time
            if ( com_buf[4] != '\0' ) 
                return UNNECESSARY_ARGS;
            return time_com();
            break;
        case 1:  // Date
            if ( com_buf[4] != '\0' )  
                return UNNECESSARY_ARGS;
            return date_com();
            break;
        case 2:  // Howmuch
            return howmuch_com( com_buf + 7 );
            break;
        case 3:  // Logout
            if ( com_buf[6] != '\0' ) 
                return UNNECESSARY_ARGS;
            return LOGOUT_CODE;
            break;
        case 4:  // Sanctions
            return sanctions_com( com_buf + 9 );
            break;  
        default:
            return 0;
            break;
    }
}

int time_com() {
    time_t t = time( 0 );
    char* str = ctime( &t );
    printf( "%.8s\n", str + 11 );
    return 0;
}

int date_com() {
    time_t t = time( 0 );
    struct tm* local = localtime( &t );
    printf( "%02d:%02d:%04d\n", local->tm_mday, local->tm_mon + 1, local->tm_year + 1900 );
    return 0;
}

int howmuch_com( const char* args ) {
    if ( *args != ' ' ) 
        return WRONG_ARGS;
    const char* ptr = args + 1;
    while ( *ptr != '\0' ) {
        if ( *ptr == '-' && *( ptr + 1 ) != '\0' ) {
            if ( *( ptr + 2 ) != '\0' ) 
                return WRONG_ARGS;
            else 
                break;
        }
        ++ptr;
    }
    int year = 0;
    int month = 0;
    int day = 0;
    char flag = '\0';
    if ( sscanf( args + 1, "%d:%d:%d -%c", &day, &month, &year, &flag ) != 4 ) 
        return WRONG_ARGS;
    if ( ( flag != 'm' && flag != 'h' && flag != 'y' && flag != 's' )
        || ( !is_valid_date( day, month, year ) ) ) 
            return WRONG_ARGS;
    struct tm user_input = {0};
    user_input.tm_year = year - 1900;
    user_input.tm_mon = month - 1;
    user_input.tm_mday = day;
    user_input.tm_isdst = -1;
    time_t user_time, now;
    user_time = mktime( &user_input );
    if ( user_time == -1 ) {
        return HC_FAILURE_CONVERSION;
    }
    time( &now );
    double sec = difftime( now, user_time );
    if ( flag == 's' ) {
        printf( "Time in seconds: %.2f\n", sec );
    }
    else if ( flag == 'm' ) {
        printf( "Time in minutes: %.2f\n", sec/60.0 );
    }
    else if ( flag == 'h' ) {
        printf( "Time in hours: %.2f\n", sec/60.0/60.0 );
    }
    else{
        printf( "Time in years: %.2f\n", sec/60.0/60.0/24.0/365.24 );
    }
    return 0;
}

int is_valid_date( int day, int month, int year ) {
    if ( year < 1 || month < 1 || month > 12 || day < 1 ) 
        return 0;
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ( ( year % 4 == 0 && year % 100 != 0 ) || ( year % 400 == 0 ) ) {
        days_in_month[1] = 29;
    }
    return day <= days_in_month[month - 1];
}

int sanctions_com( const char* args ) {
    if ( *args != ' ' ) 
        return WRONG_ARGS;
    const char* ptr = args + 1;
    int count = 0;
    while ( *ptr != '\0' ) {
        if ( *ptr != ' ' && *( ptr -1 ) == ' ' ) ++count;
        else if ( count == 2 && !isdigit( *ptr ) ) 
            return WRONG_ARGS;
        else if ( count == 3 ) 
            return WRONG_ARGS;
        ++ptr;
    }
    ptr = args + 1;
    printf( "Enter the secret code:\n" );
    int code = 0;
    scanf( "%d", &code );
    char c = getc( stdin );
    if ( !feof( stdin ) && c != '\n' ) {
        while ( c != '\n' ) {
            c = getc( stdin );
            if ( !feof( stdin ) ) 
                return SC_INVALID_CODE;
            else if ( ferror( stdin ) ) 
                return READ_ERR_I;  
        }   
    }
    else if ( ferror( stdin ) ) 
        return READ_ERR_I;    
    if ( code != CONF_CODE ) {
        return SC_INVALID_CODE;
    }
    char login[LOGIN_SIZE + 1] = {0};
    while ( *ptr != ' ' ) {
        if ( ptr - args - 1 > LOGIN_SIZE ) 
            return WRONG_ARGS;
        if ( isalpha( *ptr ) || isdigit( *ptr ) ) 
            login[ptr - args - 1] = *ptr;
        else 
            return WRONG_ARGS;
        ++ptr;
    }
    long offset = search_login( login, strlen( login ) );
    if ( offset < 0 ) 
        return offset;
    int limit = 0;
    if( sscanf( ptr + 1, "%d", &limit ) != 1 ) 
        return WRONG_ARGS;
    if ( limit < MIN_LIMIT || limit > MAX_LIMIT ) 
        return WRONG_ARGS;
    int passwords = open( PASSWORD_FILE_NAME, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO );
    if ( passwords == -1 ) 
        return OPEN_FILE_ERR_W;
    if ( lseek( passwords, offset + LOGIN_SIZE + 1 + PIN_CHAR_SIZE + 1, SEEK_SET ) == -1 ) {
        close( passwords );
        return LSEEK_ERR;
    }
    char limit_str[LIMIT_CHAR_SIZE + 1] = {0};
    sprintf( limit_str, "%d", limit );
    limit_str[LIMIT_CHAR_SIZE] = '\n';
    for ( int i = 0; i < LIMIT_CHAR_SIZE + 1; ++i ) {
        if ( limit_str[i] == '\0' ) limit_str[i] = '-'; 
    }
    if ( write( passwords, limit_str, sizeof( limit_str ) ) == -1 ) {
        close( passwords );
        return WRITE_ERR_F;
    }
    printf( "Add sanction to %s. New limit: %d\n", login, limit );
    close( passwords );
    return 0;
}

int get_limit( long offset ) {
    char buff[RECORD_SIZE];
    int passwords = open( PASSWORD_FILE_NAME, O_RDONLY, S_IRWXU | S_IRWXG | S_IRWXO );
    if ( passwords == -1 ) 
        return OPEN_FILE_ERR_R;
    if ( lseek( passwords, offset, SEEK_SET ) == -1 ) {
        close( passwords );
        return LSEEK_ERR;
    }
    int code = read( passwords, buff, sizeof( buff ) );
    if ( code == 0 || code == -1 ) {
        close( passwords );
        return READ_ERR_F;
    }
    char *ptr = buff + LOGIN_SIZE + 1 + PIN_CHAR_SIZE + 1;
    int limit = 0;
    if ( sscanf( ptr, "%d\n", &limit ) != 1 ) {
        close( passwords );
        return SSCANF_ERR;
    }
    close( passwords );
    return limit;
}

int err_print( int code ) {
    switch ( code ) {
        case SSCANF_ERR:
            perror( "File parsing error.\n" );
            printf( "File parsing error.\n" );
            break;
        case READ_ERR_I:
            perror( "Error reading from IO stream.\n" );
            printf( "Error reading from IO stream.\n" );
            break;
        case OPEN_FILE_ERR_R:
            perror( "Error opening file with passwords.\n" );
            printf( "Error opening file with passwords.\n" );
            break;
        case READ_ERR_F:
            perror( "Error reading from password file.\n" );
            printf( "Error reading from password file.\n" );
            break;
        case OPEN_FILE_ERR_W:
            perror( "Error opening file with passwords for writing.\n" );
            printf( "Error opening file with passwords for writing.\n" );
            break;
        case WRITE_ERR_F:
            perror( "Error writing to password file.\n" );
            printf( "Error writing to password file.\n" );
            break;
        case LSEEK_ERR:
            perror( "Error changing offset in password file.\n" );
            printf( "Error changing offset in password file.\n" );
            break;
        default:
            perror( "Unhandled\n" );
            printf( "Unhandled\n" );
            break;
    }
    return 0;
}
