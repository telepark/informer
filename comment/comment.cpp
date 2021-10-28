#include "comment.h"
#include <QFocusEvent>
#include <QMenu>
#include <QAction>
#include <QAction>

static const char* const kCommentQuery =
        "/accounts/%1/zzhds/hd_comments/%2";

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
    qDebug() << "\n Comment::SaveActionSlot() m_informerId : "  << m_informerId << "\n";
    qDebug() << "\n Comment::SaveActionSlot() m_commentId : "  << m_commentId << "\n";

    qDebug() << "\n Comment::SaveActionSlot() this->toHtml() : "  << this->toHtml() << "\n";
    qDebug() << "\n Comment::SaveActionSlot() this->toPlainText() : "  << this->toPlainText() << "\n";
    updateComment();

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
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    qDebug() << "\n 555 settings: " << m_settings->value("info_url", kInfoUrl) << "\n";

    QString url(m_settings->value("info_url", kInfoUrl).toString());

    url.append(kCommentQuery);
    qDebug() << "\n Comment::DeleteActionSlot m_commentId: " << m_commentId << "\n";

    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(),QString::number(m_commentId))));
    qDebug() << "\n Comment::DeleteActionSlot req.url(): \n" << req.url() << "\n";

    QNetworkReply* reply = m_nam->deleteResource(req);
    connect(reply, &QNetworkReply::finished,
            this, &Comment::updateCommentFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &Comment::handleConnectionError);
}

void Comment::setCommentHTML(QString commentHTML)
{
    m_commentHTML = commentHTML;
    this->setHtml(m_commentHTML);
}

void Comment::setInformerId(int informerId)
{
    m_informerId = informerId;
}

void Comment::setCreated(int created)
{
    m_created = created;
}
void Comment::setModified(int modified)
{
    m_modified = modified;
}

void Comment::setCommentId(int commentId)
{
    m_commentId = commentId;
}

void Comment::updateComment()
{
    qDebug() << "\n Comment::updateComment: " << this->toPlainText() <<
                "\n";
    QJsonObject jsonObject;
    QJsonObject jsonData;
    //    jsonData["credentials"] = hash.data();
    qDebug() << "\n Putting  m_commentId to jsonData: " << m_commentId << "\n";
    jsonData["comment_id"] = m_commentId;
    qDebug() << "\n Putting  comment_text to jsonData: " << this->toPlainText() << "\n";
    jsonData["comment_text"] = this->toPlainText();
    qDebug() << "\n Putting  comment_html to jsonData: " << this->toHtml() << "\n";
    jsonData["comment_html"] = this->toHtml();
    jsonObject["data"] = jsonData;
    qDebug() << "\n 111 \n";
    QJsonDocument jsonDocument(jsonObject);
    qDebug() << "\n 222 \n";
    QByteArray json = jsonDocument.toJson();
    qDebug() << "\n 333 \n";
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    qDebug() << "\n 444 \n";
    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    qDebug() << "\n 555 \n";

    qDebug() << "\n 555 settings: " << m_settings->value("info_url", kInfoUrl) << "\n";

    QString url(m_settings->value("info_url", kInfoUrl).toString());
    qDebug() << "\n 666 \n";

    url.append(kCommentQuery);
    qDebug() << "\n 777 \n";
    qDebug() << "\n Comment::updateComment m_commentId: " << m_commentId << "\n";

    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), QString::number(m_commentId))));
    qDebug() << "\n Comment::updateComment req.url(): \n" << req.url() << "\n";

    QNetworkReply* reply = m_nam->put(req, json);
    connect(reply, &QNetworkReply::finished,
            this, &Comment::updateCommentFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &Comment::handleConnectionError);
}

void Comment::updateCommentFinished()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        return;
    }
    qDebug() << "\n DocumentObject: " << document.object() << "\n";
    emit commentUpdated(this->m_commentId);
}

void Comment::handleConnectionError()
{
    qWarning("InformerDialog::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "\n InformerDialog::handleConnectionError Reply->errorString() : " <<
                reply->errorString() << "\n";
    qDebug() << "\n InformerDialog::handleConnectionError Reply->error() : " <<
                reply->error() << "\n";
}
