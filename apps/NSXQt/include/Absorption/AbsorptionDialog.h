#ifndef ABSORPTIONDIALOG_H
#define ABSORPTIONDIALOG_H

#include <QDialog>
#include "CrystalScene.h"

//Forwards
namespace SX
{
    namespace Instrument
    {
        class Experiment;
        class RotAxis;
    }
}
namespace Ui {
class AbsorptionDialog;
}

class AbsorptionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AbsorptionDialog(SX::Instrument::Experiment* experiment,QWidget *parent = nullptr);
    ~AbsorptionDialog();
signals:
    //! Emitted when the image is changed in the movie
    void loadImage(QString image);
    void angleText(QString text);
public slots:
    void initializeSlider(int i);
private slots:
    void on_button_openFile_pressed();
    void setupInitialButtons();
    void on_AbsorptionDialog_destroyed();

    void on_AbsorptionDialog_finished(int result);

private:
    Ui::AbsorptionDialog *ui;
    //! Link to the experiment
    SX::Instrument::Experiment* _experiment;
    //! Rotation axis to collect movie
    SX::Instrument::RotAxis* _spindleAxis;
    //! Set of Roatation angle and absolute fileName for jpg image
    std::vector<std::pair<double,std::string>> _imageList;
    //!Path of the file
    std::string _filepath;
    //! read info file containing video information
    void readInfoFile(const std::string& filename);
    //! Pointer to the QGraphicsScene
    CrystalScene* _cscene;
};

#endif // ABSORPTIONDIALOG_H
