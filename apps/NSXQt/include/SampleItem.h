#ifndef SAMPLEITEM_H
#define SAMPLEITEM_H

#include <QStandardItem>

#include <string>

class SampleItem : public QStandardItem
{
public:
    explicit SampleItem(const std::string& name);

signals:

public slots:

};

#endif // SAMPLEITEM_H
