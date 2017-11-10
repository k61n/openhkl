#ifndef NSXQT_ABSORPTIONDIALOG_H
#define NSXQT_ABSORPTIONDIALOG_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <QDialog>
#include <QString>

#include <nsxlib/GeometryTypes.h>
#include <nsxlib/InstrumentTypes.h>

namespace Ui {
class AbsorptionDialog;
}

class CrystalScene;

class AbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AbsorptionDialog(nsx::sptrExperiment experiment, QWidget *parent = nullptr);
    const std::string& getMovieFilename() const;
    ~AbsorptionDialog();
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
    Ui::AbsorptionDialog *ui;
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

#endif // NSXQT_ABSORPTIONDIALOG_H
