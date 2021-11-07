#ifndef MESSAGESCONTAINER_H
#define MESSAGESCONTAINER_H

#include <QObject>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>

#include "message.h"
#include "alltaskslistwindow/alltaskslist.h"

class MessagesContainer : public QObject {
    Q_OBJECT
  public:
    explicit MessagesContainer(QObject* parent = nullptr) {};
    void addMessages(QVBoxLayout*, QJsonArray);
  private:

  signals:

  private slots:
};

#endif // MESSAGESCONTAINER_H
