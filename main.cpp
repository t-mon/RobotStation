#include <QApplication>
#include "core.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Set Terminal Background to Black...
    a.setStyleSheet("QTextEdit { background-color: QColor(0, 0, 0, 255)}");

    // Start the Core...
    Core::instance();

    return a.exec();
}
