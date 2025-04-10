#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <linux/limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/fiemap.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>


#define SP_OPEN_DIR_ERR -1
#define SP_DIR_READ_ERR -2
#define SP_UNHANDLED -3
#define SP_GET_STAT_ERR -4
#define SP_OPEN_FILE_ERR -5
#define SP_GET_OFFSET_ERR -6
#define SP_ALLOC_ERR -7
#define SP_DIR_SEP "===================================================================================================\n"


int get_stat_mode( const char* full_path, mode_t* mode );
int get_disk_offset( const char* full_path, __u64* disk_off );
char get_file_type( mode_t mode );
int list_directory( const char* );


int main( int argc, char** argv ) {
    if( argc == 1  ) {
        printf("Please enter at least one directory name.\n");
        return 0;
    }
    for( int i = 1; i < argc; i++ ) {
        switch( list_directory(argv[i]) ) {
            case SP_OPEN_DIR_ERR:
                perror( "Error opening directory." );
                printf( "Error opening directory: %s\n", argv[i] );
                break;
            case SP_DIR_READ_ERR:
                perror( "Error reading directory." );
                printf( "Error reading directory: %s\n", argv[i] );
                break;
            case 0:
                break;
            default:
                perror( "Unhandled\n" );
                printf( "Unhandled\n" );
                return SP_UNHANDLED; 
                break;
        }
    }
    return 0;
}


int list_directory( const char *path_dir ) {
    DIR *dir = opendir( path_dir );
    if( dir == NULL ) {
        printf( SP_DIR_SEP );
        return SP_OPEN_DIR_ERR;
    }
    
    char buff[NAME_MAX] = {0};
    struct dirent* entry = {0};
    struct stat st = {0};
    int flag_first = 1;
    char full_path[PATH_MAX] = {0};

    while( 1 ) {
        if( (entry = readdir(dir)) == NULL ) {
            closedir(dir);
            return 0;
        }
        if( strcmp( entry->d_name, "." ) == 0 || strcmp( entry->d_name, ".." ) == 0  ) {
            continue;
        }
        
        if( flag_first == 1 ) {
            printf( SP_DIR_SEP );
            printf( "DIR: %s:\n", path_dir );
            flag_first = 0;
        }
        snprintf( full_path, sizeof(full_path), "%s/%s", path_dir, entry->d_name );

        mode_t mode_st = {0};
        if( get_stat_mode(full_path, &mode_st) == SP_GET_STAT_ERR ) {
            printf( "Error getting stat: %s\n", full_path );
            continue;
        }
        
        __u64 disk_off = {0};
        switch( get_disk_offset(full_path, &disk_off) ) {
            case SP_OPEN_FILE_ERR:
                printf( "Error opening file: %s\n", full_path );
                continue;
            case SP_GET_OFFSET_ERR:
                printf( "Error getting byte offset of extent on disk: %s\n", full_path );
                continue;
            case 0:
                break;
            default:
                perror( "Unhandled.\n" );
                break;
        }

        printf( "%-15llu %c %s\n", disk_off, get_file_type(mode_st), entry->d_name );
    }
    closedir( dir );
    return 0;
}

int get_stat_mode( const char* full_path, mode_t* mode ) {
    struct stat st = {0};
    if( fstatat(AT_FDCWD, full_path, &st, AT_SYMLINK_NOFOLLOW) == -1  ) {
        return SP_GET_STAT_ERR;
    }
    *mode = st.st_mode;
    return 0;
}

int get_disk_offset( const char* full_path, __u64* disk_off ) {
    int fd = open( full_path, O_RDONLY );
    if( fd == -1 ) {
        return SP_OPEN_FILE_ERR;
    }
    struct fiemap *fmap =( struct fiemap* )malloc(sizeof(struct fiemap) + sizeof(struct fiemap_extent));
    if( fmap == NULL ) {
        close(fd);
        return SP_ALLOC_ERR;
    }
    fmap->fm_start = 0;
    fmap->fm_length = 1;
    fmap->fm_flags = FIEMAP_FLAG_SYNC;
    fmap->fm_extent_count = 1;
    //FIBMAP
    //FS_IOC_FIEMAP
    if( ioctl(fd, FS_IOC_FIEMAP, fmap) == -1  ) {
        close(fd);
        return SP_GET_OFFSET_ERR;
    }
    *disk_off = fmap->fm_extents[0].fe_physical;
    free(fmap);
    close(fd);
    return 0;
}

char get_file_type( mode_t mode ) {
    if( S_ISREG(mode) ) return 'F';
    if( S_ISDIR(mode) ) return 'D';
    if( S_ISCHR(mode) ) return 'C';
    if( S_ISBLK(mode) ) return 'B';
    if( S_ISFIFO(mode)) return 'P';
    if( S_ISLNK(mode) ) return 'L';
    if( S_ISSOCK(mode) ) return 's';
    return '?';
}
