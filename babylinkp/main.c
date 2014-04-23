#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 100

int main(int argc, char **argv)
{
    char *target = argv[ 1 ];
    char *source = argv[ 2 ];

    char targetPath[ BUFFER_SIZE ] = "";
    char isRelative = 1;

    if ( target[ 0 ] == '/' ) {
        isRelative = 0;
    }

    if ( source[ 0 ] == '/' ) {
        if ( isRelative ) {
            getcwd( targetPath, BUFFER_SIZE );
            strcat( targetPath, "/" );
            isRelative = 0;
        }
        chdir( "/" );
    }

    char *name = strtok( source, "/" );
    char *nameNext;

    while( ( nameNext = strtok( NULL, "/" ) ) != NULL ) {
        mkdir( name, 0777 );
        chdir( name );
        if ( isRelative ) {
            strcat( targetPath, "../" );
        }
        name = nameNext;
    }

    strcat( targetPath, target );
    link( targetPath, name );

	return 0;
}
