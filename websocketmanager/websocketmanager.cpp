#include "websocketmanager.h"

#include "defaults.h"
#include "kazoo_auth/kazoo_auth.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QWebSocket>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include <QSettings>
#include <QRegularExpression>

#include <QTimer>

static const char* const kCrossbarPath = "/v1/user_auth/accounts/%1/devices?filter_owner_id=%2";
static const char* const kWsScheme = "wss";

static const int kCheckPingTimeout = 15000;
static const int kPermittedPingTimeout = 30000;

enum CallDirection {
    kCallDirectionInbound,
    kCallDirectionOutbound
};

QString socketErrorToString(QAbstractSocket::SocketError error);

WebSocketManager::WebSocketManager(QObject* parent) :
    QObject(parent)
{
    m_nam = new QNetworkAccessManager(this);
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout,
            this, &WebSocketManager::checkPingTimeout);
    connect(&KAZOOAUTH, &KazooAuth::kazooTokenObtained, this, &WebSocketManager::start);
}

WebSocketManager::~WebSocketManager()
{
    if (m_webSocket) {
        m_webSocket->deleteLater();
    }

    if (m_settings) {
        m_settings->deleteLater();
    }
}

void WebSocketManager::start()
{
    qDebug() << "\n Inside WebSocketManager::start() \n";
    m_timer->start(kCheckPingTimeout);
    m_callerIdsSet.clear();

    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);

    if (m_webSocket) {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }

    m_authToken = KAZOOAUTH.authToken();
    m_accountId = KAZOOAUTH.accountId();

    QString ownerId = KAZOOAUTH.ownerId();

    QNetworkRequest crossbarReq;
    crossbarReq.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    crossbarReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QSslConfiguration config = crossbarReq.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    crossbarReq.setSslConfiguration(config);

    QString crossbarUrl(m_settings->value("crossbar_url", kCrossbarUrl).toString());
    crossbarUrl.append(QString(kCrossbarPath).arg(m_accountId, ownerId));
    crossbarReq.setUrl(QUrl(crossbarUrl));
    qDebug("Crossbar url: %s", crossbarUrl.toLatin1().data());
    QNetworkReply* crossbarReply = m_nam->get(crossbarReq);
    connect(crossbarReply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveDevicesFinished);
    connect(crossbarReply, &QNetworkReply::errorOccurred,
            this, &WebSocketManager::handleConnectionError);
}

void WebSocketManager::stop()
{

    qDebug() << "\n WebSocketManager::stop()  Stopping!!!!!!!!!!!!!!!!!!!!!\n";
    m_timer->stop();

    m_callerIdsSet.clear();

    if (m_settings) {
        m_settings->deleteLater();
        m_settings = nullptr;
    }

    if (m_webSocket) {
        m_webSocket->deleteLater();
        m_webSocket = nullptr;
    }
}

void WebSocketManager::retrieveDevicesFinished()
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
        qWarning("Cannot parse retrieve device data: %s",
                 error.errorString().toLatin1().data());
        qDebug("Retrieve device data: %s", data.data());
        handleConnectionError();
        return;
    }

    qDebug("\nRetrieve device data2: %s\n", data.data());

    QJsonArray devices = jsonDocument.object().value("data").toArray();

    for (int i = 0; i < devices.count(); ++i) {
        m_devices.append(devices.at(i).toObject().value("id").toString());
    }

    QNetworkRequest req;
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);

    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    QString url(m_settings->value("event_url", kEventUrl).toString());
    //   url.append(kRetrieveWsPath);
    req.setUrl(QUrl(url));
    qDebug("235 event url: %s", url.toLatin1().data());
    QNetworkReply* newReply = m_nam->get(req);
    connect(newReply, &QNetworkReply::finished,
            this, &WebSocketManager::retrieveWsAddressFinished);
    //    connect(newReply, &QNetworkReply::errorOccurred,
    //            this, &WebSocketManager::handleConnectionError);
}


void WebSocketManager::retrieveWsAddressFinished()
{
    QUrl wsUrl(m_settings->value("event_url", kEventUrl).toUrl());
    wsUrl.setScheme(kWsScheme);
    qDebug("websocket url: %s", wsUrl.toString().toLatin1().data());
    m_webSocket = new QWebSocket();

    /* Setup SSL */
    QSslConfiguration config = m_webSocket->sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    m_webSocket->setSslConfiguration(config);

    connect(m_webSocket, &QWebSocket::connected,
            this, &WebSocketManager::webSocketConnected);
    connect(m_webSocket, &QWebSocket::disconnected,
            this, &WebSocketManager::webSocketDisconnected);
    connect(m_webSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleConnectionError()));

    m_webSocket->open(wsUrl);
}

void WebSocketManager::webSocketConnected()
{
    qDebug("WebSocket connected");
    connect(m_webSocket, &QWebSocket::textFrameReceived,
            this, &WebSocketManager::webSocketTextFrameReceived);
    QString subscribe("{\"action\":\"subscribe\",\"auth_token\":\"%1\",\"data\":{\"account_id\":\"%2\",\"binding\":\"call.CHANNEL_CREATE.*\"}}");
    m_webSocket->sendTextMessage(subscribe.arg(KAZOOAUTH.authToken(), m_accountId));
    subscribe =
        QString("{\"action\":\"subscribe\",\"auth_token\":\"%1\",\"data\":{\"account_id\":\"%2\",\"binding\":\"call.CHANNEL_ANSWER.*\"}}");
    m_webSocket->sendTextMessage(subscribe.arg(KAZOOAUTH.authToken(), m_accountId));
    subscribe =
        QString("{\"action\":\"subscribe\",\"auth_token\":\"%1\",\"data\":{\"account_id\":\"%2\",\"binding\":\"call.CHANNEL_DESTROY.*\"}}");
    m_webSocket->sendTextMessage(subscribe.arg(KAZOOAUTH.authToken(), m_accountId));
    subscribe = QString("{\"action\":\"ping\"}");
    m_webSocket->sendTextMessage(QString("{\"action\":\"ping\"}"));

    emit connected();
}

void WebSocketManager::webSocketDisconnected()
{
    qDebug("WebSocket disconnected1");
}

void WebSocketManager::webSocketTextFrameReceived(const QString& frame)
{
    if (frame.startsWith("2")) {
        qDebug("inside frame 2");
        qDebug() << frame.toUtf8();
        m_lastPing = QDateTime::currentMSecsSinceEpoch();
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(frame.toLatin1(), &error);

    if (error.error != QJsonParseError::NoError) {
        qWarning("Cannot parse data from websocket: %s",
                 error.errorString().toLatin1().data());
        qDebug("WebSocket data: %s", frame.toLatin1().data());
        return;
    }

    QString actionName = jsonDocument.object().value("action").toString();

    if (actionName == "reply") {
        QJsonObject reply_data = jsonDocument.object().value("data").toObject();
        QString maybe_response = reply_data.value("response").toString();

        if (maybe_response == "pong") {
            qWarning("Got pong");
            m_lastPing = QDateTime::currentMSecsSinceEpoch();
            return;
        }

    }

    if (actionName != "event") {
        return;
    }

    processWsData(frame);
}

void WebSocketManager::processWsData(const QString& data)
{
    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data.toLatin1(), &error);
    QJsonObject jObj = jsonDocument.object();

    if (error.error != QJsonParseError::NoError) {
        qWarning("Cannot parse data from websocket: %s",
                 error.errorString().toLatin1().data());
        qDebug("WebSocket data: %s", data.toLatin1().data());
        return;
    }

    QString eventName = jsonDocument.object().value("name").toString();
    QJsonObject args = jsonDocument.object().value("args").toObject();

    if (eventName == "CHANNEL_CREATE") {
        qDebug() << "Inside ChannelCreate";
        processChannelCreate(jObj["data"].toObject());
    } else if (eventName == "CHANNEL_ANSWER") {
        processChannelAnswer(jObj["data"].toObject());
    } else if (eventName == "CHANNEL_DESTROY") {
        processChannelDestroy(jObj["data"].toObject());
    }
}

bool WebSocketManager::isSupportCallDirection(const QString& callDirection)
{
    int direction = m_settings->value("call_direction", kCallDirection).toInt();

    if (direction == kCallDirectionInbound && callDirection != "inbound") {
        return false;
    }

    if (direction == kCallDirectionOutbound && callDirection != "outbound") {
        return false;
    }

    return true;
}

void WebSocketManager::processChannelCreate(const QJsonObject& args)
{
    QJsonObject customChannelVars = args.value("custom_channel_vars").toObject();
    QString authorizingId = customChannelVars.value("authorizing_id").toString();

    if (!m_devices.contains(authorizingId)) {
        return;
    }

    QString callDirection = args.value("call_direction").toString();

    if (!isSupportCallDirection(callDirection)) {
        return;
    }

    QString otherLegCallId = args.value("other_leg_call_id").toString();

    if (m_callerIdsSet.contains(otherLegCallId)) {
        return;
    }

    QString callerIdName = args.value("caller_id_name").toString();
    QString callerIdNumber = args.value("caller_id_number").toString();
    QString callerDialed = args.value("request").toString();
    int separatorIndex = callerDialed.indexOf("@");
    callerDialed = callerDialed.mid(0, separatorIndex);

    QString url_btn(m_settings->value("info_url", kInfoUrl).toString());
    QRegularExpression re("[^\\{]*\\{\\{([^\\}]*)\\}\\}");
    QRegularExpressionMatch match = re.match(url_btn);

    if (match.hasMatch()) {
        QString key = match.captured(1);
        QString value = args.value(key).toString();
        url_btn.replace("{{" + key + "}}", value);
    }

    m_callerIdsSet.insert(otherLegCallId);

    qDebug("Channel created");
    emit channelCreated(otherLegCallId, callerIdName, callerIdNumber, callerDialed);
}

void WebSocketManager::processChannelAnswer(const QJsonObject& args)
{
    QString callDirection = args.value("call_direction").toString();

    if (callDirection != "outbound") {
        return;
    }

    QJsonObject customChannelVars = args.value("custom_channel_vars").toObject();
    QString authorizingId = customChannelVars.value("authorizing_id").toString();

    QString otherLegCallId = args.value("other_leg_call_id").toString();

    if (m_devices.contains(authorizingId)) {
        qDebug("Channel answered");
        emit channelAnswered(otherLegCallId);
    } else {
        QString calleeNumber = args.value("callee_id_number").toString();
        QString calleeName = args.value("callee_id_name").toString();
        qDebug("Channel answered by another");
        emit channelAnsweredAnother(otherLegCallId, calleeNumber, calleeName);
    }
}

void WebSocketManager::processChannelDestroy(const QJsonObject& args)
{
    QJsonObject customChannelVars = args.value("custom_channel_vars").toObject();
    QString authorizingId = customChannelVars.value("authorizing_id").toString();

    if (!m_devices.contains(authorizingId)) {
        return;
    }

    QString otherLegCallId = args.value("other_leg_call_id").toString();

    if (!m_callerIdsSet.contains(otherLegCallId)) {
        return;
    }

    qDebug("Channel destroyed");
    emit channelDestroyed(otherLegCallId);
    m_callerIdsSet.remove(otherLegCallId);
}

void WebSocketManager::handleConnectionError()
{
    qWarning("WebSocket connection error");

    if (m_webSocket != nullptr) {
        qDebug("WebSocket error: %s",
               socketErrorToString(m_webSocket->error()).toLatin1().data());
    }

    stop();
    emit connectionError();
}

void WebSocketManager::checkPingTimeout()
{
    m_webSocket->sendTextMessage(QString("{\"action\":\"ping\"}"));
    qint64 currentTimestamp = QDateTime::currentMSecsSinceEpoch();

    if (currentTimestamp - m_lastPing > kPermittedPingTimeout) {
        qDebug("checkPingTimeout timeout!!");
        handleConnectionError();
    } else {
        qDebug("checkPingTimeout NO timeout!!");
    }
}

QString socketErrorToString(QAbstractSocket::SocketError error)
{
    switch (error) {
        case QAbstractSocket::ConnectionRefusedError:
            return "The connection was refused by the peer (or timed out)";

        case QAbstractSocket::RemoteHostClosedError:
            return "The remote host closed the connection";

        case QAbstractSocket::HostNotFoundError:
            return "The host address was not found";

        case QAbstractSocket::SocketAccessError:
            return "The socket operation failed because the application lacked the required privileges";

        case QAbstractSocket::SocketResourceError:
            return "The local system ran out of resources (e.g., too many sockets)";

        case QAbstractSocket::SocketTimeoutError:
            return "The socket operation timed out";

        case QAbstractSocket::DatagramTooLargeError:
            return "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes)";

        case QAbstractSocket::NetworkError:
            return "An error occurred with the network (e.g., the network cable was accidentally plugged out)";

        case QAbstractSocket::AddressInUseError:
            return "The specified address is already in use and was set to be exclusive";

        case QAbstractSocket::SocketAddressNotAvailableError:
            return "The specified address does not belong to the host";

        case QAbstractSocket::UnsupportedSocketOperationError:
            return "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support)";

        case QAbstractSocket::ProxyAuthenticationRequiredError:
            return "The socket is using a proxy, and the proxy requires authentication";

        case QAbstractSocket::SslHandshakeFailedError:
            return "The SSL/TLS handshake failed, so the connection was closed";

        case QAbstractSocket::UnfinishedSocketOperationError:
            return "The last operation attempted has not finished yet (still in progress in the background)";

        case QAbstractSocket::ProxyConnectionRefusedError:
            return "Could not contact the proxy server because the connection to that server was denied";

        case QAbstractSocket::ProxyConnectionClosedError:
            return "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";

        case QAbstractSocket::ProxyConnectionTimeoutError:
            return "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase";

        case QAbstractSocket::ProxyNotFoundError:
            return "The proxy address was not found";

        case QAbstractSocket::ProxyProtocolError:
            return "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood";

        case QAbstractSocket::OperationError:
            return "An operation was attempted while the socket was in a state that did not permit it";

        case QAbstractSocket::SslInternalError:
            return "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library";

        case QAbstractSocket::SslInvalidUserDataError:
            return "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library";

        case QAbstractSocket::TemporaryError:
            return "A temporary error occurred (e.g., operation would block and socket is non-blocking)";

        case QAbstractSocket::UnknownSocketError:
        default:
            return "An unidentified websocket error occurred";
    }
}
