#include "addfielddialog.h"

AddFieldDialog::AddFieldDialog( QWidget* parent ) : QDialog( parent ) {
    QBoxLayout* layout = new QHBoxLayout;
    m_edit = new QLineEdit;
    layout->addWidget( m_edit );

    QPushButton* okBtn = new QPushButton( "OK" );
    connect( okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    layout->addWidget( okBtn );

    QPushButton* cancelBtn = new QPushButton( "Cancel" );
    connect( cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    layout->addWidget( cancelBtn );

    setLayout( layout );
}

AddFieldDialog::~AddFieldDialog() {
}

QString AddFieldDialog::getInput() const {
    return m_edit->text();
}
