#include "Tree/NormalizePeaksItem.h"

NormalizePeaksItem::NormalizePeaksItem(Experiment* experiment) : TreeItem(experiment)
{
    setText("Normalize peaks");
    setEditable(false);
    setSelectable(false);
}
