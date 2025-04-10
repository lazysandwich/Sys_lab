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


int cpf( char* source, char* destination ) {
	FILE* src;
	FILE* dst;
	
    if( !(src=fopen( source, "rb" )) ) return ERR_NO_SUCH_RESOURCE;
	
	if( !(dst=fopen( destination, "wb" )) ) {
		fclose(src);
		return ERR_NO_SUCH_RESOURCE;
	}

    char buffer[1024];
    size_t bytesRead;
	
    while( (bytesRead = fread( buffer, 1, sizeof(buffer), src )) > 0)  {
		if( ferror( src ) ) {
			fclose(src);
			fclose(dst);
			return ERR_NO_SUCH_RESOURCE;
		}
		fwrite(buffer, 1, bytesRead, dst); 
		if( ferror( src ) ) {
			fclose(src);
			fclose(dst);
			return ERR_NO_SUCH_RESOURCE;
		}
	}

    fclose(src);
    fclose(dst);
	
	return 0;
}

int main( int argc, char** argv ) {
	if( argc != 3 ) return ERR_OUT_OF_RANGE;
	
	int num = atoi( argv[1] );
	char* path = calloc(strlen(argv[2]), sizeof(char));
	strcpy(path, argv[2]);
	char* ext = NULL;
	
	for( ssize_t i = strlen(path)-1; i > -1; --i ) {
		if( path[i] == '.' ) {
			path[i] = '\0';
			ext = &path[i+1];
			break;
		}
	}
	
	size_t err = 0;
	size_t strsize = strlen(argv[2]) + 33;
	char* dest = calloc( strsize, sizeof(char) );
	
	for( size_t i = 0; i < num; ++i ) {
		snprintf(dest, strsize, "%s%ld.%s", path, i, ext);
		err = cpf( argv[2], dest );
		if( err ) break;
	}
	
	free( dest );
	free( path );
	return err;
}

