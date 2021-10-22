#ifndef ALLTASKSLIST_H
#define ALLTASKSLIST_H

#include <QMainWindow>

namespace Ui {
class AllTasksList;
}

class AllTasksList : public QMainWindow
{
    Q_OBJECT

public:
    explicit AllTasksList(QWidget *parent = nullptr);
    ~AllTasksList();

private:
    Ui::AllTasksList *ui;
};

#endif // ALLTASKSLIST_H
