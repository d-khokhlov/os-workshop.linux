#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main( int argc, char **argv )
{
    char *prompt = "> ";
    char buffer[ BUFFER_SIZE ] = "";

    while ( 1 ) {

        printf( "%s", prompt );

        fgets( buffer, BUFFER_SIZE, stdin );
        buffer[ strlen( buffer ) - 1 ] = 0;

        if ( strcmp( buffer, "exit" ) == 0 ) {
            break;
        }

        int pid = fork();
        if ( pid != 0 ) {
            waitpid( pid, NULL, 0 );
        } else {
            execlp( buffer, buffer, NULL );
        }
    }

    return 0;
}
