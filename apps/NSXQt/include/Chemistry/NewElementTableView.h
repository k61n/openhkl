#ifndef NEWELEMENTTABLEVIEW_H
#define NEWELEMENTTABLEVIEW_H

#include <QTableView>

class NewElementTableView : public QTableView
{
    Q_OBJECT
public:
    explicit NewElementTableView(QWidget *parent = 0);

    void dropEvent(QDropEvent *event);

signals:

public slots:

};

#endif // NEWELEMENTTABLEVIEW_H
