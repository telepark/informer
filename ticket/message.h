#ifndef MESSAGE_H
#define MESSAGE_H

#include <QTextEdit>
#include <QFocusEvent>
#include <QMouseEvent>
#include <QSettings>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include "defaults.h"
#include "kazoo_auth/kazoo_auth.h"

#include "callerdatawindow/callerdatawindow.h"

class Message : public QTextEdit {
    Q_OBJECT

  public:
    explicit Message(QWidget* parent = nullptr): QTextEdit(parent) {
    }

    void setMessageHTML(QString messageHTML);
    void setCreated(int created);
    void setMessageId(int messageId);

  private:
    QString m_messageHTML;
    QString m_informerName;
    int m_informerId;
    int m_created;
    int m_messageId;

  protected:
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);

  signals:

  public  slots:

};

#endif // MESSAGE_H
