// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#ifndef NSXTOOL_PEAKFINDMODEL_H_
#define NSXTOOL_PEAKFINDMODEL_H_

#include <QObject>
#include <QStandardItemModel>
#include <QStandardItem>

class PeakFindModel: public QStandardItemModel {
    Q_OBJECT

public:
    PeakFindModel(QObject* parent = nullptr);
    ~PeakFindModel();


private:

};

#endif
