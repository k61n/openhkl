
#ifndef SAMPLESHAPEPROPERTIES_H
#define SAMPLESHAPEPROPERTIES_H

#include "nsxgui/qcr/widgets/views.h"
#include "nsxgui/qcr/widgets/controls.h"
#include "nsxgui/qcr/widgets/actions.h"
#include <QTableWidget>

class SampleShapeProperties : public QcrWidget {
public:
    SampleShapeProperties();
private:
    void onRemake();
    void clear();
    void facesChanged();
    void edgesChanged();
    void verticesChanged();
    void volumeChanged();
    QcrLineEdit* movie;
    QcrLineEdit* volume;
    QcrLineEdit* faces;
    QcrLineEdit* edges;
    QcrLineEdit* vertices;
    QcrTextTriggerButton* loadMovieButton;
    QTableWidget* sampleGoniometer;
};

#endif //SAMPLESHAPEPROPERTIES_H
