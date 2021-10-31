#include "conpamynamedialog.h"


ConpamyNameDialog::ConpamyNameDialog( QWidget* parent ) : QDialog( parent ) {
    this->setWindowTitle("Set company name");
    QBoxLayout* layout = new QHBoxLayout;
    m_edit = new QLineEdit;
    layout->addWidget( m_edit );

    QPushButton* okBtn = new QPushButton( "OK" );
    connect( okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    layout->addWidget( okBtn );

    //    QPushButton* applyBtn = new QPushButton( "Apply" );
    //    connect( applyBtn, SIGNAL( clicked() ), SIGNAL( applied() ) );
    //    layout->addWidget( applyBtn);

    QPushButton* cancelBtn = new QPushButton( "Cancel" );
    connect( cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    layout->addWidget( cancelBtn );

    setLayout( layout );
}

ConpamyNameDialog::~ConpamyNameDialog() {
}

QString ConpamyNameDialog::getInput() const {
    return m_edit->text();
}
