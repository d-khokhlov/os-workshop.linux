#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
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

			execl( "/bin/echo", "/bin/echo", message, NULL );

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
	close( fd );

	puts( "Here what we can read from this file:" );

	output[ length ] = 0;
	fputs( output, stdout );

	struct stat info;
	stat( filename, &info );

	printf( "This file has UID %u, GID %u and size %li.\n", info.st_uid, info.st_gid, info.st_size );
}

int main( int argc, char **argv )
{
	//testProcessCalls();
	testFileCalls();
	return 0;
}
