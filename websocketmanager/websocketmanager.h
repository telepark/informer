#ifndef WEBSOCKETMANAGER_H
#define WEBSOCKETMANAGER_H

#include <QObject>
#include <QHash>
#include <QSet>
#include <QStringList>

class QNetworkAccessManager;
class QWebSocket;
class QSettings;
class QTimer;

class WebSocketManager : public QObject {
    Q_OBJECT
  public:
    explicit WebSocketManager(QObject* parent = 0);
    ~WebSocketManager();

  public slots:
    void start();
    void stop();

  private:
    void processWsData(const QString& data);
    void processChannelCreate(const QJsonObject& args);
    void processChannelAnswer(const QJsonObject& args);
    void processChannelDestroy(const QJsonObject& args);

    bool isSupportCallDirection(const QString& callDirection);

    QNetworkAccessManager* m_nam = nullptr;
    QWebSocket* m_webSocket = nullptr;
    QTimer* m_timer = nullptr;
    QSettings* m_settings = nullptr;
    qint64 m_lastPing = 0;

    QString m_authToken;
    QString m_accountId;
    QStringList m_devices;
    QSet<QString> m_callerIdsSet;


  signals:
    void channelCreated(const QString& callId, const QString& callerIdName,
                        const QString& callerIdNumber, const QString& callerDialed);
    void channelAnswered(const QString& callId);
    void channelAnsweredAnother(const QString& callId, const QString& calleeNumber,
                                const QString& calleeName);
    void channelDestroyed(const QString& callId);

    void connectionError();
    void connected();

  private slots:
    void retrieveDevicesFinished();
    void retrieveWsAddressFinished();

    void webSocketConnected();
    void webSocketDisconnected();
    void webSocketTextFrameReceived(const QString& frame);

    void checkPingTimeout();
    void handleConnectionError();
};

#endif // WEBSOCKETMANAGER_H
