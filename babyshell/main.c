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
    char *commands[ BUFFER_SIZE ];
    char *parameters[ BUFFER_SIZE ];
    int inFd, outFd, pipeFds[ 2 ];

    // Duplicate input (0) and output (1) file descriptors
    // to be able to restore them later
    inFd = dup( 0 );
    outFd = dup( 1 );

    // Main loop
    while ( 1 ) {

        // Print prompt
        printf( "%s", prompt );

        // Read command line from user
        fgets( input, BUFFER_SIZE, stdin );

        // Split command line into commands separated by '|' character
        commands[ 0 ] = strtok( input, "|" );
        int commandIndex = 0;
        while ( commands[ commandIndex ] != NULL ) {
            commandIndex++;
            commands[ commandIndex ] = strtok( NULL, "|" );
        }

        // Handle each command
        for ( commandIndex = 0; commands[ commandIndex ] != NULL; commandIndex++ ) {

            // Extract command name
            parameters[ 0 ] = strtok( commands[ commandIndex ], spaces );

            // Handle empty command
            if ( parameters[ 0 ] == NULL ) {
                continue;
            }

            // Handle 'exit' command
            if ( strcmp( parameters[ 0 ], "exit" ) == 0 ) {
                return 0;
            }

            // If this is not the last command
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // Create pipe to use between this and next command
                pipe( pipeFds );

                // Connect write end of pipe to stdout
                close( 1 );
                dup( pipeFds[ 1 ] );

                // Close created pipe write end file descriptor.
                // Otherwise pipe will won't terminate correctly.
                close( pipeFds[ 1 ] );

            // If this is last but not single command
            } else if ( commandIndex > 0 ) {

                // Restore original stdout
                close( 1 );
                dup( outFd );
            }

            // Filenames for input/output redirect will be stored here. NULL - means no redirect.
            char *inputFilename = NULL;
            char *outputFilename = NULL;

            // Extract command parameters
            int parameterIndex = 1;
            while( ( parameters[ parameterIndex ] = strtok( NULL, spaces ) ) != NULL ) {

                // Check if this parameter is input/output redirect
                switch ( parameters[ parameterIndex ][ 0 ] ) {

                    // If input redirect
                    case '<':

                        // Store filename excluding first '<' character
                        inputFilename = parameters[ parameterIndex ] + 1;
                        break;

                    // If output redirect
                    case '>':

                        // Store filename excluding first '>' character
                        outputFilename = parameters[ parameterIndex ] + 1;
                        break;

                    // If no redirect (usual command parameter)
                    default:

                        // Increase index so that this parameter won't be overwritten
                        // during next iteration (as it will in case of redirect)
                        parameterIndex++;
                }
            } // parameter extraction loop end

            // Fork process to execute current command
            if ( fork() == 0 ) {

                // Child process

                // If input redirect specified
                if ( inputFilename != NULL ) {

                    // Close stdin (file descriptor 0)
                    close( 0 );

                    // Open input file (will use descriptor 0 as it is minimal available)
                    open( inputFilename, O_RDONLY );
                }

                // If output redirect specified
                if ( outputFilename != NULL ) {

                    // Close stdout (file descriptor 1)
                    close( 1 );

                    // Open output file (will use descriptor 1 as it is minimal available)
                    open( outputFilename, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
                }

                // Execute command in child process
                execvp( parameters[ 0 ], parameters );

                // We can reach here only if command executable
                // wasn't load into this process, assume bad command
                fprintf( stderr, "error: unknown command '%s'\n", parameters[ 0 ] );

                // Exit child process manualy to prevent parent process code execution.
                return 1;
            }

            // Parent process continue

            // If this was not the last command
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // Connect read end of pipe to stdin for the next command
                close( 0 );
                dup( pipeFds[ 0 ] );

                // Close created pipe read end file descriptor.
                // Otherwise pipe will won't terminate correctly.
                close( pipeFds[ 0 ] );

            // If this was last but not single command
            } else if ( commandIndex > 0 ) {

                // Restore original stdin
                close( 0 );
                dup( inFd );
            }

        } // commands loop end

        // Wait for all child processes to exit
        while ( wait( NULL ) > 0 );

    } // main loop end

    return 0;
}
