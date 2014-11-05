#ifndef DETECTORITEM_H
#define DETECTORITEM_H

#include <QStandardItem>

#include <string>

class DetectorItem : public QStandardItem
{
public:
    explicit DetectorItem(const std::string& name);

signals:

public slots:

};

#endif // DETECTORITEM_H
