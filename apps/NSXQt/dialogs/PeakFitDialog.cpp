#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QGraphicsView>

#include <set>

#include "dialogs/PeakFitDialog.h"
#include "ui_PeakFitDialog.h"

#include "Peak3D.h"
#include "IData.h"

#include "AABB.h"
#include "ColorMap.h"
#include "IMinimizer.h"
#include "MinimizerEigen.h"
#include "MinimizerGSL.h"

#include <cmath>



using SX::Crystal::Peak3D;

PeakFitDialog::PeakFitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PeakFitDialog),
    _image(nullptr),
    _peak(nullptr)
{
    ui->setupUi(this);

    _tree = dynamic_cast<ExperimentTree*>(parent);
    assert(_tree != nullptr);

    _scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(_scene);
    ui->graphicsView->scale(1, -1); // flip image vertically to agree with conventions

    connect(ui->frameScrollBar, SIGNAL(valueChanged(int)), this, SLOT(changeFrame(int)));
    connect(ui->spinBoxH, SIGNAL(valueChanged(int)), this, SLOT(changeH(int)));
    connect(ui->spinBoxK, SIGNAL(valueChanged(int)), this, SLOT(changeK(int)));
    connect(ui->spinBoxL, SIGNAL(valueChanged(int)), this, SLOT(changeL(int)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(fitPeakShape()));
    connect(ui->collisionButton, SIGNAL(clicked()), this, SLOT(checkCollisions()));

    _hkl << 1, 0, 0;

    ui->spinBoxH->setValue(_hkl[0]);
    ui->spinBoxK->setValue(_hkl[1]);
    ui->spinBoxL->setValue(_hkl[2]);

}

PeakFitDialog::~PeakFitDialog()
{
    delete ui;
}

void PeakFitDialog::changeFrame(int value)
{
    updateView();
}

void PeakFitDialog::changeH(int value)
{
    _hkl[0] = value;
    updatePeak();
    updateView();
}

void PeakFitDialog::changeK(int value)
{
    _hkl[1] = value;
    updatePeak();
    updateView();
}

void PeakFitDialog::changeL(int value)
{
    _hkl[2] = value;
    updatePeak();
    updateView();
}

void PeakFitDialog::fitPeakShape()
{
#ifdef NSXTOOL_GSL_FOUND
    SX::Utils::MinimizerGSL minimizer;
#else
    SX::Utils::MinimizerEigen minimizer;
#endif

}

void PeakFitDialog::checkCollisions()
{
    if (!_peak) {
        qDebug() << "error: no peak selected!";
        return;
    }

    qDebug() << "checking collisions with peak at hkl = (" << _hkl[0] << ", " << _hkl[1] << ", " << _hkl[2] << ")";

    std::shared_ptr<IData> numor = _peak->getData();
    std::set<Peak3D*>& peaks = numor->getPeaks();

    for (Peak3D* other_peak: peaks) {
        if ( other_peak == _peak)
            continue;

        if (_peak->getBackground()->collide(*other_peak->getPeak())) {
            Eigen::RowVector3i hkl = other_peak->getIntegerMillerIndices();
            qDebug() << "COLLISION FOUND: ("
                     << hkl[0] << ", "
                     << hkl[1] << ", "
                     << hkl[2] << ")";




            int i;
            for (i = 0; i < 1000; ++i) {
                _peak->scaleBackgroundShape(0.90);
                if ( !_peak->getBackground()->collide(*other_peak->getPeak()))
                    break;
            }

            qDebug() << "collision removed after " << i+1 << " iterations.";
            _peak->integrate();
        }
    }
}

void PeakFitDialog::updateView()
{
    if (_tree->getSelectedNumors().size() != 1) {
        qDebug() << "must have exactly 1 numor selected!";
        return;
    }

    std::shared_ptr<IData> numor = _tree->getSelectedNumors()[0];
    SX::Data::RowMatrixi frame = numor->getFrame(ui->frameScrollBar->value());

    int intensity = std::ceil(frame.sum() / (double)(frame.rows()*frame.cols())) * 5.0;

//    int ymin = frame.cols()-_ymax;
//    int ymax = frame.cols()-_ymin;

    QImage new_image = Mat2QImage(frame.data(), frame.rows(), frame.cols(), _xmin, _xmax, _ymin, _ymax, intensity);
    new_image = new_image.scaled(ui->graphicsView->width(), ui->graphicsView->height());

    if (_image)
        _image->setPixmap(QPixmap::fromImage(new_image));
    else
        _image = _scene->addPixmap(QPixmap::fromImage(new_image));
}

void PeakFitDialog::updatePeak()
{
    // todo: handle the case of more than one selected numor!!
    if (_tree->getSelectedNumors().size() != 1) {
        qDebug() << "ERROR: as currently implemented this method supports only one selected numor!!";
        return;
    }

    std::shared_ptr<IData> numor = _tree->getSelectedNumors()[0];
    SX::Data::RowMatrixi frame = numor->getFrame(ui->frameScrollBar->value());
    std::set<Peak3D*>& peaks = numor->getPeaks();

    Peak3D* the_peak = nullptr;
    bool found_peak = false;

    for(Peak3D* peak: peaks) {
        if ( peak->getIntegerMillerIndices() == _hkl) {
            found_peak = true;
            the_peak = peak;
            break;
        }
    }

    // didn't find peak, so return
    if (!found_peak) {
        qDebug() << "Peak not found in dataset!";
        return;
    }

    // update current peak
    _peak = the_peak;

    // get AABB
    const SX::Geometry::IShape<double,3>* aabb = the_peak->getBackground();

    Eigen::Vector3d lower = aabb->getLower();
    Eigen::Vector3d upper = aabb->getUpper();

    _xmin = std::floor(lower(0));
    _ymin = std::floor(lower(1));
    _zmin = std::floor(lower(2));

    _xmax = std::ceil(upper(0));
    _ymax = std::ceil(upper(1));
    _zmax = std::ceil(upper(2));

    _xmin = _xmin < 0? 0 : _xmin;
    _ymin = _ymin < 0? 0 : _ymin;
    _zmin = _zmin < 0? 0 : _zmin;

    _xmax = _xmax >= frame.cols()? frame.cols()-1 : _xmax;
    _ymax = _ymax >= frame.rows()? frame.rows()-1 : _ymax;
    _zmax = _zmax >= numor->getNFrames()? numor->getNFrames()-1 : _zmax;


    qDebug() << _xmin << "    " << _ymin << "    " << _zmin;
    qDebug() << _xmax << "    " << _ymax << "    " << _zmax;

    // testing
    const Ellipsoid<double, 3>* ellipse = dynamic_cast<const Ellipsoid<double, 3>*>(the_peak->getPeak());

    if (ellipse) {
        Eigen::Matrix<double, 3, 1> center = ellipse->getCenter();
        qDebug() << "center: " << center(0) << ", " << center(1) << ", " << center(2);
    }

    ui->frameScrollBar->setMinimum(_zmin);
    ui->frameScrollBar->setMaximum(_zmax);
    ui->frameScrollBar->setValue(std::round(aabb->getAABBCenter()[2]));
}
