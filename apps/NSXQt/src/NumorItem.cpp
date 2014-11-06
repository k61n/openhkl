#include "NumorItem.h"

NumorItem::NumorItem(Experiment* experiment) : TreeItem(experiment)
{
    setText("Numor");

    setEditable(false);

    setDragEnabled(true);
    setDropEnabled(true);
}
