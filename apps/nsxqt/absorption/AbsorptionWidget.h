#ifndef ABSORPTIONWIDGET_H
#define ABSORPTIONWIDGET_H

#include <QWidget>
#include <nsxlib/instrument/Experiment.h>
#include <nsxlib/instrument/RotAxis.h>
#include <vector>

namespace Ui {
class AbsorptionWidget;
}

class AbsorptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AbsorptionWidget(nsx::Instrument::Experiment* experiment,QWidget *parent = nullptr);
    ~AbsorptionWidget();

private:
    Ui::AbsorptionWidget *ui;
    //! Link to the experiment
    nsx::Instrument::Experiment* _experiment;
    //!
    nsx::Instrument::RotAxis* _spindleAxis;
    //! Map of angle and
    std::vector<std::pair<double,std::string>> _imageList;
    //!Path of the file
    std::string _filepath;
    //! read info file containing video information
    void readInfoFile(const std::string& filename);
public slots:
    void loadImage(unsigned int i);
    void initializeSlider(int i);
private slots:
    void on_pushButton_clicked();

};

#endif // ABSORPTIONWIDGET_H
