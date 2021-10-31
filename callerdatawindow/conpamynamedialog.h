#ifndef CONPAMYNAMEDIALOG_H
#define CONPAMYNAMEDIALOG_H
#include <QObject>
#include <QBoxLayout>


#include <QDialog>

#include <QLineEdit>
#include <QPushButton>


class ConpamyNameDialog : public QDialog {
    Q_OBJECT

public:
    ConpamyNameDialog( QWidget* parent = 0 );
    ~ConpamyNameDialog();

    QString getInput() const;

signals:
    void applied();

private:
    QLineEdit* m_edit;
};

#endif // CONPAMYNAMEDIALOG_H






