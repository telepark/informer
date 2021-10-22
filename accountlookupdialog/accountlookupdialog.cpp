#include "accountlookupdialog.h"
#include "ui_accountlookupdialog.h"

#include "callerdatawindow/callerdatawindow.h"

#include <QTimer>
#include <QScrollBar>
#include <QMessageBox>

static const char* const kAccountsQuery =
        "/accounts/%1/zzhds/hd_accounts?md5=%2";


AccountLookupDialog::AccountLookupDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::AccountLookupDialog)
{
    ui->setupUi(this);
    retrieveAccountsList();
}

AccountLookupDialog::~AccountLookupDialog()
{
    delete ui;
}

void AccountLookupDialog::retrieveAccountsList()
{
    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    QByteArray hashTemplate(KAZOOAUTH.accountId().toLatin1());
    hashTemplate.append(":");
    hashTemplate.append(m_settings->value("md5_hash", kMd5Hash).toByteArray());
    QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();
    QNetworkRequest req;
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //SSL
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    QString url(m_settings->value("info_url", kInfoUrl).toString());
    url.append(kAccountsQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &AccountLookupDialog::retrieveAccountsListFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &AccountLookupDialog::handleConnectionError);
}

void AccountLookupDialog::retrieveAccountsListFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return;
    }

    m_dataValue = document.object().value("data");

    if (m_dataValue.isArray()) {
        QJsonArray dataArray = m_dataValue.toArray();

        for (int i = 0; i < dataArray.count(); i++) {
            QJsonObject AccountObj = dataArray.at(i).toObject();
            QListWidgetItem* newItem = new QListWidgetItem;
            newItem->setData(1, AccountObj.value("id").toString());
            newItem->setText(AccountObj.value("name").toString());
            ui->accountsListWidget->addItem(newItem);
        }
    }
    ui->total_digits_label->setText(QString("%1").arg(ui->accountsListWidget->count()));
}

void AccountLookupDialog::handleConnectionError()
{
    qWarning("AccountLookupDialog::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    qDebug() << "\n AccountLookupDialog::handleConnectionError Reply->errorString() : " <<
                reply->errorString() << "\n";
    qDebug() << "\n AccountLookupDialog::handleConnectionError Reply->error() : " <<
                reply->error() << "\n";
}

void AccountLookupDialog::on_pushButton_clicked()
{if (ui->accountsListWidget->currentItem()) {
        CallerDataWindow* m_callerdatawindow = new CallerDataWindow();
        m_callerdatawindow->show();
        m_callerdatawindow->setAccountId(ui->accountsListWidget->currentItem()->data(1).toString());
        this->hide();
    } else {
        qDebug("No Account selected in lookup window\n");
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No account selected");
        messageBox.setFixedSize(500,200);
    }
}

void AccountLookupDialog::on_accountsListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    CallerDataWindow* m_callerdatawindow = new CallerDataWindow();
    m_callerdatawindow->show();
    m_callerdatawindow->setAccountId(item->data(1).toString());
    this->hide();
}

void AccountLookupDialog::on_pushButton_2_clicked()
{
    this->hide();
}

void AccountLookupDialog::on_lineEdit_textChanged(const QString &arg1)
{
    qDebug() << "on_lineEdit_textChanged: " << arg1 << "\n";
    if (m_dataValue.isArray()) {
        QJsonArray dataArray = m_dataValue.toArray();
        ui->accountsListWidget->clear();
        QJsonObject AccountObj;
        for (int i = 0; i < dataArray.count(); i++) {
            AccountObj = dataArray.at(i).toObject();
            if (AccountObj.value("name").toString().contains(arg1, Qt::CaseInsensitive)) {

                QListWidgetItem* newItem = new QListWidgetItem;
                newItem->setData(1, AccountObj.value("id").toString());
                newItem->setText(AccountObj.value("name").toString());
                ui->accountsListWidget->addItem(newItem);
            }
        }
    }
    ui->total_digits_label->setText(QString("%1").arg(ui->accountsListWidget->count()));
}

