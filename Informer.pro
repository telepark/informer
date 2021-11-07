#-------------------------------------------------
#
# Project created by QtCreator 2014-09-10T16:29:41
#
#-------------------------------------------------

QT       += core gui websockets network

### greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QT += widgets
QT += core5compat

include(qtsingleapplication/qtsingleapplication.pri)

TARGET = Informer
TEMPLATE = app

CONFIG += c++11

# Application version
VERSION_MAJOR=1
VERSION_MINOR=0
VERSION_PATCH=0
DEFINES += APP_VERSION=\\\"$${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_PATCH}\\\"

SOURCES += \
    accountlookupdialog/accountlookupdialog.cpp \
    alltaskslistwindow/alltaskslist.cpp \
    callerdatawindow/addfielddialog.cpp \
    callerdatawindow/callerdatawindow.cpp \
    comment/comment.cpp \
    comment/commentscontainer.cpp \
    debugdialog/debugdialog.cpp \
    informerdialog/informerdialog.cpp \
    kazoo_auth/kazoo_auth.cpp \
    main.cpp \
    defaults.cpp \
    logger.cpp \
    mainwindow/mainwindow.cpp \
    ticket/message.cpp \
    ticket/messagescontainer.cpp \
    ticket/ticket.cpp \
    ticket/ticketscontainer.cpp \
    websocketmanager/websocketmanager.cpp

HEADERS  += \
    accountlookupdialog/accountlookupdialog.h \
    alltaskslistwindow/alltaskslist.h \
    callerdatawindow/addfielddialog.h \
    callerdatawindow/callerdatawindow.h \
    comment/comment.h \
    comment/commentscontainer.h \
    debugdialog/debugdialog.h \
    informerdialog/informerdialog.h \
    defaults.h \
    kazoo_auth/kazoo_auth.h \
    kazoo_auth/singleton.h \
    logger.h \
    mainwindow/mainwindow.h \
    ticket/message.h \
    ticket/messagescontainer.h \
    ticket/ticket.h \
    ticket/ticketscontainer.h \
    websocketmanager/websocketmanager.h

win32 | macx {
    SOURCES += updatemanager.cpp
    HEADERS += updatemanager.h
}

FORMS    += \
    accountlookupdialog/accountlookupdialog.ui \
    alltaskslistwindow/alltaskslist.ui \
    callerdatawindow/callerdatawindow.ui \
    debugdialog/debugdialog.ui \
    informerdialog/informerdialog.ui \
    mainwindow/mainwindow.ui

RESOURCES += \
    app.qrc

macx {
    RESOURCES += macx.qrc
}

win32 {
    RC_FILE += rc.rc
}

macx {
    ICON = res/mac/kazoo.icns
    QMAKE_INFO_PLIST = res/mac/MyAppInfo.plist

    OTHER_FILES += res/mac/MyAppInfo.plist

    deploy.depends  += all
    deploy.commands += macdeployqt $${TARGET}.app;

    deploy.commands += cp $${PWD}/setup/settings.ini $${TARGET}.app/Contents/MacOS/settings.ini;

    # Remove unneeded plugins
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/printsupport;
    deploy.commands += rm -r $${TARGET}.app/Contents/PlugIns/sqldrivers;

    # Build the product package
    #product.depends += all
    #product.commands += pkgbuild --identifier 2600hz --version 1.0 --root Informer.app --install-location ~/Applications/Informer.app Informer.pkg;

    makedmg.commands += sh $${PWD}/res/mac/make_dmg.sh -V -i $${PWD}/$${ICON} -s "400:300" -c "300:50:100:50" "$${TARGET}.app";

    QMAKE_EXTRA_TARGETS += deploy makedmg
}
