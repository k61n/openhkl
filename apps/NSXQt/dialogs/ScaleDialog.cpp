// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "ScaleDialog.h"
#include "ui_ScaleDialog.h"

#include <QImage>
#include <QTreeView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QList>
#include <QString>
#include <QDoubleSpinBox>

#include <QModelIndex>

#include <Eigen/Core>
#include <QDebug>

#include <iostream>

#include <QMessageBox>

#include "Sample.h"
#include "SpaceGroup.h"
#include "SpaceGroupSymbols.h"
#include "IData.h"
#include "Peak3D.h"
#include "Minimizer.h"
#include "RFactor.h"

#include "Externals/qcustomplot.h"

using namespace std;

using SX::Crystal::Peak3D;

ScaleDialog::ScaleDialog(const vector<vector<Peak3D*>>& peaks, QWidget *parent) :
    QDialog(parent),
    _peaks(peaks),
    _numFrames(0),
    ui(new Ui::ScaleDialog)
{
    ui->setupUi(this);

    // set all scaling factors to 1
    resetScale();

    // calculate and build
    calculateRFactors();


    // reserve enough space in _scale!!
//    for (auto& peak_list: _peaks) {
//        for (auto& peak: peak_list) {
//            int frame = std::ceil(peak->getPeak()->getAABBCenter()[2]);
//            if ( frame > _scale.size() )
//                _scale.resize(frame+1);
//        }
//    }

    // initialize the plot
    buildScalePlot();
}

ScaleDialog::~ScaleDialog()
{
    delete ui;
    // this should be handled by Qt. check with valgrind?
    // delete _peakFindModel;
}

void ScaleDialog::buildPlot()
{
    double sigma_max = ui->sigmaSpinBox->value();

    double a = ui->spinBoxA->value();
    double b = ui->spinBoxB->value();

    QCustomPlot* plot = ui->plotWidget;

    double xmin, xmax, ymin, ymax;

    xmin = xmax = ymin = ymax = 0.0;

    QVector<double> xs, ys;

    // clear all previous graphs
    while(plot->graphCount()) {
        QCPGraph* graph = plot->graph(0);
        plot->removeGraph(graph);
    }

    // go through each equivalence class of peaks
    for (auto&& peak_list: _peaks) {
        // skip if there are fewer than two peaks
        if ( peak_list.size() < 2)
            continue;

        double average = 0.0;
        double sigma, var = 0.0;

        for (auto&& p: peak_list) {
            double frame = p->getPeak()->getAABBCenter()[2];
            double in = p->getScaledIntensity() * (a+b*frame);
            average += in;
            var += in*in;
        }

        average /= peak_list.size();
        var -= peak_list.size()*average*average;
        var /= (peak_list.size()-1);
        sigma = std::sqrt(var);

        if (sigma > sigma_max*average) {
            qDebug() << "skipping set of peaks because sigma_max is too large!";
            continue;
        }

        for (auto&& p: peak_list) {


            double x = p->getPeak()->getAABBCenter()[2]; // frame
            double y = (p->getScaledIntensity() * (a+b*x) - average);

            xmin = x < xmin? x : xmin;
            xmax = x > xmax? x : xmax;
            ymin = y < ymin? y : ymin;
            ymax = y > ymax? y : ymax;

            xs.push_back(x);
            ys.push_back(y);
        }
    }

    // useful aliases
    plot->addGraph();
    QCPGraph* graph = plot->graph(0);

    // do the plotting
    graph->setData(xs, ys);
    plot->xAxis->setLabel("Frame");
    plot->yAxis->setLabel("Difference");
    plot->xAxis->setRange(xmin, xmax);
    plot->yAxis->setRange(ymin, ymax);
    plot->replot();
}

void ScaleDialog::buildScalePlot()
{
    double sigma_max = ui->sigmaSpinBox->value();

    double a = ui->spinBoxA->value();
    double b = ui->spinBoxB->value();

    QCustomPlot* plot = ui->plotWidget;

    double xmin, xmax, ymin, ymax;

    xmin = xmax = ymin = ymax = 0.0;

    QVector<double> xs, ys;

    // clear all previous graphs
    while(plot->graphCount()) {
        QCPGraph* graph = plot->graph(0);
        plot->removeGraph(graph);
    }

    // go through each equivalence class of peaks
    for (int i = 0; i < _numFrames; ++i) {

        xs.push_back(i);
        ys.push_back(getScale(i));

        if (ys[i] > ymax)
            ymax = ys[i];
    }

    xmin = 0;
    xmax = xs.size();
    ymin = 0;

    // useful aliases
    plot->addGraph();
    QCPGraph* graph = plot->graph(0);

    // do the plotting
    graph->setData(xs, ys);
    plot->xAxis->setLabel("Frame");
    plot->yAxis->setLabel("Difference");
    plot->xAxis->setRange(xmin, xmax);
    plot->yAxis->setRange(ymin, ymax);
    plot->replot();
}

void ScaleDialog::calculateRFactors()
{
    _Rmerge = 0;
    _Rmeas = 0;
    _Rpim = 0;
    _values = 0;

    double I_total = 0.0;

    _averages.resize(_peaks.size());

    // go through each equivalence class of peaks
    for (int i = 0; i < _peaks.size(); ++i) {
        vector<Peak3D*> &peak_list = _peaks[i];

        // skip if there are fewer than two peaks
        if ( peak_list.size() < 2)
            continue;

        double average = 0.0;
        double sigma, var = 0.0;

        for (auto&& p: peak_list) {
            double z = p->getPeak()->getAABBCenter()[2];
            double in = p->getScaledIntensity()*getScale(z);

            if ( z > _numFrames)
                _numFrames = std::ceil(z);

            average += in;
            ++_values;
        }

        const double n = peak_list.size();
        average /= n;

        _averages[i] = average;

        assert(average > 0.0);

        I_total += n*average;

        const double Fmeas = std::sqrt(n / (n-1));
        const double Fpim = std::sqrt(1 / (n-1));

        double I_total = 0.0;

        for (auto&& p: peak_list) {
            double z = p->getPeak()->getAABBCenter()[2];
            double diff = std::fabs(p->getScaledIntensity()*getScale(z) - average);
            _Rmerge += diff;
            _Rmeas += Fmeas*diff;
            _Rpim += Fpim*diff;
        }
    }

    _Rmerge /= I_total;
    _Rmeas /= I_total;
    _Rpim /= I_total;

    qDebug() << "R merge = " << _Rmerge;
    qDebug() << "R meas  = " << _Rmeas;
    qDebug() << "R pim   = " << _Rpim;
}

void ScaleDialog::on_redrawButton_clicked()
{
    calculateRFactors();
    buildPlot();
}

void ScaleDialog::resetScale()
{
//    for (auto& peak_list: _peaks) {
//        for (auto& peak: peak_list) {
//            peak->setScale(1.0);
//            int frame = std::ceil(peak->getPeak()->getAABBCenter()[2]);
//            if ( frame >= _scale.size())
//                _scale.resize(frame+1);
//        }
//    }

//    for (int i = 0; i < _scale.size(); ++i)
//        _scale[i] = 1.0;
    _scaleParams.resize(4);
    _scaleParams << 0.0, 0.0, 0.0, 0.0;
}

void ScaleDialog::setScale()
{
    for (auto& peak_list: _peaks) {
        for (auto& peak: peak_list) {
            double z = peak->getPeak()->getAABBCenter()[2];
            peak->setScale(getScale(z));
        }
    }
}

void ScaleDialog::refineScale()
{
    auto residual_fn = [&](const Eigen::VectorXd& params, Eigen::VectorXd& residuals)
    {
        int i = 0;
        int idx = 0;

        Eigen::VectorXd old_params = _scaleParams;
        _scaleParams = params;

        for (i = 0, idx = 0; i < _peaks.size(); ++i) {
            if ( _peaks[i].size() < 2)
                continue;

            double average = 0;

            for (Peak3D* peak: _peaks[i]) {
                double z = peak->getPeak()->getAABBCenter()[2] ;
                average += getScale(z) * peak->getScaledIntensity();
            }

            average /= _peaks[i].size();

            for (Peak3D* peak: _peaks[i]) {
                double z = peak->getPeak()->getAABBCenter()[2];
                residuals(idx++) = getScale(z) * peak->getScaledIntensity() - average;
            }
        }

        _scaleParams = old_params;

        return 0;
    };

    qDebug() << "Refining scale using minimizer...";

    SX::Utils::Minimizer minimizer;

    resetScale();

    minimizer.initialize(_scaleParams.size(), _values);

    minimizer.setxTol(1e-15);
    minimizer.setfTol(1e-15);
    minimizer.setgTol(1e-15);

    minimizer.setParams(_scaleParams);

    minimizer.set_f(residual_fn);
    minimizer.fit(100);

    _scaleParams = minimizer.params();

    qDebug() << "...done after " << minimizer.numIterations() << "iterations";
}

void ScaleDialog::on_pushButton_clicked()
{
    refineScale();
    buildScalePlot();
    calculateRFactors();
}

double ScaleDialog::getScale(double z)
{
    // simple polynomial in the frame number
    auto& p = _scaleParams;
    return (1.0 /*+ z*p[0] + z*z*p[1]*/) * std::exp(z*p[2] + z*z*p[3]);
}

