#include <stdio.h>
#include <signal.h>

int main(int argc, char **argv)
{
    int pid;

    if ( argc < 2 || sscanf( argv[ 1 ], "%i", &pid ) != 1 ) {
        puts( "Usage: babykill <pid>" );
        return 1;
    }

    kill( pid, SIGKILL );

	return 0;
}
