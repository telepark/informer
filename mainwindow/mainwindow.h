#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

#include "alltaskslistwindow/alltaskslist.h"

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;

class WebSocketManager;
class InformerDialog;
class DebugDialog;
class AccountLookupDialog;
//class AllTasksList;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void createTrayIcon();
    bool isCorrectSettings() const;

    Ui::MainWindow* ui;
    QSystemTrayIcon* m_trayIcon;

    QHash<QString, InformerDialog*> m_informerDialogsHash;
    QHash<QString, InformerDialog*> m_attachedDialogsHash;
    QHash<QString, QTimer*> m_timersHash;

    WebSocketManager* m_wsMan;
    AllTasksList* m_alltaskslist;

    DebugDialog* m_debugDialog = nullptr;
    AccountLookupDialog* m_accountLookupDialog = nullptr;
    //    AllTasksList* m_alltaskslist = new AllTasksList(this);
    //    AllTasksList* m_alltaskslist = nullptr;


private slots:
    void onChannelCreated(const QString& callId, const QString& callerIdName,
                          const QString& callerIdNumber, const QString& callerDialed);
    void onChannelAnswered(const QString& callId);
    void onChannelAnsweredAnother(const QString& callId, const QString& calleeNumber,
                                  const QString& calleeName);
    void onChannelDestroyed(const QString& callId);
    void handleWsConnectionError();
    void handleWsConnected();

    void saveSettings();
    void loadSettings();

    void timeout();

    void processDialogFinished();
    void processDialogAttached(bool attached);

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    void processUpdateAvailable();
    void processNoUpdate();
    void updateApp();
#endif
    void showOpenedTasksDialog();
    void showAccountLookupDialog();
    void showDebugDialog();
    void closeAllPopups();
    void quit();
};

#endif // MAINWINDOW_H
