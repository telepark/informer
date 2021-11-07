#include "message.h"
#include <QFocusEvent>
#include <QMenu>
#include <QAction>
#include <QAction>

void Message::focusInEvent(QFocusEvent* e)
{
    if (e->reason() == Qt::MouseFocusReason) {
        this->setFixedHeight( 200 + 3 );
    } else {
        QTextEdit::focusInEvent(e);
    }
}

void Message::focusOutEvent(QFocusEvent* e)
{
    if (e->reason() == Qt::MouseFocusReason) {
        this->setFixedHeight( 100 + 3 );
    } else {
        QTextEdit::focusInEvent(e);
    }
}

void Message::setMessageHTML(QString messageHTML)
{
    m_messageHTML = messageHTML;
    this->setHtml(m_messageHTML);
}

void Message::setCreated(int created)
{
    m_created = created;
}

void Message::setMessageId(int messageId)
{
    m_messageId = messageId;
}

