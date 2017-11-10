#include "NormalizePeaksItem.h"

NormalizePeaksItem::NormalizePeaksItem(nsx::sptrExperiment experiment) : TreeItem(experiment)
{
    setText("Normalize peaks");
    setEditable(false);
    setSelectable(false);
}
