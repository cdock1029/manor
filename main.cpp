#include "database.h"
#include "manor.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    if (!createConnection())
        return EXIT_FAILURE;

    Manor w("properties");
    w.show();
    return a.exec();
}
