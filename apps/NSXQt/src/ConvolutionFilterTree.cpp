#include "ConvolutionFilterTree.h"
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QComboBox>
#include <QStringList>
#include <QLabel>
#include <QDebug>

ConvolutionFilterTree::ConvolutionFilterTree(QWidget* parent): QTreeWidget(parent)
{
    this->setColumnCount(2);

    QLabel* labelWidget = new QLabel(this);
    labelWidget->setText("Filter");
    QTreeWidgetItem* filterWidget = new QTreeWidgetItem(this);
    m_comboBox = new QComboBox(this);
    m_comboBox->addItems(QStringList() << "No filter" << "Annular" << "Gaussian" << "Lorentzian");
    this->addTopLevelItem(filterWidget);
    this->setItemWidget(filterWidget, 0, labelWidget);
    this->setItemWidget(filterWidget, 1, m_comboBox);

    connect(m_comboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(onComboBoxChange()));
}

void ConvolutionFilterTree::onComboBoxChange()
{
    // JMF DEBUG just checking that the signal/slots work as intended
    qDebug() << "combo box has changed: " << m_comboBox->currentIndex();
}
