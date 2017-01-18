#ifndef AUTOINDEXEDPEAKSTABLEVIEW_H
#define AUTOINDEXEDPEAKSTABLEVIEW_H

#include <QTableView>

class AutoIndexedPeaksTableView : public QTableView
{
    Q_OBJECT
public:
    explicit AutoIndexedPeaksTableView(QWidget *parent = 0);

    void contextMenuEvent(QContextMenuEvent *);

public slots:

    void clearSelectedPeaks();
    void selectUnindexedPeaks();

};

#endif // AUTOINDEXEDPEAKSTABLEVIEW_H
