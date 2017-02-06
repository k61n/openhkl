#include "models/NormalizePeaksItem.h"

NormalizePeaksItem::NormalizePeaksItem(std::shared_ptr<Experiment> experiment) : TreeItem(experiment)
{
    setText("Normalize peaks");
    setEditable(false);
    setSelectable(false);
}
