#include "OCT_Angio.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OCT_Angio w;
    w.show();
    return a.exec();
}
