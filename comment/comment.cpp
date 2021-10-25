#include "comment.h"
#include <QFocusEvent>
#include <QMenu>
#include <QAction>
#include <QAction>

void Comment::focusInEvent(QFocusEvent* e)
{
    qDebug() << "\n focusInEvent 1 " << e << "\n";
    if (e->reason() == Qt::MouseFocusReason)
    {
        qDebug() << "\n focusInEvent 2 \n";
        this->setFixedHeight( 200 + 3 );
        // Resize the geometry -> resize(bigWidth,bigHeight);
    }
    //    QTextEdit::focusInEvent(e);
}

void Comment::focusOutEvent(QFocusEvent* e)
{
    qDebug() << "\n focusOutEvent 1 " << e << "\n";
    if (e->reason() == Qt::MouseFocusReason)
    {
        qDebug() << "\n focusOutEvent 2 \n";
        this->setFixedHeight( 100 + 3 );

    } else
        QTextEdit::focusInEvent(e);
}


void Comment::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QMenu *stdMenu = createStandardContextMenu();
        if (this->isReadOnly()) {
            stdMenu->addAction(tr("Edit"), this, SLOT(EditActionSlot()));
            stdMenu->addAction(tr("Delete"), this, SLOT(DeleteActionSlot()));
        } else {
            stdMenu->addAction(tr("Save"), this, SLOT(SaveActionSlot()));
            stdMenu->addAction(tr("Cancel"), this, SLOT(CancelActionSlot()));
        }
        stdMenu->exec(event->globalPosition().toPoint());
        delete stdMenu;
    }
    else
        QTextEdit::mousePressEvent(event);
}

void Comment::SaveActionSlot()
{
    qDebug() << "\n Comment::SaveActionSlot() \n";
    qDebug() << "\n Comment::SaveActionSlot() this->isReadOnly() 1 : "  << this->isReadOnly() << "\n";
    this->setReadOnly(true);
    qDebug() << "\n Comment::SaveActionSlot() this->isReadOnly() 2 : "  << this->isReadOnly() << "\n";
}

void Comment::CancelActionSlot()
{
    qDebug() << "\n Comment::CancelActionSlot() \n";
    this->setReadOnly(true);
    this->setHtml(m_commentHTML);
}

void Comment::EditActionSlot()
{
    qDebug() << "\n Comment::EditActionSlot() \n";
    qDebug() << "\n Comment::EditActionSlot() this->isReadOnly() 1 : "  << this->isReadOnly() << "\n";
    this->setReadOnly(false);
    qDebug() << "\n Comment::EditActionSlot() this->isReadOnly() 2 : "  << this->isReadOnly() << "\n";

}
void Comment::DeleteActionSlot()
{
    qDebug() << "\n Comment::DeleteActionSlot() \n";
}

void Comment::setCommentHTML(QString commentHTML)
{
    m_commentHTML = commentHTML;
    this->setHtml(m_commentHTML);
}
