#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFFER_SIZE 100

int main(int argc, char **argv)
{
    // Если получено мало параметров (минимально должно быть:
    // имя программы, цель ссылки и источник ссылки)
    if ( argc < 3 ) {

        puts( "Usage: babylinkp <target> <source>" );

        // Выйти со статусом ошибки
        return 1;
    }

    // Для удобства сохранить цель и источник в отдельные переменные
    char *target = argv[ 1 ];
    char *source = argv[ 2 ];

    // Путь к цели (абсолютный или относительно источника)
    char targetPath[ BUFFER_SIZE ] = "";

    // Флаг, указывающий, является ли путь к цели относительным
    char isRelative = 1;

    // Если цель передана в виде абсолютного путь, сбросить флаг
    if ( target[ 0 ] == '/' ) {
        isRelative = 0;
    }

    // Если источник передан в виде абсолютного пути
    if ( source[ 0 ] == '/' ) {

        // Если путь к цели был относительный, его на абсолютный
        if ( isRelative ) {

            // Получить путь к текущей директории и добавить в конец слэш,
            // чтобы после присоединени цели получился ее абсолютный путь
            getcwd( targetPath, BUFFER_SIZE );
            strcat( targetPath, "/" );

            // Сбросить флаг, т.к. теперь мы используем абсолютный путь к цели
            isRelative = 0;
        }

        // Перейти в корневую директорию
        chdir( "/" );
    }

    // Получить первый элемент из пути к источнику. При последующем разборе
    // этого пути данная переменная будет хранить текущий элемент.
    char *name = strtok( source, "/" );

    // Потребуется хранить еще и следующий элемент, т.к. последний элемент требует
    // особенной обработки (не такой, как все остальные элементы), а мы поймем, что
    // элемент был последним не тогда, когда его получим, а только при следующем
    // вызове strtok, когда она вернет NULL.
    char *nameNext;

    // Получать следующие элементы пути к источнику, пока путь не закончится
    while( ( nameNext = strtok( NULL, "/" ) ) != NULL ) {

        // Создать директорию по текущему элементу (если она уже есть,
        // ничего не произойдет)
        mkdir( name, 0777 );

        // Перейти в эту директорию
        chdir( name );

        // Если путь к цели относительный, добавить к нему выход наверх
        // из только что созданной директории
        if ( isRelative ) {
            strcat( targetPath, "../" );
        }

        // Следующий элемент для сделующей итерации будет текущим
        name = nameNext;
    }

    // Соединить две части пути к цели
    strcat( targetPath, target );

    // Создать ссылку
    link( targetPath, name );

    // Выйти со статусом успеха
	return 0;
}
