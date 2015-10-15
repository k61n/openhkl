#ifndef CONSTRAINTSMODEL_H
#define CONSTRAINTSMODEL_H

#include <vector>

#include <QStandardItemModel>

class ConstraintsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    ConstraintsModel(QObject *parent = 0);

    void setItem(int row, int column, QStandardItem *item);

signals:

public slots:

private:

    std::vector<QString> _possibleConstraints;

};

#endif // CONSTRAINTSMODEL_H
