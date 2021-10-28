#include "comment.h"
#include <QFocusEvent>
#include <QMenu>
#include <QAction>
#include <QAction>

static const char* const kCommentQuery = "/accounts/%1/zzhds/hd_comments/%2";

void Comment::focusInEvent(QFocusEvent* e)
{
    if (e->reason() == Qt::MouseFocusReason)
    {
        this->setFixedHeight( 200 + 3 );
    }
    else
        QTextEdit::focusInEvent(e);
}

void Comment::focusOutEvent(QFocusEvent* e)
{
    if (e->reason() == Qt::MouseFocusReason)
    {
        this->setFixedHeight( 100 + 3 );
    }
    else
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
    this->setReadOnly(true);
    updateComment();
}

void Comment::CancelActionSlot()
{
    this->setReadOnly(true);
    this->setHtml(m_commentHTML);
}

void Comment::EditActionSlot()
{
    this->setReadOnly(false);
}
void Comment::DeleteActionSlot()
{
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    QString url(m_settings->value("info_url", kInfoUrl).toString());
    url.append(kCommentQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(),QString::number(m_commentId))));
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
    QJsonObject jsonObject;
    QJsonObject jsonData;
    //    jsonData["credentials"] = hash.data();
    jsonData["comment_id"] = m_commentId;
    jsonData["comment_text"] = this->toPlainText();
    jsonData["comment_html"] = this->toHtml();
    jsonObject["data"] = jsonData;
    QJsonDocument jsonDocument(jsonObject);
    QByteArray json = jsonDocument.toJson();
    QNetworkRequest req;
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    /* Setup SSL */
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    QString url(m_settings->value("info_url", kInfoUrl).toString());
    url.append(kCommentQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), QString::number(m_commentId))));
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
    emit commentUpdated(this->m_commentId);
}

void Comment::handleConnectionError()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qWarning() << "\n InformerDialog::handleConnectionError ---  error: " << reply->errorString() << "\n";
}
