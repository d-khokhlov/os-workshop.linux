#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void printUsageAndExit() {
    puts( "Usage: babywc [-l|-c] <filename>" );
    exit( 1 );
}

int main(int argc, char **argv)
{
    char mode = 'l';
    char *filename = NULL;

    int argIndex;
    for ( argIndex = 1; argIndex < argc; argIndex++ ) {

       if ( argv[ argIndex ][ 0 ] == '-' ) {

            int optionIndex = 1;
            while ( argv[ argIndex ][ optionIndex ] != 0 ) {
                switch ( argv[ argIndex ][ optionIndex ] ) {
                    case 'c':
                        mode = 'c';
                        break;
                    case 'l':
                        mode = 'l';
                        break;
                    default:
                        printUsageAndExit();
                }
                optionIndex++;
            }

        } else {
            filename = argv[ argIndex ];
            break;
        }
    }

    if ( filename == NULL ) {
        printUsageAndExit();
    }

    int fd = open( filename, O_RDONLY );

    char input;
    int count = 0;
    while ( read( fd, &input, 1 ) == 1 ) {
        if ( mode == 'c' || input == '\n' ) {
            count++;
        }
    }

    close( fd );

	printf( "%i\n", count );
	return 0;
}
