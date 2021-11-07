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
#include <QMenu>

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
    void retrieveTicketsList();
    void retrieveTicketsListFinished();

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
    QJsonValue m_ticketsDataValue;

    QMenu* m_company_name_contextmenu = nullptr;
    QMenu* m_informer_phonenumbers_contextmenu = nullptr;
    QMenu* m_informer_phonenumber_contextmenu = nullptr;
    QMenu* m_informer_emails_contextmenu = nullptr;
    QMenu* m_informer_email_contextmenu = nullptr;
    QAction* deleteInformerEmailAction = nullptr;
    QAction* deleteInformerPhoneNumberAction = nullptr;

    void renameCompany(const QString& newName);
    void lookup_n_set_CompanyName();
    void addInformerEmail(const QString&);
    void deleteInformerEmail(const QString&);
    void informerInfoReq(const QByteArray, const QByteArray);
    void addInformerPhoneNumber(const QString&);
    void updateFinished();

  public slots:
    void on_addCommentButton_clicked();
    void onCommentUpdated(int);

  private slots:
    void on_consumer_tabWidget_tabBarClicked(int);
    void showCompanyNameMenu(const QPoint&);
    void onCompanyNameSlot(QAction*);
    void showInformerEmailsMenu(const QPoint&);
    void showInformerEmailMenu(const QPoint&);
    void showInformerPhoneNumbersMenu(const QPoint&);
    void showInformerPhoneNumberMenu(const QPoint&);
    void onInformerEmailsSlot(QAction*);
    void onInformerPhoneNumbersSlot(QAction*);

  signals:
    void commentAdded(int);

};

#endif // CALLERDATAWINDOW_H
