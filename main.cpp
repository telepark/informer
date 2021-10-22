#include "mainwindow/mainwindow.h"
#include "logger.h"
#include "kazoo_auth/kazoo_auth.h"

#include <QtSingleApplication>
#include <QTextCodec>

int main(int argc, char* argv[])
{
    QtSingleApplication a(argc, argv);

    if (a.isRunning()) {
        return 0;
    }

    a.setQuitOnLastWindowClosed(false);
    a.setApplicationVersion(APP_VERSION);

    Logger::instance()->start();

    KAZOOAUTH.start();

    qDebug() << "\n KAZOOAUTH.authToken(): " << KAZOOAUTH.authToken() << "\n";

    MainWindow w;
    w.hide();

    return a.exec();
}
