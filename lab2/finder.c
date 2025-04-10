#include <stddef.h>

#include <stdio.h>

#include <stdint.h>

#include <limits.h>

#include <math.h>

#include <stdlib.h>

#include <errno.h>

#include <string.h>

#include <unistd.h>

#include <sys/types.h>

#include <sys/wait.h>



#define ERR_NO_SUCH_FLAG 256

#define ERR_OUT_OF_RANGE 255

#define ERR_OUT_OF_MEMORY 254

#define ERR_NO_SUCH_RESOURCE 253



#define TXT_ERR_GEN "Program has encountered err no. %d and shall cease execution now. Thank you!\n"



int sif(const char *filename, const char *search_str) {

    FILE *file = fopen(filename, "r");

    if (file == NULL) return ERR_NO_SUCH_RESOURCE;

	

    char line[1024];

	size_t read;

    while( (read=fread(line, 1, sizeof(line) - 1, file )) > 0 ) {

		if( ferror( file ) ) return ERR_NO_SUCH_RESOURCE;

        if (strstr(line, search_str) != NULL) {

            fclose(file);

            return 0;

        }else{

			fseek( file, -(read < sizeof(line) ? read - 1 : sizeof(line)-1), SEEK_CUR );

		}

    }



    fclose(file);

    return -1;

}



int main( int argc, char** argv ) {

	if( argc != 3 ) return ERR_OUT_OF_RANGE;

	

	char* path = argv[2];

	char* str = argv[1];

		

	

	int err = sif( path, str );

	

	if( err > 0 ) return err;

	if( !err ) printf("Found - %s\n", path );

	return 0;

}

