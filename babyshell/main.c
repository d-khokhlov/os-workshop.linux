#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

int main( int argc, char **argv )
{
    // Создать копии дескрипторов stdin (0) и stdout (1),
    // чтобы можно было восстановить их после.
    int inFd = dup( 0 );
    int outFd = dup( 1 );

    // Главный цикл
    while ( 1 ) {

        // Вывести приглашение
        printf( "%s", "> " );

        // Прочитать командную строку пользователя
        char input[ BUFFER_SIZE ] = "";
        fgets( input, BUFFER_SIZE, stdin );

        // Разбить введенную строку на отдельные команды, разделенные символом '|'
        char *commands[ BUFFER_SIZE ];
        commands[ 0 ] = strtok( input, "|" );
        int commandIndex = 0;
        while ( commands[ commandIndex ] != NULL ) {
            commandIndex++;
            commands[ commandIndex ] = strtok( NULL, "|" );
        }

        // Параметры команды (нулевой элемент - сама команда)
        char *parameters[ BUFFER_SIZE ];

        // Обработать каждую команду
        for ( commandIndex = 0; commands[ commandIndex ] != NULL; commandIndex++ ) {

            // Пробельные символы, которыми разделяются параметры
            char *spaces = " \t\n\r\v\f";

            // Выделить имя команды (исполняемого файла)
            parameters[ 0 ] = strtok( commands[ commandIndex ], spaces );

            // Если команда пустая, перейти к следующей
            if ( parameters[ 0 ] == NULL ) {
                continue;
            }

            // Если получена команда 'exit', выйти
            if ( strcmp( parameters[ 0 ], "exit" ) == 0 ) {
                return 0;
            }

            // Дескрипторы канала для передачи ввода/вывода между командами
            int pipeFds[ 2 ];

            // Если текущая команда не последняя
            if ( commands[ commandIndex + 1 ] != NULL ) {

                // Создать канал для текущей и следующей команд
                pipe( pipeFds );

                // Закрыть stdout (дескриптор 1 становится свободным)
                close( 1 );

                // Создать копию дескриптора входа (write end) канала.
                // В качестве копии береться минимальный свободный дескриптор
                // (в данном случае 1), таким образом stdout связывается с входом канала.
                dup( pipeFds[ 1 ] );

                // Закрыть первоначальный дескриптор входа канала.
                // Иначе канал никогда не выдаст EOF.
                close( pipeFds[ 1 ] );

            // Если текущая команда последняя, но не единственная
            } else if ( commandIndex > 0 ) {

                // Восстановить оригинальный дескриптор stdout
                close( 1 );
                dup( outFd );
            }

            // Имена файлов для перенаправления ввода/вывода.
            // NULL означает, что соответствующего перенаправления нет.
            char *inputFilename = NULL;
            char *outputFilename = NULL;

            // Индекс элемента массива для сохранения следующего параметра команды
            int parameterIndex = 1;

            // Извлечь параметры команды по одному
            while( ( parameters[ parameterIndex ] = strtok( NULL, spaces ) ) != NULL ) {

                // Проверить, является ли данный параметр перенаправлением ввода/вывода
                switch ( parameters[ parameterIndex ][ 0 ] ) {

                    // Если это перенаправление ввода
                    case '<':

                        // Сохранить имя файла исключая символ '<'
                        inputFilename = parameters[ parameterIndex ] + 1;
                        break;

                    // Если это перенаправление вывода
                    case '>':

                        // Сохранить имя файла исключая символ '>'
                        outputFilename = parameters[ parameterIndex ] + 1;
                        break;

                    // Если это не перенаправление (обычный параметр)
                    default:

                        // Увеличить индекс в массиве параметров, чтобы текущий параметр
                        // не был переписан на следующей итерации (как это происходит с
                        // перенаправлениями ввода/вывода)
                        parameterIndex++;
                }
            } // Конец цикла извлечения параметров

            // Создать дочерний процесс для выполнения текущей команды
            if ( fork() == 0 ) {

                // В дочернем процессе

                // Если задано перенаправление ввода
                if ( inputFilename != NULL ) {

                    // Закрыть stdin (дескриптор 0 становится свободным)
                    close( 0 );

                    // Открыть заданный файл на чтение. При этом занимается
                    // минимальный свободный дескриптор (в данном случае 0),
                    // таким образом stdin связывается с заданным файлом.
                    open( inputFilename, O_RDONLY );
                }

                // Если задано перенаправление вывода
                if ( outputFilename != NULL ) {

                    // Закрыть stdout (дескриптор 1 становится свободным)
                    close( 1 );

                    // Создать/открыть заданный файл на запись. При этом занимается
                    // минимальный свободный дескриптор (в данном случае 1),
                    // таким образом stdout связывается с заданным файлом.
                    open( outputFilename, O_CREAT | O_TRUNC | O_WRONLY, 0644 );
                }

                // Execute command in child process
                execvp( parameters[ 0 ], parameters );

                // We can reach here only if command executable
                // wasn't load into this process, assume bad command
                fprintf( stderr, "error: unknown command '%s'\n", parameters[ 0 ] );

                // Exit child process manualy to prevent parent process code execution.
                return 1;
            } // конец дочернего процесса

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
