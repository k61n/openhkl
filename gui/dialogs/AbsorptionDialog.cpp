//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/dialogs/AbsorptionDialog.cpp
//! @brief     Implements class AbsorptionDialog
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include "gui/dialogs/AbsorptionDialog.h"
#include "core/experiment/Experiment.h"
#include "gui/models/Project.h"
#include "gui/models/Session.h"
#include <QFileDialog>
#include <QGraphicsView>
#include <QPushButton>
#include <QSettings>
#include <QVBoxLayout>
#include <fstream>

AbsorptionDialog::AbsorptionDialog()
{
    setAttribute(Qt::WA_DeleteOnClose);
    resize(900, 650);

    QWidget* layoutWidget = new QWidget(this);
    layoutWidget->setGeometry(80, 10, 800, 630);
    QVBoxLayout* verticalLayout = new QVBoxLayout(layoutWidget);
    QGraphicsView* crystalView = new QGraphicsView(layoutWidget);
    crystalView->setMinimumSize(800, 600);
    crystalView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    crystalView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalLayout->addWidget(crystalView);

    scrollBar = new QScrollBar(layoutWidget);
    scrollBar->setOrientation(Qt::Horizontal);
    verticalLayout->addWidget(scrollBar);

    QWidget* layoutWidget2 = new QWidget(this);
    layoutWidget2->setGeometry(10, 10, 60, 340);
    QVBoxLayout* verticalLayout2 = new QVBoxLayout(layoutWidget2);

    QPushButton* openFileButton = new QPushButton("open ILL *jpr file");
    openFileButton->setIcon(QIcon(":/resources/openFileIcon.png"));
    verticalLayout2->addWidget(openFileButton);

    rulerButton = new QPushButton("Calibrate distance using ruler");
    rulerButton->setIcon(QIcon(":/resources/IconcalibrateDistance.png"));
    verticalLayout2->addWidget(rulerButton);

    pickCenterButton = new QPushButton("Calibrate pin position");
    pickCenterButton->setIcon(QIcon(":/resources/IconpickCenter.png"));
    verticalLayout2->addWidget(pickCenterButton);

    pickPointButton = new QPushButton("Add point to the hull");
    pickPointButton->setIcon(QIcon(":/resources/IconpickingPoints.png"));
    verticalLayout2->addWidget(pickPointButton);

    removePointButton = new QPushButton("remove existing points");
    removePointButton->setIcon(QIcon(":/resources/IconremovingPoints.png"));
    verticalLayout2->addWidget(removePointButton);

    triangulateButton = new QPushButton("construct convex hull from points");
    triangulateButton->setIcon(QIcon(":/resources/Icontriangulate.png"));
    verticalLayout2->addWidget(triangulateButton);

    crystalScene = new CrystalScene(
        &gSession->currentProject()->experiment()->getDiffractometer()->sample().shape());
    crystalView->setScene(crystalScene);

    connect(scrollBar, &QScrollBar::valueChanged, [=](int i) {
        crystalScene->loadImage(QString::fromStdString(_imageList[i].second));
        crystalScene->drawText("Phi: " + QString::number(_imageList[i].first));
        connect(scrollBar, &QScrollBar::valueChanged, [=](int i) {
            crystalScene->setRotationAngle(_imageList[i].first);
        });
    });

    connect(rulerButton, &QPushButton::clicked, [=]() {
        crystalScene->activateCalibrateDistance();
    });

    connect(pickCenterButton, &QPushButton::clicked, [=]() { crystalScene->activatePickCenter(); });

    connect(pickPointButton, &QPushButton::clicked, [=]() {
        crystalScene->activatePickingPoints();
    });

    connect(removePointButton, &QPushButton::clicked, [=]() {
        crystalScene->activateRemovingPoints();
    });

    connect(triangulateButton, &QPushButton::clicked, [=]() { crystalScene->triangulate(); });

    connect(openFileButton, &QPushButton::clicked, [=]() {
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFile);

        QSettings s;
        s.beginGroup("RecentDirectories");
        QString loadDirectory = s.value("absorption", QDir::homePath()).toString();

        QString fileName = QFileDialog::getOpenFileName(
            this, "Select video file", loadDirectory, "Video file (*.info)");
        if (fileName.isEmpty())
            return;

        QFileInfo info(fileName);
        s.setValue("absorption", info.absolutePath());

        readInfoFile(fileName.toStdString());
    });

    connect(crystalScene, &CrystalScene::calibrateDistanceOK, [=]() {
        pickCenterButton->setEnabled(true);
    });

    connect(crystalScene, &CrystalScene::calibrateCenterOK, [=]() {
        pickPointButton->setEnabled(true);
        removePointButton->setEnabled(true);
        triangulateButton->setEnabled(true);
    });

    crystalView->setRenderHint(QPainter::Antialiasing);

    setupInitialButtons();

    show();
}

void AbsorptionDialog::setupInitialButtons()
{
    rulerButton->setDisabled(true);
    pickCenterButton->setDisabled(true);
    pickPointButton->setDisabled(true);
    removePointButton->setDisabled(true);
    triangulateButton->setDisabled(true);
}

void AbsorptionDialog::initializeSlider(int i)
{
    scrollBar->setEnabled(true);
    scrollBar->setRange(0, i);
}

void AbsorptionDialog::readInfoFile(const std::string& filename)
{
    // Clear alll images.
    _imageList.clear();
    _imageList.shrink_to_fit();

    std::ifstream file;
    file.open(filename.c_str(), std::ios::in);

    if (file.is_open()) {
        // First read instrument name and validate with diffractometer name
        std::string _instrumentName, date;
        file >> _instrumentName >> date;
        std::string diffType = _experiment->getDiffractometer()->name();
        if (_instrumentName.compare(diffType) != 0)
            return;

        std::string line;
        // Skip one line (comment)
        getline(file, line);
        getline(file, line);
        // Read line with goniometer angles
        getline(file, line);
        // Cout number of axes, validate with goniometer definition
        const nsx::Sample& sample = _experiment->getDiffractometer()->sample();
        const nsx::Gonio& sample_gonio = sample.gonio();
        std::size_t numberAngles = std::count(line.begin(), line.end(), ':');
        if (numberAngles == sample_gonio.nAxes())
            return;

        // Remove all occurences of ':' before reading
        line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
        std::stringstream is(line);
        for (std::size_t i = 0; i < numberAngles; ++i) {
            std::string name;
            double value;
            is >> name >> value;
            const auto& axis = sample_gonio.axis(i);
            if (axis.name().compare(name) != 0)
                return;
        }

        // Get base directory where images are stored
        QFileInfo info(filename.c_str());
        QDir dir = info.absoluteDir();
        _filepath = dir.absolutePath().toStdString();
        //
        getline(file, line);
        if (line.find("camera:") != std::string::npos)
            getline(file, line);
        while (!file.eof()) {
            line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
            std::stringstream is(line);
            std::string name, jpgfile;
            double value;
            is >> name >> value >> jpgfile;
            _imageList.emplace_back(
                std::pair<double, std::string>(value, _filepath + "/" + jpgfile));
            getline(file, line);
        }
        file.close();
        initializeSlider(_imageList.size() - 1);
        // Load front image of the movie
        crystalScene->loadImage(QString::fromStdString(_imageList[0].second));
    }
    rulerButton->setEnabled(true);
}
