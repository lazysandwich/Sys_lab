
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#include "sp_1.h"


int main(){
    printf("Hi\n");
    char login[LOGIN_SIZE + 1] = {0};
    int code = 0;
    int flag = 1;
    while (flag){
        printf("If you want to register, select \"R\".\nIf you want to log in, select \"L\"\n");
        char c = getchar();
        if (feof(stdin)) continue;
        else if (ferror(stdin)){
            code = READ_ERR_I;
            err_print(code);
            return code;
        }
        else if (c == 'L'){
            c = getchar();
            if (feof(stdin) || c == '\n') break;
            else if (ferror(stdin)){
                code = READ_ERR_I;
                err_print(code);
                return code;
            }
            else {
                while (1) {
                    c = getchar();
                    if (feof(stdin)) break;
                    else if (ferror(stdin)){
                        code = READ_ERR_I;
                        err_print(code);
                        return code;
                    }
                    else if (c == '\n') break;
                }
                
            }
        }
        else if (c == 'R') {
            c = getchar();
            if (ferror(stdin)){
                code = READ_ERR_I;
                err_print(code);
                return code;
            }
            else if (feof(stdin) || c == '\n') {
                code = registration(login, sizeof(login)/sizeof(char));
                if (code == 0){
                    flag = 0;
                }
                else {
                    err_print(code);
                    return code;
                }
            }
            else {
                while (1) {
                    c = getchar();
                    if (feof(stdin)) break;
                    else if (ferror(stdin)){
                        code = READ_ERR_I;
                        err_print(code);
                        return code;
                    }
                    else if (c == '\n') break;
                }
            }
        }
        else if (c == '\n') continue;
        else if (c == EXIT_CHAR){
            c = getchar();
            if (ferror(stdin)){
                code = READ_ERR_I;
                err_print(code);
                return code;
            }
            else if (feof(stdin) || c == '\n') return 0;
            while (c == ' ') {
                c = getchar();
                if (ferror(stdin)){
                    code = READ_ERR_I;
                    err_print(code);
                    return code;
                }
                else if (feof(stdin) || c == '\n') return 0;
            }
            while (1) {
                c = getchar();
                if (feof(stdin)) break;
                else if (ferror(stdin)){
                    code = READ_ERR_I;
                    err_print(code);
                    return code;
                }
                else if (c == '\n') break;
            }
        }
        else {
            c = getchar();
            if (ferror(stdin)){
                code = READ_ERR_I;
                err_print(code);
                return code;
            }
            else if (feof(stdin) || c == '\n') continue;
            else {
                while (1) {
                    c = getchar();
                    if (feof(stdin)) break;
                    else if (ferror(stdin)){
                        code = READ_ERR_I;
                        err_print(code);
                        return code;
                    }
                    else if (c == '\n') break;
                }
            }
        }
    }
    while (flag){
        code = authorization(login, sizeof(login)/sizeof(char));
        if (code == 0){
            flag = 0;
        }
        else {
            err_print(code);
            return code;
        }
    }
    flag = 1;
    while (flag){
        code = command_handler(login);
        if (code == 0) break;
        else if (code == LOGOUT_CODE){
            code = authorization(login, sizeof(login)/sizeof(char));
                if (code == 0){
                    code = 0;
                }
                else {
                    err_print(code);
                    return code;
                }
        }
        else{
            err_print(code);
            return code;
        }
    }
    return 0;
}
