#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

void printUsageAndExit() {
    puts( "Usage: babytail [-c <bytes_count>] <filename>" );
    exit( 1 );
}

int main(int argc, char **argv)
{
    if ( argc < 2 ) {
        printUsageAndExit();
    }

    char *filename;
    int count = 10;
    if ( strcmp( argv[ 1 ], "-c" ) == 0 ) {
        if ( argc < 4 ) {
            printUsageAndExit();
        }
        if ( sscanf( argv[ 2 ], "%i", &count ) != 1 ) {
            printUsageAndExit();
        }
        filename = argv[ 3 ];
    } else {
        filename = argv[ 1 ];
    }

    int fd = open( filename, O_RDONLY );

    lseek( fd, -count, SEEK_END );

    char buffer[ BUFFER_SIZE ];
    int readCount = read( fd, buffer, count );

    close( fd );

    write( 1, buffer, readCount );

	return 0;
}
