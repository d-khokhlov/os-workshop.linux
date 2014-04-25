#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void printUsageAndExit() {
    puts( "Usage: babywc [-l|-c] <filename>" );

    // Выйти со статусом ошибки
    exit( 1 );
}

int main(int argc, char **argv)
{
    char *filename = NULL;

    // Режим подсчета: 'l' - строки, 'c' - байты
    char mode = 'l';

    // Для каждого параметра командной строки
    // (кроме нулевого, которым является имя программы)
    int argIndex;
    for ( argIndex = 1; argIndex < argc; argIndex++ ) {

        // Если параметр является набором флагов (начинается с '-')
        if ( argv[ argIndex ][ 0 ] == '-' ) {

            // Для каждого символа в этом параметре
            // (кроме нулевого, которым является '-')
            int optionIndex = 1;
            while ( argv[ argIndex ][ optionIndex ] != 0 ) {

                // Если символ - один из флагов режима, сохранить его,
                // иначе - вывести usage и выйти
                switch ( argv[ argIndex ][ optionIndex ] ) {
                    case 'c':
                        mode = 'c';
                        break;
                    case 'l':
                        mode = 'l';
                        break;
                    default:
                        printUsageAndExit();
                }

                // Перейти к следующему символу для следующей итерации
                optionIndex++;
            }

        // Если параметр начинается не с '-', значит это имя файла
        } else {

            // Сохранить его и закончить разбор параметров
            filename = argv[ argIndex ];
            break;
        }
    }

    // Если имя файла не было получено, вывести usage и выйти
    if ( filename == NULL ) {
        printUsageAndExit();
    }

    // Открыть заданный файл на чтение
    int fd = open( filename, O_RDONLY );

    int count = 0;

    // Читать по одному символу из файла, пока файл не кончится
    // (при этом read вернет 0)
    char input;
    while ( read( fd, &input, 1 ) > 0 ) {

        // Увеличить счетчик, если мы считаем байты либо
        // если мы считаем строки и прочитан символ новой страки
        if ( mode == 'c' || ( mode == 'l' && input == '\n' ) ) {
            count++;
        }
    }

    // Закрыть файл
    close( fd );

    // Вывести подсчитанное количество
	printf( "%i\n", count );

    // Выйти со статусом успеха
	return 0;
}
