#include "kazoo_auth.h"

#include "defaults.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QSettings>
#include <QRegularExpression>

#include <QTimer>

static const char* const kRetrieveWsPath = "/";
static const char* const kWsPath = "/";
static const char* const kAuthPath = "/v1/user_auth";
static const char* const kCheckTokenPath = "/v2/token_auth";


static const int kCheckTokenTimeoutMSec = 30000;
static const int kRetrieveTokenGapSec = 300;


KazooAuth::KazooAuth(QObject* parent) : QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
    m_token_check_timer = new QTimer(this);
    connect(m_token_check_timer, &QTimer::timeout, this, &KazooAuth::checkTokenAlive);
}

void KazooAuth::start()
{
    qDebug() << "\n KazooAuth::start()  Starting!!!!!!!!!!!!!!!!!!!!!\n";

    m_token_check_timer->start(kCheckTokenTimeoutMSec);

    if (m_settings) {
        m_settings->deleteLater();
    }

    qDebug() << "dataDirPath(): " << dataDirPath();

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    retrieveAuthToken();
}

void KazooAuth::stop()
{
    qDebug() << "\n KazooAuth::stop()  Stopping!!!!!!!!!!!!!!!!!!!!!\n";
    m_token_check_timer->stop();

    if (m_settings) {
        m_settings->deleteLater();
        m_settings = nullptr;
    }
}

QString KazooAuth::authToken()
{
    return m_authToken;
}

QString KazooAuth::ownerId()
{
    return m_ownerId;
}

QString KazooAuth::accountId()
{
    return m_accountId;
}

void KazooAuth::retrieveAuthToken()
{
    QByteArray hashTemplate(m_settings->value("login", kLogin).toByteArray());
    qDebug("KazooAuth Login: %s", hashTemplate.data());

    hashTemplate.append(":");
    hashTemplate.append(m_settings->value("password", kPassword).toByteArray());

    QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();
    QJsonObject jsonObject;
    QJsonObject jsonData;
    jsonData["credentials"] = hash.data();
    jsonData["realm"] = m_settings->value("realm", kRealm).toString();
    qDebug("Realm: %s", jsonData["realm"].toString().toLatin1().data());
    jsonObject["data"] = jsonData;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray json = jsonDocument.toJson();

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    QString authUrl(m_settings->value("auth_url", kAuthUrl).toString());
    authUrl.append(kAuthPath);
    qDebug("KazooAuth Auth url: %s", authUrl.toLatin1().data());
    req.setUrl(QUrl(authUrl));

    QNetworkReply* reply = m_nam->put(req, json);
    connect(reply, &QNetworkReply::finished,
            this, &KazooAuth::retrieveAuthTokenFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &KazooAuth::handleConnectionError);
}

void KazooAuth::retrieveAuthTokenFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        qDebug("%s, NetworkReply error: %s, request: %s",
               Q_FUNC_INFO,
               reply->errorString().toLatin1().data(),
               reply->request().url().toString().toLatin1().data());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning("Cannot parse retrieve auth token data: %s",
                 error.errorString().toLatin1().data());
        qDebug("Retrieve auth token data: %s", data.data());
        handleConnectionError();
        return;
    }

    m_authToken = jsonDocument.object().value("auth_token").toString();
    QJsonObject dataObject = jsonDocument.object().value("data").toObject();
    m_accountId = dataObject.value("account_id").toString();
    m_ownerId = dataObject.value("owner_id").toString();
    qDebug() << "\n About to emit kazooTokenObtained() \n";
    emit kazooTokenObtained();
}

void KazooAuth::handleConnectionError()
{
    qWarning("KazooAuth::retrieveAuthToken connection error");
    //    stop();
    //    emit connectionError();
}

void KazooAuth::checkTokenAlive()
{
    qDebug("KazooAuth::checkTokenAlive() It's time to check token!!");

    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", m_authToken.toLatin1());

    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    QString authUrl(m_settings->value("auth_url", kAuthUrl).toString());
    authUrl.append(kCheckTokenPath);
    qDebug("KazooAuth::checkTokenAlive url: %s", authUrl.toLatin1().data());
    req.setUrl(QUrl(authUrl));

    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &KazooAuth::checkTokenAliveFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &KazooAuth::handleConnectionError);


}

void KazooAuth::checkTokenAliveFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());

    if (reply->error() != QNetworkReply::NoError) {
        qDebug("%s, NetworkReply error: %s, request: %s",
               Q_FUNC_INFO,
               reply->errorString().toLatin1().data(),
               reply->request().url().toString().toLatin1().data());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning("Cannot parse retrieve auth token data: %s",
                 error.errorString().toLatin1().data());
        qDebug("Retrieve auth token data: %s", data.data());
        handleConnectionError();
        return;
    }

    QString resp_status = jsonDocument.object().value("status").toString();
    QJsonObject dataObject = jsonDocument.object().value("data").toObject();
    //    qDebug() << "\n Resp dataObject: " << dataObject << "\n";
    qlonglong resp_expire_time = dataObject.value("exp").toInteger();
    qDebug() << "\n Token will be expired in " << (resp_expire_time -
                                                   QDateTime::currentSecsSinceEpoch()) / 60 << " minutes\n";

    if ((resp_expire_time - QDateTime::currentSecsSinceEpoch()) < kRetrieveTokenGapSec) {
        retrieveAuthToken();
    }

}
