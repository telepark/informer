#include "messagescontainer.h"

void MessagesContainer::addMessages(QVBoxLayout* container_layout, QJsonArray dataArray)
{
    for (int i = 0; i < dataArray.count(); i++) {
        QJsonObject AccountObj = dataArray.at(i).toObject();
        QWidget* widget = new QWidget();
        QWidget* labelWidget = new QWidget();
        Message* messageBox = new Message(widget);
        int messageId = AccountObj.value("message_id").toInt();
        messageBox->setAcceptRichText(true);
        messageBox->setReadOnly(true);
        messageBox->setContextMenuPolicy(Qt::CustomContextMenu);
        messageBox->setMessageHTML(AccountObj.value("contents").toString());
        messageBox->setMessageId(messageId);
        messageBox->setCreated(AccountObj.value("dateline").toInt());

        QLocale locale(QLocale("ru_RU"));
        QDateTime message_date;
        message_date.fromSecsSinceEpoch(AccountObj.value("created").toInteger());
        QString message_label_string =
            locale.toString(message_date.fromSecsSinceEpoch(AccountObj.value("dateline").toInteger()));
        QLabel* message_label = new QLabel(message_label_string, labelWidget);
        QVBoxLayout* message_layout = new QVBoxLayout(widget);
        QHBoxLayout* labels_layout = new QHBoxLayout(labelWidget);

        labels_layout->addWidget(message_label);
        message_layout->addWidget(labelWidget);
        message_layout->addWidget(messageBox);
        container_layout->addWidget(widget);
    }
}
