// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include "ConvolutionFilterTree.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QDebug>
#include <QSpinBox>

#include "AnnularKernel.h"
#include "BoxKernel.h"
#include "GaussianKernel.h"
#include "LorentzianKernel.h"

void ConvolutionFilterTree::rebuild()
{
    _rebuild = true;
    // save currently selected filter
    int currentIndex = _comboBox ? _comboBox->currentIndex() : 0;

    _parameters.clear();

    //this->clear();
    while (topLevelItemCount() != 0) {
        //qDeleteAll(topLevelItem(i)->takeChildren());
        delete topLevelItem(0);
    }

    // TODO: this should be replaced by a factory
    _kernels.clear();
    _kernels << "No kernel" << "Annular" << "Gaussian" << "Lorentzian";


    QLabel* labelWidget = new QLabel(this);
    labelWidget->setText("Filter");
    QTreeWidgetItem* filterWidget = new QTreeWidgetItem(this);

    _comboBox = new QComboBox(this);
    _comboBox->addItems(_kernels);

    this->addTopLevelItem(filterWidget);
    this->setItemWidget(filterWidget, 0, labelWidget);
    this->setItemWidget(filterWidget, 1, _comboBox);

    connect(_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxChange()));

    // set kernel back to previous value
    _comboBox->setCurrentIndex(currentIndex);

    _rebuild = false;
}

void ConvolutionFilterTree::retrieveParameters()
{
    // no kernel selected: can't continue
    if (!_selectedKernel) {
        qDebug() << "warning: attempting to retrieve parameters from null kernel";
        return;
    }

    // go through each item of the tree, set values as appropriate
    for (auto& x: _parameters) {
        _selectedKernel->getParameters()[x.first] = x.second->value();
    }
}

ConvolutionFilterTree::ConvolutionFilterTree(QWidget* parent): QTreeWidget(parent)
{
    _selectedKernel = nullptr;
    _comboBox = nullptr;
    _rebuild = false;
    rebuild();
}

SX::Imaging::ConvolutionKernel* ConvolutionFilterTree::getKernel()
{
    return _selectedKernel.get();
}

void ConvolutionFilterTree::onComboBoxChange()
{
    // called during tree rebuild: do nothing
    if ( _rebuild)
        return;

    // JMF DEBUG just checking that the signal/slots work as intended
    qDebug() << "combo box has changed: " << _comboBox->currentIndex();
    qDebug() << _kernels;


    // kernel changed, so we have to rebuild the tree
    rebuild();

    // get name of selected kernel_
    std::string kernel_name = _kernels[_comboBox->currentIndex()].toStdString();
    qDebug() << "the selected kernel is " << QString(kernel_name.c_str()) << " " << _comboBox->currentIndex();

    // TODO: replace with factory
    if (kernel_name == "No kernel") {
    } else if ( kernel_name == "Annular") {
        qDebug() << "annular kernel selected";
        _selectedKernel = std::unique_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::AnnularKernel());
    }
    else if ( kernel_name == "Box") {
        _selectedKernel = std::unique_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::BoxKernel());
    }
    else if ( kernel_name == "Gaussian") {
        _selectedKernel = std::unique_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::GaussianKernel());
    }
    else if ( kernel_name == "Lorentzian") {
        _selectedKernel = std::unique_ptr<SX::Imaging::ConvolutionKernel>(new SX::Imaging::LorentzianKernel());
    }
    else {
        qDebug("Error: unidentified kernel");
    }

    // now populate the tree with parameters
    for (auto& par: _selectedKernel->getParameters()) {
        //_comboBox->addItem();

        // special cases: these are determined by the data
        if  (par.first == "rows" || par.first == "cols")
            continue;

        auto parameter_item = new QTreeWidgetItem(this);
        auto parameter_name = new QLabel(this);

        auto parameter_value = new QSpinBox(this);

        parameter_name->setText(par.first.c_str());
        parameter_value->setValue(par.second);

        this->addTopLevelItem(parameter_item);
        this->setItemWidget(parameter_item, 0, parameter_name);
        this->setItemWidget(parameter_item, 1, parameter_value);

        _parameters[par.first] = parameter_value;
    }
}
