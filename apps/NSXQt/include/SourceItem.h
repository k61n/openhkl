#ifndef SOURCEITEM_H
#define SOURCEITEM_H

#include <QStandardItem>

#include <string>

class SourceItem : public QStandardItem
{
public:
    explicit SourceItem(const std::string& name);

signals:

public slots:

};

#endif // SOURCEITEM_H
