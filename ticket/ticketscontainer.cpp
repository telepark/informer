#include "ticketscontainer.h"

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
        QWidget* widget = new QWidget();
        QWidget* labelWidget = new QWidget();
        int ticketId = AccountObj.value("ticketid").toInt();
        //        Comment* commentBox = new Comment(widget);
        //        commentBox->setAcceptRichText(true);
        //        commentBox->setReadOnly(true);
        //        commentBox->setContextMenuPolicy(Qt::CustomContextMenu);
        //        commentBox->setCommentHTML(AccountObj.value("comment_html").toString());
        //        commentBox->setInformerId(AccountObj.value("informer_id").toInt());
        //        commentBox->setCommentId(commentId);
        //        commentBox->setCreated(AccountObj.value("created").toInt());
        //        commentBox->setModified(AccountObj.value("modified").toInt());

        QLocale locale(QLocale("ru_RU"));
        QDateTime comment_date;
        comment_date.fromSecsSinceEpoch(AccountObj.value("created").toInteger());
        QString comment_label_string =
            locale.toString(comment_date.fromSecsSinceEpoch(AccountObj.value("lastactivity").toInteger()),
                            "yyyy-MM-dd")
            + " (создано: "
            + locale.toString(comment_date.fromSecsSinceEpoch(AccountObj.value("dateline").toInteger()),
                              "yyyy-MM-dd")
            + ")";
        QLabel* comment_label = new QLabel(comment_label_string, labelWidget);
        QVBoxLayout* comment_layout = new QVBoxLayout(widget);
        QHBoxLayout* labels_layout = new QHBoxLayout(labelWidget);


        QString TicketId = QString::number(AccountObj.value("ticketid").toInt());
        QString subject = AccountObj.value("subject").toString();
        QString informerName_label_string =
            "<a href='" + TicketId + "'>"
            + subject
            + "</a>";
        QLabel* ticket_subject_label = new QLabel(informerName_label_string, labelWidget);

        ticket_subject_label->setTextFormat(Qt::RichText);
        ticket_subject_label->setOpenExternalLinks(false);
        ticket_subject_label->setStyleSheet("font-weight: bold; color: red");
        connect(ticket_subject_label, SIGNAL(linkActivated(QString)), this,
                SLOT(on_linkActivated(QString)));
        labels_layout->addWidget(ticket_subject_label);



        labels_layout->addWidget(comment_label);
        comment_layout->addWidget(labelWidget);
        //        comment_layout->addWidget(commentBox);
        container_layout->addWidget(widget);
        //        connect(commentBox, SIGNAL(commentUpdated(int)), parent_window, SLOT(onCommentUpdated(int)));
        //        connect(commentBox, SIGNAL(commentUpdated(int)), m_allTaskWin, SLOT(onCommentUpdated(int)));
    }
}
