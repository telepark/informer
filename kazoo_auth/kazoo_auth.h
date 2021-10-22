#ifndef KAZOOAUTH_H
#define KAZOOAUTH_H

#include <QObject>
#include <QHash>
#include <QStringList>

#include "singleton.h"

class QNetworkAccessManager;
class QSettings;
class QTimer;

class KazooAuth : public QObject {
    Q_OBJECT

  public:
    QString authToken();
    QString accountId();
    QString ownerId();

  private:
    explicit KazooAuth(QObject* parent = 0);

  private:
    QNetworkAccessManager* m_nam = nullptr;
    QTimer* m_token_check_timer = nullptr;
    QSettings* m_settings = nullptr;
    qint64 m_lastPing = 0;

    QString m_authToken;
    QString m_ownerId;
    QString m_accountId;


    void retrieveAuthToken();

  signals:
    void connectionError();
    void kazooTokenObtained();
    void KazooTokenChanged();

  public slots:
    void start();
    void stop();

  private slots:
    void retrieveAuthTokenFinished();
    void checkTokenAliveFinished();
    void handleConnectionError();
    void checkTokenAlive();

    friend class Singleton<KazooAuth>;
};

#define KAZOOAUTH Singleton<KazooAuth>::instance()

#endif // KAZOOAUTH_H
