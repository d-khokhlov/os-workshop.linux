#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
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

        // Extract command name
        parameters[ 0 ] = strtok( input, spaces );

        // Handle empty command line
        if ( parameters[ 0 ] == NULL ) {
            continue;
        }

        // Handle 'exit' command
        if ( strcmp( parameters[ 0 ], "exit" ) == 0 ) {
            break;
        }

        // Filenames for input/output redirect will be stored here. NULL - means no redirect.
        char *inputFilename = NULL;
        char *outputFilename = NULL;

        // Extract command parameters
        int i = 1;
        while( ( parameters[ i ] = strtok( NULL, spaces ) ) != NULL ) {

            // Check if this parameter is input/output redirect
            switch ( parameters[ i ][ 0 ] ) {

                // If input redirect
                case '<':

                    // Store filename excluding first '<' character
                    inputFilename = parameters[ i ] + 1;
                    break;

                // If output redirect
                case '>':

                    // Store filename excluding first '>' character
                    outputFilename = parameters[ i ] + 1;
                    break;

                // If no redirect (usual command parameter)
                default:

                    // Increase index so that this parameter won't be overwritten
                    // during next iteration (as it will in case of redirect)
                    i++;
            }
        } // parameter extraction loop end

        // Fork current process to perform command execution
        int pid = fork();

        // Parent process
        if ( pid != 0 ) {

            // Wait until child exits
            waitpid( pid, NULL, 0 );

        // Child process
        } else {

            // If input redirect specified
            if ( inputFilename != NULL ) {

                // Close stdin (file descriptor - 0)
                close( 0 );

                // Open input file (will use descriptor 0 as it is minimal available)
                open( inputFilename, O_RDONLY );
            }

            // If output redirect specified
            if ( outputFilename != NULL ) {

                // Close stdout (file descriptor - 1)
                close( 1 );

                // Open output file (will use descriptor 1 as it is minimal available)
                open( outputFilename, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
            }

            // Execute command in child process
            execvp( parameters[ 0 ], parameters );

            // We can reach here only if command executable
            // wasn't load into this process, assume bad command
            puts( "error: unknown command");
            return 1;
        }
    } // main loop end

    return 0;
}
