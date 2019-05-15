
#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include <build/core/include/core/DataTypes.h>
#include "nsxgui/gui/models/experimentmodel.h"

extern class Session* gSession; //!< global handle for Session

class Session {
public:
	Session();
    ExperimentModel* selectedExperiment();
    ExperimentModel* selectExperiment(int);
    int selectedExperimentNum() { return selected; }
    ExperimentModel* experimentAt(int i) { return experiments.at(i); }
    int numExperiments() { return experiments.size(); }

    void createExperiment();
    void removeExperiment();

    void loadData();
    void loadRawData();
    void removeData();

private:
    QList<ExperimentModel*> experiments;
    int selected = -1;
};

#endif //SESSION_H
