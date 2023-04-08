#include "episodiac.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setApplicationName(APP_PRODUCT);
    a.setApplicationVersion(APP_VERSION);

    Episodiac w;

    w.setWindowTitle("EpisodiacV2 (" + QString(APP_VERSION) + ")");
    w.setWindowIcon(QIcon("icon.ico"));

    w.show();

    return a.exec();
}
