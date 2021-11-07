#ifndef TICKET_H
#define TICKET_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "defaults.h"
#include "kazoo_auth/kazoo_auth.h"

#include "callerdatawindow/callerdatawindow.h"

class Ticket : public QWidget {
    Q_OBJECT
  public:
    Ticket(QString, int, int, int, QWidget* parent = nullptr);
  private:
    QVBoxLayout* ticketLayoutV = new QVBoxLayout(this);
    QWidget* labelWidget = new QWidget();
    QWidget* messagesWidget = nullptr;
    QLabel* ticketLabel;
    QString m_ticket_subject;
    QJsonValue m_messagesDataValue;
    int m_ticketId;
    int m_ticket_created;
    int m_ticket_modified;
    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = new QNetworkAccessManager(this);

    void retrieveMessagesList();
    void retrieveMessagesListFinished();
    void handleConnectionError();

  signals:
    void linkActivated(QString );

  private slots:
    void on_linkActivated(QString );

};


#endif // TICKET_H
