#ifndef COMMENT_H
#define COMMENT_H

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

class Comment : public QTextEdit
{
    Q_OBJECT

public:
    explicit Comment(QWidget *parent = nullptr): QTextEdit(parent){
        if (m_settings) {
            m_settings->deleteLater();
        }

        m_settings = new QSettings(dataDirPath() + "/settings.ini",
                                   QSettings::IniFormat,
                                   this);
//        QObject::connect(this,&Comment::commentUpdated, *parent, &CallerDataWindow::onCommentUpdated);
    }
    void setCommentHTML(QString commentHTML);
    void setInformerId(int informerId);
    void setCreated(int created);
    void setModified(int modified);
    void setCommentId(int commentId);

private:
    QString m_commentHTML;
    QString m_informerName;
    int m_informerId;
    int m_created;
    int m_modified;
    int m_commentId;
    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = new QNetworkAccessManager(this);

    void updateComment();
    void updateCommentFinished();
    void handleConnectionError();

protected:
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);
    void mousePressEvent(QMouseEvent *e);

signals:
    void commentUpdated(int);

public  slots:
    void SaveActionSlot();
    void CancelActionSlot();
    void EditActionSlot();
    void DeleteActionSlot();

};

#endif // COMMENT_H
