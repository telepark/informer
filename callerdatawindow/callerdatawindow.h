#ifndef CALLERDATAWINDOW_H
#define CALLERDATAWINDOW_H

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
class CallerDataWindow;
}

class QMouseEvent;


class CallerDataWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit CallerDataWindow(QWidget* parent = nullptr);
    ~CallerDataWindow();

    void setAccountId(const QString&);
    void setInformerId(const int);
    void setInformerId(const QString);
    void retrieveConsumerInfoFinished();
    void handleConnectionError();
    void addCommentFinished();
    void retrieveCommentsList();
    void retrieveCommentsListFinished();

private:
    Ui::CallerDataWindow* ui;
    QString m_consumer_accountId = "";
    int m_lbId;
    int m_informerId;
    QString m_phone_number = "";
    QJsonObject m_kazooDataJObj = {};
    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = new QNetworkAccessManager(this);
    QJsonValue m_commentsDataValue;

public slots:
    void on_some_pushButton_clicked();
    void onCommentUpdated(int);

private slots:
    void on_consumer_tabWidget_tabBarClicked(int);

signals:
    void commentAdded(int);

};

#endif // CALLERDATAWINDOW_H
