// author: Jonathan Fisher
// j.fisher@fz-juelich.de

#include <QTreeWidget>
#include <QComboBox>
#include <QStringList>

class QSpinBox;

#include <memory>
#include <map>
#include <string>

#include "ConvolutionKernel.h"

class ConvolutionFilterTree: public QTreeWidget {
    Q_OBJECT

private:
    QComboBox* _comboBox;
    QStringList _kernels;
    SX::Imaging::ConvolutionKernel* _selectedKernel;
    bool _rebuild;
    std::map<std::string, QSpinBox*> _parameters;

    void rebuild();


public:
    ConvolutionFilterTree(QWidget* parent);
    SX::Imaging::ConvolutionKernel* getKernel();
    void retrieveParameters();

public slots:
    void onComboBoxChange();

};

