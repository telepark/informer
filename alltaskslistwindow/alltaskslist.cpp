#include "alltaskslist.h"
#include "ui_alltaskslist.h"

AllTasksList::AllTasksList(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::AllTasksList)
{
    ui->setupUi(this);
}

AllTasksList::~AllTasksList()
{
    delete ui;
}
