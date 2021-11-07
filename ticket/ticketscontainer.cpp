#include "ticketscontainer.h"
#include "ticket.h"

#include <QWidget>
#include <QApplication>

TicketsContainer::TicketsContainer(QObject* parent) : QObject(parent)
{

}

void TicketsContainer::addTickets(QVBoxLayout* container_layout, QJsonArray dataArray)
{
    QLayoutItem* child;
    child = container_layout->takeAt(0);

    while (child != 0) {
        if (child->widget()) {
            delete child->widget();
        }

        delete child;
        child = container_layout->takeAt(0);
    }

    for (int i = 0; i < dataArray.count(); i++) {
        QJsonObject AccountObj = dataArray.at(i).toObject();
        int ticketId = AccountObj.value("ticketid").toInt();
        int created = AccountObj.value("dateline").toInteger();
        int modified = AccountObj.value("lastactivity").toInteger();
        QString subject = AccountObj.value("subject").toString();
        Ticket* ticketBox = new Ticket(subject, created, modified, ticketId);
        container_layout->addWidget(ticketBox);
    }
}


