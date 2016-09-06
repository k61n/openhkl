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

#include "Externals/qcustomplot.h"

using namespace std;

using SX::Crystal::Peak3D;

ScaleDialog::ScaleDialog(const vector<vector<Peak3D*>>& peaks, QWidget *parent) :
    QDialog(parent),
    _peaks(peaks),
    ui(new Ui::ScaleDialog)
{
    ui->setupUi(this);
    calculateRFactors();
    buildPlot();
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

void ScaleDialog::calculateRFactors()
{
    _Rmerge = 0;
    _Rmeas = 0;
    _Rpim = 0;

    double I_total = 0.0;

    // go through each equivalence class of peaks
    for (auto&& peak_list: _peaks) {
        // skip if there are fewer than two peaks
        if ( peak_list.size() < 2)
            continue;

        double average = 0.0;
        double sigma, var = 0.0;

        for (auto&& p: peak_list) {
            double in = p->getScaledIntensity();
            average += in;
        }

        const double n = peak_list.size();
        average /= n;

        I_total += n*average;

        const double Fmeas = std::sqrt(n / (n-1));
        const double Fpim = std::sqrt(1 / (n-1));

        double I_total = 0.0;

        for (auto&& p: peak_list) {
            double diff = std::fabs(p->getScaledIntensity() - average);
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
    buildPlot();
}
