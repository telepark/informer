#include "ticket.h"

#include <QTextEdit>

#include "messagescontainer.h"

static const char* const kGetMessagesQuery =
    "/accounts/%1/zzhds/informer_ticket_messages?ticket_id=%2&md5=%3";

Ticket::Ticket(QString subject, int created, int modified, int ticketId, QWidget* parent)
{
    m_ticketId = ticketId;
    m_ticket_created = created;
    m_ticket_modified = modified;
    m_ticket_subject = subject;

    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);

    QLocale locale(QLocale("ru_RU"));
    QDateTime ticket_date;
    QString ticket_label_string =
        locale.toString(ticket_date.fromSecsSinceEpoch(m_ticket_modified));
    //        locale.toString(ticket_date.fromSecsSinceEpoch(m_ticket_modified),
    //                        "yyyy-MM-dd")
    //        + " (создано: "
    //        + locale.toString(ticket_date.fromSecsSinceEpoch(m_ticket_created),
    //                          "yyyy-MM-dd")
    //        + ")";
    ticketLabel = new QLabel(ticket_label_string, labelWidget);
    QHBoxLayout* labelLayout = new QHBoxLayout(labelWidget);

    QString informerName_label_string =
        "<a href='" + QString::number(m_ticketId) + "'>"
        + m_ticket_subject
        + "</a>";
    QLabel* ticket_subject_label = new QLabel(informerName_label_string, labelWidget);

    ticket_subject_label->setTextFormat(Qt::RichText);
    ticket_subject_label->setOpenExternalLinks(false);
    ticket_subject_label->setStyleSheet("font-weight: bold; color: red");
    connect(ticket_subject_label, SIGNAL(linkActivated(QString)), this,
            SLOT(on_linkActivated(QString)));
    labelLayout->addWidget(ticket_subject_label);
    labelLayout->addWidget(ticketLabel);
    ticketLayoutV->addWidget(labelWidget);
};

void Ticket::on_linkActivated(QString link)
{
    if (!messagesWidget) {
        qDebug() << "n inside if (!messagesWidget)\n";
        messagesWidget = new QWidget(this);

        retrieveMessagesList();
    }

    if (messagesWidget->isVisible()) {
        qDebug() << "\n About to hide messagesWidget->isVisible(): " << messagesWidget->isVisible() << "\n";
        messagesWidget->hide();
    } else {
        qDebug() << "\n About to show messagesWidget->isVisible(): " << messagesWidget->isVisible() << "\n";
        messagesWidget->show();
    }

}

void Ticket::retrieveMessagesList()
{
    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    QByteArray hashTemplate(KAZOOAUTH.accountId().toLatin1());
    hashTemplate.append(":");
    hashTemplate.append(m_settings->value("md5_hash", kMd5Hash).toByteArray());
    QByteArray hash = QCryptographicHash::hash(hashTemplate, QCryptographicHash::Md5).toHex();
    QNetworkRequest req;
    req.setRawHeader("X-Auth-Token", KAZOOAUTH.authToken().toLatin1());
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    //SSL
    QSslConfiguration config = req.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    QString url(m_settings->value("info_url", kInfoUrl).toString());
    url.append(kGetMessagesQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), QString::number(m_ticketId),
                            hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &Ticket::retrieveMessagesListFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &Ticket::handleConnectionError);
}

void Ticket::retrieveMessagesListFinished()
{
    qDebug() << "\n Inside CallerDataWindow::retrieveTicketsListFinished \n";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "\n Inside CallerDataWindow::retrieveTicketsListFinished ERROR!!!! \n";
        return;
    }

    m_messagesDataValue = document.object().value("data");

    if (m_messagesDataValue.isArray()) {

        QVBoxLayout* messagesLayoutV = new QVBoxLayout(messagesWidget);

        QJsonArray dataArray = m_messagesDataValue.toArray();
        qDebug() << "\n Ticket::retrieveMessagesListFinished dataArray: " << dataArray << "\n";

        MessagesContainer* messageContainer = new MessagesContainer;
        //        messageContainer->addMessages(ticketLayoutV, dataArray);
        messageContainer->addMessages(messagesLayoutV, dataArray);
        ticketLayoutV->addWidget(messagesWidget);

    }
}

void Ticket::handleConnectionError()
{
    qWarning("Ticket::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "\n Ticket::handleConnectionError Reply->errorString() : " <<
             reply->errorString() << "\n";
    qDebug() << "\n Ticket::handleConnectionError Reply->error() : " <<
             reply->error() << "\n";
}
