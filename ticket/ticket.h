#ifndef TICKET_H
#define TICKET_H

#include <QObject>
#include <QTextEdit>

class Ticket : public QTextEdit {
    Q_OBJECT
  public:
    Ticket();
};

#endif // TICKET_H
