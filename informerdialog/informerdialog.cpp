#include "informerdialog.h"
#include "ui_informerdialog.h"

#include "callerdatawindow/callerdatawindow.h"
#include "kazoo_auth/kazoo_auth.h"

#include "defaults.h"

#include <QMouseEvent>

#include <QDesktopServices>
#include <QScreen>
#include <QSettings>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>

static const char* const kStyleSheetRinging = "QDialog {\nbackground-color: #FFFFBF;\n}";
static const char* const kStyleSheetAnswered = "QDialog {\nbackground-color: #63C248;\n}";
static const char* const kStyleSheetAnsweredAnother = "QDialog {\nbackground-color: #FFBA66;\n}";
static const char* const kStyleSheetAttached = "QDialog {\nbackground-color: #BFDFFF;\n}";

static const char* const kCallerInfoQuery = "/accounts/%1/zzhds/cid_info?phone_number=%2&md5=%3";

InformerDialog::InformerDialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::InformerDialog)
{
    ui->setupUi(this);

    setWindowFlags(
#ifdef Q_OS_MAC
        Qt::SubWindow | // This type flag is the second point
#else
        Qt::Tool |
#endif
        Qt::FramelessWindowHint |
        Qt::WindowSystemMenuHint |
        Qt::WindowStaysOnTopHint
    );

    m_nam = new QNetworkAccessManager(this);

    connect(ui->closeToolButton, &QToolButton::clicked,
            this, &InformerDialog::hide);
    connect(ui->attachToolButton, &QToolButton::clicked,
            this, &InformerDialog::processAttach);
    connect(ui->openUrlToolButton, &QToolButton::clicked,
            this, &InformerDialog::openCallerDataWindow);
}

InformerDialog::~InformerDialog()
{
    delete ui;
}

QString InformerDialog::informationLabelText()
{
    QString tmpName = (m_companyName == "") ? "Неизвестный абонент" : m_companyName;
    return tmpName
           + "\n" + m_callerIdName
           + " (" + m_callerIdNumber + ")"
           + "\nНабран номер: " + m_callerDialed;
}


void InformerDialog::setCaller(const QString& callerIdName, const QString& callerIdNumber,
                               const QString& callerDialed)
{
    m_callerIdName = callerIdName;
    m_callerIdNumber = callerIdNumber;
    m_callerDialed = callerDialed;
    ui->informationLabel->setText(informationLabelText());
}

void InformerDialog::setCallee(const QString& calleeNumber, const QString& calleeName)
{
    QString text = ui->informationLabel->text();
    text.append("\nCallee number: " + calleeNumber + "\nCallee name: " + calleeName);
    ui->informationLabel->setText(text);
    adjustSize();
    QRect rect = screen()->availableGeometry();
    setGeometry(rect.width() - width(),
                rect.height() - height(),
                width(),
                height());
}

void InformerDialog::setState(State state)
{
    if (state == kStateAnswered) {
        setStyleSheet(kStyleSheetAnswered);
        ui->stateLabel->setText(tr("State: Answered"));
    } else if (state == kStateAnsweredAnother) {
        setStyleSheet(kStyleSheetAnsweredAnother);
        ui->stateLabel->setText(tr("State: Answered by another user"));
    } else if (state == kStateRinging) {
        setStyleSheet(kStyleSheetRinging);
        ui->stateLabel->setText(tr("State: Ringing"));
    }
}

bool InformerDialog::isAnsweredAnother() const
{
    return styleSheet() == kStyleSheetAnsweredAnother;
}

void InformerDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (rect().contains(event->pos())) {
            m_dragging = true;
            setCursor(Qt::SizeAllCursor);
            m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
        }
    }
}

void InformerDialog::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);

    m_dragging = false;
    setCursor(Qt::ArrowCursor);
}

void InformerDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (m_dragging) {
            move(event->globalPosition().toPoint() - m_dragPosition);
            event->accept();
        }
    }
}

bool InformerDialog::isAttached() const
{
    return ui->attachToolButton->isChecked();
}

void InformerDialog::processAttach(bool checked)
{
    emit dialogAttached(checked);

    if (checked) {
        setStyleSheet(kStyleSheetAttached);
        ui->stateLabel->setText(tr("State: Attached"));
    } else {
        ui->stateLabel->setText(tr("State: Detached"));
    }
}

void InformerDialog::openCallerDataWindow()
{
    m_callerdatawindow = new CallerDataWindow();
    m_callerdatawindow->show();
    m_callerdatawindow->setAccountId(m_callerAccountId);
    //    m_callerdatawindow->show();
    qDebug() << "\n Inside InformerDialog KAZOOAUTH.authToken(): " << KAZOOAUTH.authToken() << "\n";
    this->hide();
}

void InformerDialog::retrieveCallerData()
{
    if (m_settings) {
        m_settings->deleteLater();
    }

    m_settings = new QSettings(dataDirPath() + "/settings.ini",
                               QSettings::IniFormat,
                               this);
    QByteArray hashTemplate(m_callerIdNumber.toLatin1());
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
    url.append(kCallerInfoQuery);
    req.setUrl(QUrl(url.arg(KAZOOAUTH.accountId().toLatin1(), m_callerIdNumber, hash.data())));
    qDebug() << "\n req.url(): \n" << req.url() << "\n";
    QNetworkReply* reply = m_nam->get(req);
    connect(reply, &QNetworkReply::finished,
            this, &InformerDialog::retrieveCallerInfoFinished);
    connect(reply, &QNetworkReply::errorOccurred,
            this, &InformerDialog::handleConnectionError);
}

void InformerDialog::retrieveCallerInfoFinished()
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

    QJsonObject respData = document.object().value("data").toObject();
    QJsonObject accountInfo = respData.value("account_info").toObject();
    QString kazooAccountId = respData.value("kazoo_account_id").toString();

    qDebug() << "\n AccountInfo: " << accountInfo << "\n";

    QString account_name = accountInfo.value("name").toString();
    m_companyName = account_name;
    m_callerAccountId = kazooAccountId;
    ui->informationLabel->setText(informationLabelText());
}

void InformerDialog::handleConnectionError()
{
    qWarning("InformerDialog::handleConnectionError ---  error");
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    qDebug() << "\n InformerDialog::handleConnectionError Reply->errorString() : " <<
             reply->errorString() << "\n";
    qDebug() << "\n InformerDialog::handleConnectionError Reply->error() : " <<
             reply->error() << "\n";
}

