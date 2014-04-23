#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

void printUsageAndExit() {
    puts( "Usage: babycat <in_filename1> [<in_filename2> ...] <out_filename>" );
    exit( 1 );
}

int main(int argc, char **argv)
{
    if ( argc < 3 ) {
        printUsageAndExit();
    }

    argc--;
    int outFd = open( argv[ argc ], O_CREAT | O_TRUNC | O_WRONLY, 0644 );

    int i;
    for ( i = 1; i < argc; i++ ) {

        char *buffer[ BUFFER_SIZE ];

        int inFd = open( argv[ i ], O_RDONLY );

        int count;
        while ( ( count = read( inFd, buffer, BUFFER_SIZE ) ) > 0 ) {
            write( outFd, buffer, count );
        }

        close( inFd );
    }

    close( outFd );

	return 0;
}
