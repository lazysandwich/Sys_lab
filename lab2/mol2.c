#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ERR_NO_SUCH_FLAG 256
#define ERR_OUT_OF_RANGE 255
#define ERR_OUT_OF_MEMORY 254
#define ERR_NO_SUCH_RESOURCE 253

#define TXT_ERR_GEN "Program has encountered err no. %d and shall cease execution now. Thank you!\n"

struct FlagHandle {
	char flag[16];
	int (*handle)(int, char**);
};

// Inverted result: 0 for A != B, >0 otherwise
int strcmpno( char* A, char* B ) {
	int no = 0;
	for( 0; *A; ++A ) {
		if( *A == 'N' ) {
			if( *B < '0' || *B > '9' ) return -1;
			
			for( 0; *B >= '0' && *B <= '9'; ++B ) {
				if( INT_MAX / 10 < no ) return -2;
				if( no * 10 > INT_MAX - (*B - '0') ) return -3;
				no = no * 10 + (*B - '0');
			}
		}else if( *A != *B ) {
			return -4;
		}else{
			++B;
		}
	}
	if( *A != *B ) return -5;
	
	return no;
}

int processHandles( struct FlagHandle* handles, int argc, int argn, char** argv ) {
	while( handles->handle != NULL ) {
		if( strcmpno(handles->flag, argv[argn]) >= 0 ) return handles->handle( argc, argv );
		++handles;
	}
	return ERR_NO_SUCH_FLAG;
}

void printMemReg(void *ptr, size_t size, size_t bits) {
    unsigned char *byte_ptr = (unsigned char *)ptr;
    
    for( size_t i = 0; i < size; i++ ) {
        unsigned char byte = byte_ptr[i];
    
        for( int j = ((7 < bits) ? 7 : bits); j >= 0; j-- ) {
            putchar( (byte & (1 << j)) ? '1' : '0' );
        }
    }
	putchar('\n');
}

#include <stdio.h>

char* toHex( size_t num, char* trg, size_t len ) {
    char digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int index = 0;
	
	trg[--len] = '\0';

    while( num > 0 ) {
        trg[--len] = digits[num % 16];
        num /= 16;
    }
	
	return &trg[len];
}

int isHexTemplate( char* template, char* hex ) {
	do {
		switch( *template ) {
			case '?':
				break;
			case '*':
				while( *(hex+1) != *(template+1) && *(hex+1) != '\0' )
					++hex;
				break;
			default:
				if( *hex != *template ) return 0;
				break;
		}
		
		++hex;
		++template;
	} while( *template && *hex );
	
	if( *template != *hex ) return 0; // Did these values both reach \0 \0?
	
	return 1;
}


int handleXorN( int argc, char** argv ) {
	int value = strcmpno( "xorN", argv[argc-1] );
	if( value < 2 || value > 8 ) return ERR_OUT_OF_RANGE;
	
	size_t bytes = ceil(pow(2, value) / 8);
	char* storage = calloc( bytes, 1 );
	if( !storage ) return ERR_OUT_OF_MEMORY;
	
	for( size_t i = 1; i < argc - 1; i++ ) {
		FILE* handle = fopen( argv[i], "r" );
		if( !handle ){ free( storage ); fclose( handle ); return ERR_NO_SUCH_RESOURCE; }
		if( ferror( handle ) ) { free( storage ); fclose( handle ); return ERR_NO_SUCH_RESOURCE; }
		
		int c;
		while( (c=fgetc( handle )) != EOF ) {
			if( ferror( handle ) ) { free( storage ); return ERR_NO_SUCH_RESOURCE; }
			storage[i % bytes] ^= (char)(c);
			
			if( value == 2 ) storage[i % bytes] ^= storage[i % bytes] >> 4;
		}
		printMemReg( storage, bytes, pow(2, value) - 1 );
		fclose( handle );
	}
	
	free( storage );
	return 0;
}



int handleMask( int argc, char** argv ) {
	char data[10] = "\0\0\0\0\0\0\0\0\0\0";
	size_t no = 0;
	
	for( size_t i = 1; i < argc - 2; i++ ) {
		FILE* handle = fopen( argv[i], "r" );
		if( ferror( handle ) ) { fclose( handle ); return ERR_NO_SUCH_RESOURCE; }
		if( !handle ) { fclose( handle ); return ERR_NO_SUCH_RESOURCE; }
		
		int read;
		uint32_t c;
		while( (read=fread( &c, 4, 1, handle )) != 0 ) {
			if( ferror( handle ) ) return ERR_NO_SUCH_RESOURCE;
			char* hex = toHex( c, data, 10 );
			
			if( isHexTemplate(argv[argc - 1], hex) ) {
				no++;
			}else{
				fseek( handle, -(read-1 < 3 ? read - 1 : 3), SEEK_CUR );
			}
		}
		
		printf("%ld\n", no );
		fclose( handle );
	}
	
	return 0;
}
int handleCopyN( int argc, char** argv ) {
	int value = strcmpno( "copyN", argv[argc-1] );
	
	int err = 0;
	int newerr = 0;
	for( size_t argn = 1; argn < argc - 1; ++argn ) {
		pid_t pid = fork();
		switch( pid ) {
			case -1:
				err = ERR_NO_SUCH_RESOURCE;
				break;
			case 0:
				char cwd[1024];
				char out[1536];
				char txt[32];
				
				getcwd(cwd, sizeof(cwd));
				sprintf( out, "%s/copyifier.elf", cwd );
				sprintf( txt, "%d", value );
				char* args[] = {out, txt, argv[argn], NULL};
				if( execv( out, args ) == -1 ) return ERR_NO_SUCH_RESOURCE;
				break;
			default:
				break;
		}
		
		if( err ) break;
	}
	pid_t wpid;
	while ( (wpid = wait(&newerr)) > 0 ) err |= newerr;
	return err;
}

int handleFind( int argc, char** argv ) {
	
	int err = 0;
	int newerr = 0;
	for( size_t argn = 1; argn < argc - 2; ++argn ) {
		pid_t pid = fork();
		switch( pid ) {
			case -1:
				err = ERR_NO_SUCH_RESOURCE;
				break;
			case 0:
				
				char cwd[1024];
				char out[1536];
				char txt[32];
				
				getcwd(cwd, sizeof(cwd));
				sprintf( out, "%s/finder.elf", cwd );
				char* args[] = {out, argv[argc-1], argv[argn], NULL};
				
				if( execv( out, args ) == -1 ) return ERR_NO_SUCH_RESOURCE;
				break;
			default:
				break;
		}
		if( err ) break;
	}
	pid_t wpid;
	while ( (wpid = wait(&newerr)) > 0 ) err |= newerr;
	return err;
}


int main( int argc, char** argv ) {
	struct FlagHandle handles[] = {
		(struct FlagHandle){ .flag="xorN", .handle=handleXorN },
		(struct FlagHandle){ .flag="mask", .handle=handleMask },
		(struct FlagHandle){ .flag="copyN", .handle=handleCopyN },
		(struct FlagHandle){ .flag="find", .handle=handleFind },
		(struct FlagHandle){ .flag='\0', .handle=NULL } // ALWAYS NULL-Terminated!!!!
	};
	
	int err = processHandles( handles, argc, argc - 1, argv );
	if( err == ERR_NO_SUCH_FLAG && argc > 2 ) err = processHandles( handles, argc, argc - 2, argv );
	
	if( err ) printf( TXT_ERR_GEN, err );
	return err;
}
