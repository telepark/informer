#include <QApplication>

#include "callerdatawindow.h"
#include "ui_callerdatawindow.h"
#include "comment/comment.h"
#include "comment/commentscontainer.h"
#include "alltaskslistwindow/alltaskslist.h"

static const char* const kAccountInfoQuery =
        "/accounts/%1/zzhds/hd_info?consumer_accountId=%2&md5=%3";

static const char* const informerInfoQuery =
        "/accounts/%1/zzhds/informer_info?informer_id=%2&md5=%3";

static const char* const kPostCommentQuery =
        "/accounts/%1/zzhds/hd_comments";

static const char* const kGetCommentQuery =
        "/accounts/%1/zzhds/hd_comments?informer_id=%2&md5=%3";


CallerDataWindow::CallerDataWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::CallerDataWindow)
{
    ui->setupUi(this);
    foreach (QWidget *w, QApplication::topLevelWidgets())
        if (AllTasksList* allTaskWin = qobject_cast<AllTasksList*>(w))
            connect(this, SIGNAL(commentAdded(int)), allTaskWin, SLOT(onCommentUpdated(int)));

}

CallerDataWindow::~CallerDataWindow()
{
    delete ui;
}

void CallerDataWindow::setAccountId(const QString& accountId)
{
    m_consumer_accountId = accountId;

    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    QByteArray hashTemplate(m_consumer_accountId.toLatin1());
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
    url.append(kAccountInfoQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), m_consumer_accountId, hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &CallerDataWindow::retrieveConsumerInfoFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &CallerDataWindow::handleConnectionError);
}

void CallerDataWindow::setInformerId(const int informerId)
{
    setInformerId(QString::number(informerId));
}

void CallerDataWindow::setInformerId(QString informerId)
{
    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    QByteArray hashTemplate(informerId.toLatin1());
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
    url.append(informerInfoQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), informerId, hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &CallerDataWindow::retrieveConsumerInfoFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &CallerDataWindow::handleConnectionError);
}

void CallerDataWindow::retrieveConsumerInfoFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return;
    }

    QJsonObject respData = document.object().value("data").toObject();

    auto russian_locale = QLocale("ru_RU");
    double numeric_balance = respData.value("account_balance").toDouble();
    QString account_balance = russian_locale.toString(numeric_balance, 'f', 2);

    m_lbId = respData.value("lb_id").toInteger();
    m_informerId = respData.value("informer_id").toInteger();

    if (!account_balance.isNull()) {
        ui->optional_comment1_label->setText("Balance: " + account_balance);
    } else {
        ui->optional_comment1_label->hide();
    }

    if (numeric_balance < 0) {
        ui->optional_comment1_label->setStyleSheet("QLabel { color : red; }");
    }

    QJsonObject accountInfo = respData.value("account_info").toObject();

    qDebug() << "\n AccountInfo: " << accountInfo << "\n";

    QString account_name = accountInfo.value("name").toString();

    QJsonValue emailsVal =  accountInfo.value("emails");
    qDebug() << "\n emailsVal: " << emailsVal << "\n";
    QString emailsStr = "";

    if (emailsVal.isArray()) {
        QJsonArray emailsArray = emailsVal.toArray();
        qDebug() << "\n emailsArray: " << emailsArray << "\n";

        for (int i = 0; i < emailsArray.count(); i++) {
            if (i > 0) {
                emailsStr += ", ";
            }

            emailsStr += emailsArray.at(i).toString();
        }

        ui->emails_label->setText("Emails: " + emailsStr);
    }

    QJsonValue phonesVal =  accountInfo.value("phones");
    qDebug() << "\n phonesVal: " << phonesVal << "\n";
    QString phonesStr = "";

    if (phonesVal.isArray()) {
        QJsonArray phonesArray = phonesVal.toArray();
        qDebug() << "\n emailsArray: " << phonesArray << "\n";

        for (int i = 0; i < phonesArray.count(); i++) {
            if (i > 0) {
                phonesStr += ", ";
            }

            phonesStr += phonesArray.at(i).toString();
        }

        ui->phone_numbers_label->setText("Phone numbers: " + phonesStr);
    }

    ui->companyname_label->setText(account_name);
    ui->latest_comment_label->setText("comment_label here 29");
    ui->latest_message_label->setText("latest message date");

    this->setWindowTitle(account_name);
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_some_pushButton_clicked()));
}

void CallerDataWindow::handleConnectionError()
{
    qWarning("InformerDialog::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "\n InformerDialog::handleConnectionError Reply->errorString() : " <<
                reply->errorString() << "\n";
    qDebug() << "\n InformerDialog::handleConnectionError Reply->error() : " <<
                reply->error() << "\n";
}


void CallerDataWindow::on_some_pushButton_clicked()
{
    qDebug() << "\n CallerDataWindow::on_some_pushButton_clicked: " << ui->textEdit->toPlainText() <<
                "\n";
    QJsonObject jsonObject;
    QJsonObject jsonData;
    //    jsonData["credentials"] = hash.data();
    qDebug() << "\n Putting  m_informerId to jsonData: " << m_informerId << "\n";
    jsonData["informer_id"] = m_informerId;
    jsonData["comment_text"] = ui->textEdit->toPlainText();
    jsonData["comment_html"] = ui->textEdit->toHtml();
    jsonObject["data"] = jsonData;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray json = jsonDocument.toJson();

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());

    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    QString url(m_settings->value("info_url", kInfoUrl).toString());
    url.append(kPostCommentQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";

    QNetworkReply* reply = m_nam->post(req, json);
    connect(reply, &QNetworkReply::finished,
            this, &CallerDataWindow::addCommentFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &CallerDataWindow::handleConnectionError);
}

void CallerDataWindow::addCommentFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return;
    }
    retrieveCommentsList();
    ui->textEdit->clear();
    ui->consumer_tabWidget->setCurrentIndex(1);
    emit commentAdded(0);
}

void CallerDataWindow::on_consumer_tabWidget_tabBarClicked(int index)
{
    qDebug() << "\n CallerDataWindow::on_consumer_tabWidget_tabBarClicked index: " << index << "\n";
    if (index == 1) retrieveCommentsList();
}

void CallerDataWindow::onCommentUpdated(int commentId)
{
    qDebug() << "onCommentUpdated commentId: " << commentId << "\n";
    retrieveCommentsList();
}

void CallerDataWindow::retrieveCommentsList()
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
    url.append(kGetCommentQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), QString::number(m_informerId), hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &CallerDataWindow::retrieveCommentsListFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &CallerDataWindow::handleConnectionError);
}

void CallerDataWindow::retrieveCommentsListFinished()
{
    qDebug() << "\n Inside CallerDataWindow::retrieveCommentsListFinished \n";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "\n Inside CallerDataWindow::retrieveCommentsListFinished ERROR!!!! \n";
        return;
    }

    m_commentsDataValue = document.object().value("data");
    if (m_commentsDataValue.isArray()) {
        QJsonArray dataArray = m_commentsDataValue.toArray();
        CommentsContainer* commentContainer = new CommentsContainer;
        commentContainer->addComments(ui->commentsLayout, dataArray, this);
    }
}
