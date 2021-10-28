#include "alltaskslist.h"
#include "ui_alltaskslist.h"

#include "comment/commentscontainer.h"

#include <QPushButton>

static const char* const kGetAllCommentsQuery =
        "/accounts/%1/zzhds/hd_comments?md5=%3";

AllTasksList::AllTasksList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AllTasksList)
{
    ui->setupUi(this);

//    QWidget * widget = new QWidget();
//    QPushButton  * leftBut = new QPushButton("Left");
//    QPushButton  * rightBut = new QPushButton("Right");
//    QGridLayout * layout = new QGridLayout(widget);
//    layout->addWidget(leftBut,0,0,0,0,Qt::AlignVCenter | Qt::AlignLeft);
//    layout->addWidget(rightBut,0,0,0,0,Qt::AlignVCenter | Qt::AlignRight);
//    this->ui->statusbar->addWidget(widget,1);

//    retrieveCommentsList();
}

AllTasksList::~AllTasksList()
{
    delete ui;
}

void AllTasksList::retrieveCommentsList()
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
    url.append(kGetAllCommentsQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &AllTasksList::retrieveCommentsListFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &AllTasksList::handleConnectionError);

}

void AllTasksList::retrieveCommentsListFinished()
{
    qDebug() << "\n Inside CallerDataWindow::retrieveCommentsListFinished \n";
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray data = reply->readAll();
    reply->deleteLater();

    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(data, &error);

    if (error.error != QJsonParseError::NoError) {
        qDebug() << "\n Inside CallerDataWindow::retrieveCommentsListFinished ERROR!!!! \n";
        return;
    }

    m_commentsDataValue = document.object().value("data");

    if (m_commentsDataValue.isArray()) {
        QJsonArray dataArray = m_commentsDataValue.toArray();
        CommentsContainer* commentContainer = new CommentsContainer;
        commentContainer->addComments(ui->allCommentsLayout, dataArray, this);
    }

}

void AllTasksList::handleConnectionError()
{
    qWarning("AllTasksList::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "\n AllTasksList::handleConnectionError Reply->errorString() : " <<
                reply->errorString() << "\n";
    qDebug() << "\n AllTasksList::handleConnectionError Reply->error() : " <<
                reply->error() << "\n";
}

void AllTasksList::onCommentUpdated(int commentId)
{
    qDebug() << "AllTasksList::onCommentUpdated commentId: " << commentId << "\n";
    retrieveCommentsList();
}
