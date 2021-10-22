#ifndef ACCOUNTLOOKUPDIALOG_H
#define ACCOUNTLOOKUPDIALOG_H

#include <QDialog>

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
#include <QJsonArray>
#include <QListWidgetItem>

#include "kazoo_auth/kazoo_auth.h"
#include "defaults.h"

namespace Ui {
class AccountLookupDialog;
}

class AccountLookupDialog : public QDialog {
    Q_OBJECT

  public:
    explicit AccountLookupDialog(QWidget* parent = 0);
    ~AccountLookupDialog();

    void retrieveAccountsList();
    void retrieveAccountsListFinished();
    void handleConnectionError();


  private slots:
    void on_pushButton_clicked();
    void on_accountsListWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_2_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::AccountLookupDialog* ui;

    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = new QNetworkAccessManager(this);
    QJsonObject m_kazooDataJObj = {};
    QJsonValue m_dataValue;
};

#endif // ACCOUNTLOOKUPDIALOG_H



