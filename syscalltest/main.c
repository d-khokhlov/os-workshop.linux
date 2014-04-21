#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/stat.h>

void testProcessCalls() {

    printf( "parent: started\n" );

    int i;
    for ( i = 0; i < 3; i++ ) {

        int pid = fork();
        if ( pid != 0 ) {

            printf( "parent: child %i forked with PID %i\n", i, pid );

            int status;
            waitpid( pid, &status, 0 );

            printf( "parent: child %i exited with status 0x%x\n", i, status );

        } else {

            printf( "child %i: started\n", i );

            char message[50];
            sprintf( message, "child %i: echo: executed", i );

            execlp( "echo", "echo", message, NULL );

            printf( "child %i: executing echo failed, error %i\n", i, errno );
            exit( 1 );
        }
    }
    printf( "parent: exiting\n" );
}

void testFileCalls() {

    const int bufferSize = 100;
    char *filename = "input.txt";

    puts( "Input one short line of text:" );

    char input[ bufferSize ];
    fgets( input, bufferSize, stdin );

    int fd = open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
    write( fd, input, strlen( input ) );
    close( fd );

    printf( "Your text is saved to '%s'.\n", filename );

    char output[ bufferSize ];
    fd = open( filename, O_RDONLY );
    int length = read( fd, output, bufferSize );

    puts( "Here what we can read from this file:" );
    output[ length ] = 0;
    fputs( output, stdout );

    puts( "Let's try to rewind and read the first character again:" );
    lseek( fd, 0, SEEK_SET );
    read( fd, output, 1 );
    output[ 1 ] = 0;
    puts( output );

    close( fd );

    struct stat info;
    stat( filename, &info );

    printf( "This file has UID %u, GID %u and size %li.\n", info.st_uid, info.st_gid, info.st_size );
}

void ls() {
    if ( fork() == 0 ) {
        execlp( "ls", "ls", "-lAh", NULL );
        exit( 1 );
    }
    wait( NULL );
    printf( "\n" );
}

void testFsCalls() {

    char *dirname = "some-test-dir";

    printf( "Let's create directory '%s'.\n", dirname );
    mkdir( dirname, 0777 );

    printf( "Here what we have:\n" );
    ls();

    printf( "Now let's delete this directory.\n" );
    rmdir( dirname );

    printf( "And here what we have now:\n" );
    ls();

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "Now lets create file '%s'.\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0644 ) );

    printf( "And link it with name '%s'.\n", linkname );
    link( filename, linkname );

    printf( "We got this (pay attention to the second column):\n" );
    ls();

    printf( "Let's unlink original name first:\n" );
    unlink( filename );
    ls();

    printf( "And now unlink second (link) name:\n" );
    unlink( linkname );
    ls();
}

void testOtherCalls() {

    char *filename = "temp-file-for-chmod-test";

    printf( "Let's go to '/tmp':\n" );
    chdir( "/tmp" );
    ls();

    printf( "Now let's create file '%s' here with minimal permissions:\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0400 ) );
    ls();

    printf( "And promote it's permissions:\n" );
    chmod( filename, 0777 );
    ls();

    unlink( filename );

    printf( "And now let's fork child process\nthat will print current time every second.\nPress ENTER when ready.\nTo kill child process press ENTER again.\n");
    getchar();

    int pid = fork();
    if ( pid == 0 ) {
        while ( 1 ) {
            printf( "Current timestamp: %li\n", time( NULL ) );
            sleep( 1 );
        }
        exit( 0 );
    }

    getchar();
    kill( pid, SIGKILL );
    printf( "Child process killed.\n" );
}

int main( int argc, char **argv )
{
    printf( "\nTesting process management calls\n\n" );
    testProcessCalls();
    printf( "\n(press ENTER to continue)\n" );
    getchar();

    printf( "\nTesting files IO calls\n\n" );
    testFileCalls();
    printf( "\n(press ENTER to continue)\n" );
    getchar();

    printf( "\nTesting filesystem management calls\n\n" );
    testFsCalls();
    printf( "(press ENTER to continue)\n" );
    getchar();

    printf( "\nTesting other system calls\n\n" );
    testOtherCalls();

    return 0;
}
