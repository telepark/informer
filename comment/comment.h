#ifndef COMMENT_H
#define COMMENT_H

#include <QTextEdit>
#include <QFocusEvent>
#include <QMouseEvent>


class Comment : public QTextEdit
{
    Q_OBJECT

public:
    explicit Comment(QWidget *parent = nullptr): QTextEdit(parent){  }
    void setCommentHTML(QString commentHTML);

private:
    QString m_commentHTML;

protected:
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);
    void mousePressEvent(QMouseEvent *e);

public  slots:
    void SaveActionSlot();
    void CancelActionSlot();
    void EditActionSlot();
    void DeleteActionSlot();

};

#endif // COMMENT_H
