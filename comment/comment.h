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
    void setInformerId(int informerId);
    void setCreated(int created);
    void setModified(int modified);

private:
    QString m_commentHTML;
    int m_informerId;
    int m_created;
    int m_modified;

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
