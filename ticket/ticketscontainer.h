#ifndef TICKETSCONTAINER_H
#define TICKETSCONTAINER_H

#include <QObject>
#include <QLayoutItem>
#include <QVBoxLayout>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>

#include "ticket.h"

class TicketsContainer : public QObject {
    Q_OBJECT
  public:
    explicit TicketsContainer(QObject* parent = nullptr);
    void addTickets(QVBoxLayout*, QJsonArray );

  private:
    //    AllTasksList* m_allTaskWin = nullptr;



};

#endif // TICKETSCONTAINER_H
