#ifndef SOURCEITEM_H
#define SOURCEITEM_H

#include <string>
#include "InspectableTreeItem.h"

class QWidget;
namespace SX{
    namespace Instrument{
        class Experiment;
    }
}
class Experiment;

class SourceItem : public InspectableTreeItem
{
public:
    explicit SourceItem(SX::Instrument::Experiment* experiment);
    QWidget* inspectItem();
};

#endif // SOURCEITEM_H
