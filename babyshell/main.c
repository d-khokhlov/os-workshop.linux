#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main( int argc, char **argv )
{
    char *prompt = "> ";
    char *spaces = " \t\n\r\v\f";
    char input[ BUFFER_SIZE ] = "";
    char *parameters[ BUFFER_SIZE ];

    // Main loop
    while ( 1 ) {

        // Print prompt
        printf( "%s", prompt );

        // Read command line from user
        fgets( input, BUFFER_SIZE, stdin );

        // Split command line into parameters
        int i = 0;
        parameters[ 0 ] = strtok( input, spaces );
        while( parameters[ i ] != NULL ) {
            i++;
            parameters[ i ] = strtok( NULL, spaces );
        }

        // Handle empty command line and 'exit' command
        if ( parameters[ 0 ] == NULL ) {
            continue;
        } else if ( strcmp( parameters[ 0 ], "exit" ) == 0 ) {
            break;
        }

        // Fork current process to perform command execution
        int pid = fork();
        if ( pid != 0 ) {

            // Parent process waits until child ends
            waitpid( pid, NULL, 0 );

        } else {

            // Execute command in child process
            execvp( parameters[ 0 ], parameters );

            // We can reach here only if command executable
            // wasn't load into this process, assume bad command
            puts( "error: unknown command");
            return 1;
        }
    }

    return 0;
}
