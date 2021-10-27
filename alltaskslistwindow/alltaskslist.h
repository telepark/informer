#ifndef ALLTASKSLIST_H
#define ALLTASKSLIST_H

#include <QMainWindow>

#include <QMouseEvent>
#include <QDesktopServices>
#include <QScreen>
#include <QSettings>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QLocale>
#include <QLabel>
#include <QDateTime>

#include "kazoo_auth/kazoo_auth.h"
#include "defaults.h"

namespace Ui {
class AllTasksList;
}

class AllTasksList : public QMainWindow
{
    Q_OBJECT

public:
    explicit AllTasksList(QWidget *parent = nullptr);
    ~AllTasksList();

    void retrieveCommentsList();
    void retrieveCommentsListFinished();
    void handleConnectionError();

private:
    Ui::AllTasksList *ui;
    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = new QNetworkAccessManager(this);
    QJsonValue m_commentsDataValue;

public slots:
    void onCommentUpdated(int);

};

#endif // ALLTASKSLIST_H
