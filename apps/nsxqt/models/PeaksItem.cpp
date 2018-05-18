#include <QIcon>
#include <QFileInfo>
#include <QJsonArray>
#include <QStandardItem>
#include <QString>

#include <nsxlib/DataReaderFactory.h>
#include <nsxlib/DataSet.h>
#include <nsxlib/Experiment.h>
#include <nsxlib/IDataReader.h>
#include <nsxlib/Logger.h>
#include <nsxlib/RawDataReader.h>

#include "PeaksItem.h"
#include "PeakListItem.h"
#include "NumorItem.h"

PeaksItem::PeaksItem(): TreeItem()
{
    setText("Peaks");
    QIcon icon(":/resources/peakListIcon.png");
    setIcon(icon);
    setEditable(false);
    setSelectable(false);
}

PeakListItem* PeaksItem::createPeaksItem(const char* name)
{
    auto item = new PeakListItem;
    item->setText(name);
    appendRow(item);    
    return item;
}

nsx::PeakList PeaksItem::selectedPeaks()
{
    nsx::PeakList peaks;
    for (auto i = 0; i < rowCount(); ++i) {
        auto& list = dynamic_cast<PeakListItem&>(*child(i));

        if (list.checkState() != Qt::Checked) {
            continue;
        }

        for (auto&& peak: list.peaks()) {
            peaks.push_back(peak);
        }
    }
    return peaks;
}