#ifndef ADDFIELDDIALOG_H
#define ADDFIELDDIALOG_H

#include <QObject>
#include <QBoxLayout>


#include <QDialog>

#include <QLineEdit>
#include <QPushButton>


class AddFieldDialog : public QDialog {
    Q_OBJECT

public:
    AddFieldDialog( QWidget* parent = 0 );
    ~AddFieldDialog();

    QString getInput() const;

signals:
    void applied();

private:
    QLineEdit* m_edit;
};

#endif // ADDFIELDDIALOG_H
