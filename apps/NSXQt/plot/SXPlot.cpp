#include <fstream>

#include <QApplication>
#include <QKeyEvent>
#include <QPainter>

#include <Eigen/Dense>

#include "Plot/SXPlot.h"

SXPlot::SXPlot(QWidget *parent) : QCustomPlot(parent)
{
   legend->setSelectableParts(QCPLegend::spItems);
   connect(this, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
   connect(this, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));
   connect(this, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
   connect(this, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

   // Enable right button click to export ASCII data
   setContextMenuPolicy(Qt::CustomContextMenu);
   connect(this,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(setmenuRequested(QPoint)));
}

void SXPlot::update(PlottableGraphicsItem *item)
{
    Q_UNUSED(item)
}

std::string SXPlot::getType() const
{
    return "";
}

void SXPlot::copyViewToClipboard()
{
    // Create the image with the exact size of the shrunk scene
    QImage image(this->rect().size(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    this->render(&painter);
    painter.end();
    QApplication::clipboard()->setImage(image,QClipboard::Clipboard);
}

void SXPlot::keyPressEvent(QKeyEvent* event)
{
    if (event->matches(QKeySequence::Copy))
        copyViewToClipboard();
}

void SXPlot::mousePress()
{
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeDrag(yAxis->orientation());
    else
        axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void SXPlot::mouseWheel()
{
    if (xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(xAxis->orientation());
    else if (yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        axisRect()->setRangeZoom(yAxis->orientation());
    else
        axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

SXPlot::~SXPlot()
{
}

void SXPlot::titleDoubleClick(QMouseEvent* event, QCPPlotTitle* title)
{
  Q_UNUSED(event)
  // Set the plot title by double clicking on it
  bool ok;
  QString newTitle = QInputDialog::getText(this, "NSXTool", "New plot title:", QLineEdit::Normal, title->text(), &ok);
  if (ok)
  {
    title->setText(newTitle);
    replot();
  }
}

void SXPlot::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "NSXTool", "New legent:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      replot();
    }
  }
}

void SXPlot::exportToAscii()
{
    int ngraphs=this->graphCount();

    if (!ngraphs)
        return;

    int npoints=graph(0)->data()->size();
    if (!npoints)
        return;

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Choose ASCII file to export"), "", tr("Data File (*.dat)"));

    std::ofstream file;
    file.open(fileName.toStdString().c_str(),std::ios::out);
    if (!file.is_open())
        QMessageBox::critical(this, tr("NSXTool"),
                              tr("Problem opening file"));

    // First column is the key, then 2n columns (value error)
    Eigen::MatrixXd result(npoints,2*ngraphs+1);

    QCPDataMap* data=graph(0)->data();
    int current=0;
    for (auto point: *data)
    {
        result(current++,0)=point.key;
    }
    for (int i=0;i<ngraphs;++i)
    {
        QCPDataMap* data=graph(i)->data();
        int current=0;
        for (auto point: *data)
        {
            result(current,2*i+1)=point.value;
            result(current++,2*i+2)=0.5*(point.valueErrorPlus+point.valueErrorMinus);
        }
    }
    file << result;
    file.close();
}

void SXPlot::setmenuRequested(QPoint pos)
{
    // Add menu to export the graphs to ASCII if graphs are present
    if (this->graphCount())
    {
        QMenu* menu = new QMenu(this);
        QAction* expportASCII=menu->addAction("Export to ASCII");
        menu->popup(mapToGlobal(pos));
        connect(expportASCII,SIGNAL(triggered()),this,SLOT(exportToAscii()));
    }
}
