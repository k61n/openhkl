#ifndef NSXQT_ABSORPTIONWIDGET_H
#define NSXQT_ABSORPTIONWIDGET_H

#include <string>
#include <utility>
#include <vector>

#include <QWidget>

namespace Ui {
class AbsorptionWidget;
}

namespace nsx {
class Experiment;
class RotAxis;
}

class AbsorptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AbsorptionWidget(nsx::Experiment* experiment,QWidget *parent = nullptr);
    ~AbsorptionWidget();

private:
    Ui::AbsorptionWidget *ui;
    //! Link to the experiment
    nsx::Experiment* _experiment;
    //!
    nsx::RotAxis* _spindleAxis;
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

#endif // NSXQT_ABSORPTIONWIDGET_H
