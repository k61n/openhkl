#pragma once

#include <string>
#include <vector>
#include <QDialog>
#include <QString>

#include <core/GeometryTypes.h>
#include <core/InstrumentTypes.h>

namespace nsx {
class RotAxis;
}

namespace Ui {
class DialogAbsorption;
}

class CrystalScene;

class DialogAbsorption : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbsorption(nsx::sptrExperiment experiment, QWidget *parent = nullptr);
    const std::string& getMovieFilename() const;
    ~DialogAbsorption();
signals:
    //! Emitted when the image is changed in the movie
    void loadImage(QString image);
    void angleText(QString text);
    void exportHull(const nsx::ConvexHull& hull);
public slots:
    void initializeSlider(int i);
private slots:
    void on_button_openFile_pressed();
    void setupInitialButtons();

private:
    Ui::DialogAbsorption *ui;
    //! Link to the experiment
    nsx::sptrExperiment _experiment;
    //! Rotation axis to collect movie
    nsx::RotAxis* _spindleAxis;
    //! Set of Roatation angle and absolute fileName for jpg image
    std::vector<std::pair<double,std::string>> _imageList;
    //!Path of the file
    std::string _filepath;
    //! read info file containing video information
    void readInfoFile(const std::string& filename);
    //! Pointer to the QGraphicsScene
    CrystalScene* _cscene;
};
