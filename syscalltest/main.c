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

#define BUFFER_SIZE 100

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

    char *filename = "input.txt";

    puts( "Введите строку:" );

    char input[ BUFFER_SIZE ];
    fgets( input, BUFFER_SIZE, stdin );

    int fd = open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
    write( fd, input, strlen( input ) );
    close( fd );

    printf( "Ваша строка сохранена в файл '%s'.\n", filename );

    char output[ BUFFER_SIZE ];
    fd = open( filename, O_RDONLY );
    int length = read( fd, output, BUFFER_SIZE );

    puts( "Вот что удалось прочитать из этого файла (должна быть та же строка):" );
    output[ length ] = 0;
    fputs( output, stdout );

    puts( "Попробуем вернуться в начало файла и снова прочитать первый символ:" );
    lseek( fd, 0, SEEK_SET );
    read( fd, output, 1 );
    output[ 1 ] = 0;
    puts( output );

    close( fd );

    struct stat info;
    stat( filename, &info );

    printf( "Данный файл имеет UID %u, GID %u. Его размер в байтах: %li.\n", info.st_uid, info.st_gid, info.st_size );
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

    printf( "Создадим директорию '%s'.\n", dirname );
    mkdir( dirname, 0777 );

    printf( "Вот, что получилось:\n" );
    ls();

    printf( "Теперь удалим эту директорию.\n" );
    rmdir( dirname );

    printf( "Вот, что получилось теперь:\n" );
    ls();

    char *filename = "some-test-file";
    char *linkname = "some-test-link";

    printf( "Теперь создадим файл '%s'.\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0644 ) );

    printf( "И создадим ссылку на него с именем '%s'.\n", linkname );
    link( filename, linkname );

    printf( "Получили следующее (обратим внимание на второй столбец):\n" );
    ls();

    printf( "Сначала удалим (unlink) исходное имя файла:\n" );
    unlink( filename );
    ls();

    printf( "А теперь удалим второе имя файла (ссылку):\n" );
    unlink( linkname );
    ls();
}

void testOtherCalls() {

    char *filename = "temp-file-for-chmod-test";

    printf( "Перейдем в '/tmp':\n" );
    chdir( "/tmp" );
    ls();

    printf( "Создадим здесь файл '%s' с минимальным уровнем доступа:\n", filename );
    close( open( filename, O_CREAT | O_TRUNC | O_WRONLY, 0400 ) );
    ls();

    printf( "Теперь расширим уровень доступа для этого файла:\n" );
    chmod( filename, 0777 );
    ls();

    unlink( filename );

    printf( "Теперь создадим (fork) дочерний процесс,\nкоторый каждую секунду будет выводить текущую метку времени.\n" );
    printf( "Нажмите ENTER, когда будете готовы.\nЧтобы завершить этот дочерний процесс, нажмите ENTER еще раз.\n");
    getchar();

    int pid = fork();
    if ( pid == 0 ) {
        while ( 1 ) {
            printf( "%li\n", time( NULL ) );
            sleep( 1 );
        }
        exit( 0 );
    }

    getchar();
    kill( pid, SIGKILL );
    printf( "Дочерний процесс завершен.\n" );
}

int main( int argc, char **argv )
{
    printf( "\nДемонстрация системных вызовов управления процессами\n\n" );
    testProcessCalls();
    printf( "\n(для продолжения нажмите ENTER)\n" );
    getchar();

    printf( "\nДемонстрация системных вызовов файлового ввода/вывода\n\n" );
    testFileCalls();
    printf( "\n(для продолжения нажмите ENTER)\n" );
    getchar();

    printf( "\nДемонстрация системных вызовов управления файловой системой\n\n" );
    testFsCalls();
    printf( "(для продолжения нажмите ENTER)\n" );
    getchar();

    printf( "\nДемонстрация прочих системных вызовов\n\n" );
    testOtherCalls();

    return 0;
}
