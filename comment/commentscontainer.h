#ifndef COMMENTSCONTAINER_H
#define COMMENTSCONTAINER_H

#include <QObject>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>

#include "comment.h"

class CommentsContainer : public QObject
{
    Q_OBJECT
public:
    explicit CommentsContainer(QObject *parent = nullptr);
    void addComments(QVBoxLayout* , QJsonArray , QMainWindow* );

signals:
    void linkActivated(QString );

private slots:
    void on_linkActivated(QString );

};

#endif // COMMENTSCONTAINER_H
