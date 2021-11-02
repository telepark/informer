#ifndef INFORMERDIALOG_H
#define INFORMERDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QNetworkAccessManager>

#include "callerdatawindow/callerdatawindow.h"

namespace Ui {
class InformerDialog;
}

class QMouseEvent;

class InformerDialog : public QDialog {
    Q_OBJECT

  public:
    enum State {
        kStateRinging,
        kStateAnswered,
        kStateAnsweredAnother
    };

    explicit InformerDialog(QWidget* parent = 0);
    ~InformerDialog();

    void setState(State state);

    bool isAnsweredAnother() const;
    bool isAttached() const;

    void setCallee(const QString& calleeNumber, const QString& calleeName);
    void setCaller(const QString& callerIdName, const QString& callerIdNumber,
                   const QString& callerDialed);

    void retrieveCallerData();
    void retrieveCallerInfoFinished();
    void handleConnectionError();
    QString informationLabelText();

  public slots:
    void openCallerDataWindow();

  signals:
    void dialogAttached(bool attached);

  protected:
    // for moving frameless window
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

  private:
    Ui::InformerDialog* ui;
    QString m_companyName = "Unknown";
    CallerDataWindow* m_callerdatawindow;
    QString m_callerIdName = "";
    QString m_callerIdNumber = "";
    QString m_callerDialed = "";
    int m_informerId;

    QJsonObject m_kazooDataJObj = {};
    QSettings* m_settings = nullptr;
    QNetworkAccessManager* m_nam = nullptr;

    // for moving frameless window
    QPoint m_dragPosition;
    bool m_dragging;

  private slots:
    void processAttach(bool checked);
};

#endif // INFORMERDIALOG_H
