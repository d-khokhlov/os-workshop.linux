#include <stdio.h>
#include <signal.h>

int main(int argc, char **argv)
{
    int pid;

    // Если не получено ни одного параметра или
    // из первого параметра не получается извлечь PID (целое число)
    if ( argc < 2 || sscanf( argv[ 1 ], "%i", &pid ) != 1 ) {

        puts( "Usage: babykill <pid>" );

        // Выйти со статусом ошибки
        return 1;
    }

    // Попробовать завершить заданный процесс, отправив ему SIGKILL
    kill( pid, SIGKILL );

    // Выйти со статусом успеха
	return 0;
}
