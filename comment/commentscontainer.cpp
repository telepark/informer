#include "commentscontainer.h"

#include <QWidget>
#include <QApplication>

#include "alltaskslistwindow/alltaskslist.h"


CommentsContainer::CommentsContainer(QObject *parent) : QObject(parent)
{

}

void CommentsContainer::addComments(QVBoxLayout* container_layout, QJsonArray dataArray, QMainWindow* parent_window)
{
    QLayoutItem* child;
    child=container_layout->takeAt(0);
    while(child != 0)
    {
        if(child->widget())
            delete child->widget();
        delete child;
        child=container_layout->takeAt(0);
    }

    for (int i = 0; i < dataArray.count(); i++) {
        QJsonObject AccountObj = dataArray.at(i).toObject();
        QWidget* widget = new QWidget();
        Comment *commentBox = new Comment(widget);
        int commentId = AccountObj.value("comment_id").toInt();
        commentBox->setAcceptRichText(true);
        commentBox->setReadOnly(true);
        commentBox->setContextMenuPolicy(Qt::CustomContextMenu);
        commentBox->setCommentHTML(AccountObj.value("comment_html").toString());
        commentBox->setInformerId(AccountObj.value("informer_id").toInt());
        commentBox->setCommentId(commentId);
        commentBox->setCreated(AccountObj.value("created").toInt());
        commentBox->setModified(AccountObj.value("modified").toInt());

        QLocale locale(QLocale("ru_RU"));
        QDateTime comment_date;
        comment_date.fromSecsSinceEpoch(AccountObj.value("created").toInteger());
        QString comment_label_string =
                locale.toString(comment_date.fromSecsSinceEpoch(AccountObj.value("modified").toInteger()))
                + " (создано: "
                + locale.toString(comment_date.fromSecsSinceEpoch(AccountObj.value("created").toInteger()), "yyyy-M-d")
                + ")";
        QLabel *comment_label = new QLabel(comment_label_string, widget);
        QVBoxLayout* comment_layout = new QVBoxLayout(widget);
        comment_layout->addWidget(comment_label);
        comment_layout->addWidget(commentBox);
        container_layout->addWidget(widget);
        connect(commentBox, SIGNAL(commentUpdated(int)), parent_window, SLOT(onCommentUpdated(int)));

        foreach (QWidget *w, QApplication::topLevelWidgets())
            if (AllTasksList* allTaskWin = qobject_cast<AllTasksList*>(w))
                connect(commentBox, SIGNAL(commentUpdated(int)), allTaskWin, SLOT(onCommentUpdated(int)));
    }
}
