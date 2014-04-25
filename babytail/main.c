#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

void printUsageAndExit() {

    puts( "Usage: babytail [-c <bytes_count>] <filename>" );

    // Выйти со статусом ошибки
    exit( 1 );
}

int main(int argc, char **argv)
{
    // Если получено мало параметров (минимально должно быть:
    // имя программы и имя файла)
    if ( argc < 2 ) {

        // Вывести usage и выйти
        printUsageAndExit();
    }

    char *filename;

    // Количество выводимых байт (по умолчанию 10)
    int count = 10;

    // Если количество выводимых байт передано в параметрах
    if ( strcmp( argv[ 1 ], "-c" ) == 0 ) {

        // В этом случае минимальное количество параметров
        // увеличивается на 2. Вывести usage и выйти, если передано меньше
        if ( argc < 4 ) {
            printUsageAndExit();
        }

        // Получить требуемое количество байт из второго параметра.
        // Если не удалось, вывести usage и выйти.
        if ( sscanf( argv[ 2 ], "%i", &count ) != 1 ) {
            printUsageAndExit();
        }

        filename = argv[ 3 ];

    // Если нет параметров с требуемым количеством байт
    } else {
        filename = argv[ 1 ];
    }

    // Открыть заданный файл на чтение
    int fd = open( filename, O_RDONLY );

    // Перейти в позицию, отстоящую от конца файла на требуемое количество байт
    lseek( fd, -count, SEEK_END );

    // Прочитать требуемое количество байт, сохранив
    // фактически прочитанное количество, т.к. оно может отличаться
    char buffer[ BUFFER_SIZE ];
    int readCount = read( fd, buffer, count );

    // Закрыть файл
    close( fd );

    // Вывести прочитанные байты
    write( 1, buffer, readCount );

    // Выйти со статусом успеха
	return 0;
}
