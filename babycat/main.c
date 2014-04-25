#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

void printUsageAndExit() {

    puts( "Usage: babycat <in_filename1> [<in_filename2> ...] <out_filename>" );

    // Выйти со статусом ошибки
    exit( 1 );
}

int main(int argc, char **argv)
{
    // Если получено мало параметров (минимально должно быть:
    // имя программы, входной файл и выходной файл)
    if ( argc < 3 ) {

        // Вывести usage и выйти
        printUsageAndExit();
    }

    // Для удобства отделения последнего параметра (имя выходного файла)
    // от остальных параметров (имен входных файлов), вычислить количество
    // параметров минус один
    argc--;

    // Создать/открыть на запись выходной файл
    int outFd = open( argv[ argc ], O_CREAT | O_TRUNC | O_WRONLY, 0666 );

    // Для каждого входного файла (нулевой параметр пропускаем,
    // т.к. это имя программы, а не входного файла)
    int i;
    for ( i = 1; i < argc; i++ ) {

        // Буфер для пересылки данных между файлами
        char *buffer[ BUFFER_SIZE ];

        // Открыть входной файл на чтение
        int inFd = open( argv[ i ], O_RDONLY );

        // Количество фактически прочитанных байт (оно может быть меньше,
        // чем запрошенное количество, если достигнут конец файла)
        int count;

        // Читать данные из входного файла блоками, пока файл не кончится
        // (при этом read вернет 0)
        while ( ( count = read( inFd, buffer, BUFFER_SIZE ) ) > 0 ) {

            // Записать прочитанный блок в выходной файл
            write( outFd, buffer, count );
        }

        // Закрыть входной файл
        close( inFd );
    }

    // Закрыть выходной файл
    close( outFd );

    // Выйти со статусом успеха
	return 0;
}
