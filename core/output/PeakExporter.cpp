//  ***********************************************************************************************
//
//  NSXTool: data reduction for neutron single-crystal diffraction
//
//! @file      gui/frames/PeakExporter.cpp
//! @brief     Class for peak export to file
//!
//! @homepage  ###HOMEPAGE###
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
//! @authors   see CITATION, MAINTAINER
//
//  ***********************************************************************************************

#include <fstream>
#include <iomanip>

#include "core/output/PeakExporter.h"
#include "core/experiment/DataSet.h"
#include "core/analyse/MergedData.h"
#include "tables/crystal/MillerIndex.h"
#include "tables/crystal/UnitCell.h"
#include "core/statistics/CC.h"
#include "core/statistics/RFactor.h"
#include "core/experiment/DataTypes.h"

namespace nsx {

void PeakExporter::saveStatistics(
    std::string filename,  nsx::ResolutionShell resolutionShell, 
    nsx::SpaceGroup spaceGroup, bool inclFriedel)
{
    std::fstream file(filename, std::ios::out);

    file << std::fixed << std::setw(10) 
        << "dmax"
        << std::fixed << std::setw(10) 
        << "dmin"
        << std::fixed << std::setw(10) 
        << "nobs"
        << std::fixed << std::setw(10) 
        << "nmerge"
        << std::fixed << std::setw(11) 
        << "Redundancy"
        << std::fixed << std::setw(10) 
        << "R meas."
        << std::fixed << std::setw(12) 
        << "R exp. meas."
        << std::fixed << std::setw(11) 
        << "R merge"
        << std::fixed << std::setw(13) 
        << "R exp. merge"
        << std::fixed << std::setw(10) 
        << "Rpim"
        << std::fixed << std::setw(11) 
        << "Rpim exp."
        << std::fixed << std::setw(10) 
        << "CC half"
        << std::fixed << std::setw(10) 
        << "CC star"
        << std::endl;

    size_t shells = resolutionShell.nShells();
    for (int i = shells - 1; i >= 0; --i) {
        const double d_lower = resolutionShell.shell(i).dmin;
        const double d_upper = resolutionShell.shell(i).dmax;

        nsx::MergedData merged_data_per_shell(spaceGroup, inclFriedel);

        for (const nsx::sptrPeak3D& peak : resolutionShell.shell(i).peaks)
            merged_data_per_shell.addPeak(peak);

        nsx::CC cc;
        cc.calculate(merged_data_per_shell);
        nsx::RFactor rFactor;
        rFactor.calculate(merged_data_per_shell);

        file << std::fixed << std::setw(10) << std::setprecision(2)
             << d_upper
             << std::fixed << std::setw(10) << std::setprecision(2)
             << d_lower
             << std::fixed << std::setw(10) 
             << merged_data_per_shell.totalSize()
             << std::fixed << std::setw(10) 
             << merged_data_per_shell.mergedPeakSet().size() 
             << std::fixed << std::setw(11) << std::setprecision(3)
             << merged_data_per_shell.redundancy()
             << std::fixed << std::setw(10) << std::setprecision(3)
             << rFactor.Rmeas()
             << std::fixed << std::setw(12) << std::setprecision(3)
             << rFactor.expectedRmeas()
             << std::fixed << std::setw(11) << std::setprecision(3)
             << rFactor.Rmerge()
             << std::fixed << std::setw(13) << std::setprecision(3)
             << rFactor.expectedRmerge()
             << std::fixed << std::setw(10) << std::setprecision(3)
             << rFactor.Rpim()
             << std::fixed << std::setw(11) << std::setprecision(3)
             << rFactor.expectedRpim()
             << std::fixed << std::setw(10) << std::setprecision(3)
             << cc.CChalf()
             << std::fixed << std::setw(10) << std::setprecision(3)
             << cc.CCstar()
             << std::endl;
    }

    file.close();
}

void PeakExporter::saveToShelX(std::string filename, nsx::PeakList* peakList)
{
    std::fstream file(filename, std::ios::out);
    for (int i = 0; i < peakList->size(); i++){
        std::shared_ptr<nsx::Peak3D> peak = peakList->at(i);
        if (peak->selected()){
            nsx::UnitCell* cell = peak->unitCell();
            if (cell){
                nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())){
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    double intensity = peak->correctedIntensity().value();
                    double sigma_intensity = peak->correctedIntensity().sigma();
                    
                    file << std::fixed << std::setw(4) 
                        << hkl(0)
                        << std::fixed << std::setw(4) 
                        << hkl(1)
                        << std::fixed << std::setw(4) 
                        << hkl(2)
                        << std::fixed << std::setw(14) << std::setprecision(4)
                        << intensity
                        << std::fixed << std::setw(14) << std::setprecision(4)
                        << sigma_intensity
                        << std::fixed << std::setw(5) 
                        << "1"
                        << std::endl;
        }}}
    }
    file.close();
}

void PeakExporter::saveToShelX(std::string filename, nsx::MergedData* mergedData)
{
    std::fstream file(filename, std::ios::out);
    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()){
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma = I.sigma();
        
        file << std::fixed << std::setw(4) 
            << hkl[0]
            << std::fixed << std::setw(4) 
            << hkl[1]
            << std::fixed << std::setw(4) 
            << hkl[2]
            << std::fixed << std::setw(14) << std::setprecision(4)
            << intensity
            << std::fixed << std::setw(14) << std::setprecision(4)
            << sigma
            << std::fixed << std::setw(5) 
            << "1"
            << std::endl;
    }
    file.close();
}

void PeakExporter::saveToFullProf(std::string filename, nsx::PeakList* peakList)
{
    std::fstream file(filename, std::ios::out);

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::shared_ptr<nsx::DataSet> data = peakList->at(0)->data();
    double wavelength = data->reader()->metadata().key<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (int i = 0; i < peakList->size(); i++){
        std::shared_ptr<nsx::Peak3D> peak = peakList->at(i);
        if (peak->selected()){
            nsx::UnitCell* cell = peak->unitCell();
            if (cell){
                nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())){
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    double intensity = peak->correctedIntensity().value();
                    double sigma_intensity = peak->correctedIntensity().sigma();
                    
                    file << std::fixed << std::setw(4) 
                        << hkl(0)
                        << std::fixed << std::setw(4) 
                        << hkl(1)
                        << std::fixed << std::setw(4) 
                        << hkl(2)
                        << std::fixed << std::setw(14) << std::setprecision(4)
                        << intensity
                        << std::fixed << std::setw(14) << std::setprecision(4)
                        << sigma_intensity
                        << std::fixed << std::setw(5) 
                        << "1"
                        << std::endl;
        }}}
    }
    file.close();
}

void PeakExporter::saveToFullProf(
    std::string filename, nsx::MergedData* mergedData, 
    nsx::PeakList* peakList)
{
    std::fstream file(filename, std::ios::out);

    file << "TITLE File written by ...\n";
    file << "(3i4,2F14.4,i5,4f8.2)\n";

    std::shared_ptr<nsx::DataSet> data = peakList->at(0)->data();
    double wavelength = data->reader()->metadata().key<double>("wavelength");
    file << std::fixed << std::setw(8) << std::setprecision(3) << wavelength;
    file << " 0 0" << std::endl;

    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()){
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma = I.sigma();
        
        file << std::fixed << std::setw(4) 
            << hkl[0]
            << std::fixed << std::setw(4) 
            << hkl[1]
            << std::fixed << std::setw(4) 
            << hkl[2]
            << std::fixed << std::setw(14) << std::setprecision(4)
            << intensity
            << std::fixed << std::setw(14) << std::setprecision(4)
            << sigma
            << std::fixed << std::setw(5) 
            << "1"

            << std::endl;
    }
    file.close();
}

void PeakExporter::saveToSCA(std::string filename, nsx::PeakList* peakList)
{
    std::fstream file(filename, std::ios::out);

    UnitCell* unitCell = peakList->at(0)->unitCell();
    UnitCellCharacter character = unitCell->character();
    std::string symbol = unitCell->spaceGroup().symbol();
    std::for_each(symbol.begin(), symbol.end(), [](char & c){c = ::tolower(c);});
    symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '), symbol.end());

    file << "    1\n\n";
    file << std::fixed << std::setw(10) << std::setprecision(3)
        << character.a
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.b
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.c
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.alpha * (180.0/3.141592653589793238463)
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.beta * (180.0/3.141592653589793238463)
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.gamma * (180.0/3.141592653589793238463)
        << " " << symbol
        << std::endl;

    for (int i = 0; i < peakList->size(); i++){
        std::shared_ptr<nsx::Peak3D> peak = peakList->at(i);
        if (peak->selected()){
            nsx::UnitCell* cell = peak->unitCell();
            if (cell){
                nsx::MillerIndex miller_index(peak->q(), *cell);
                if (miller_index.indexed(cell->indexingTolerance())){
                    const Eigen::RowVector3i& hkl = miller_index.rowVector();
                    double intensity = peak->correctedIntensity().value();
                    double sigma_intensity = peak->correctedIntensity().sigma();
                    
                    file << std::fixed << std::setw(4) 
                        << hkl(0)
                        << std::fixed << std::setw(4) 
                        << hkl(1)
                        << std::fixed << std::setw(4) 
                        << hkl(2)<< " " << std::setprecision(1);
                    
                    if (abs(intensity) > 100000 - 1){
                        file << std::fixed << std::setw(7) << std::setprecision(1)
                            << std::scientific << intensity << " ";
                    }else{
                        file << std::fixed << std::setw(7)
                            << intensity << " ";
                    }

                    if (abs(sigma_intensity) > 100000 - 1){
                        file << std::fixed << std::setw(7) << std::setprecision(1)
                            << std::scientific << sigma_intensity
                            << std::endl;
                    }else{
                        file << std::fixed << std::setw(7)
                            << sigma_intensity
                            << std::endl;
                    }

        }}}
    }
    file.close();
}

void PeakExporter::saveToSCA(
    std::string filename, nsx::MergedData* mergedData, 
    nsx::PeakList* peakList)
{
    std::fstream file(filename, std::ios::out);

    UnitCell* unitCell = peakList->at(0)->unitCell();
    UnitCellCharacter character = unitCell->character();
    std::string symbol = unitCell->spaceGroup().symbol();
    std::for_each(symbol.begin(), symbol.end(), [](char & c){c = ::tolower(c);});
    symbol.erase(std::remove(symbol.begin(), symbol.end(), ' '), symbol.end());

    file << "    1\n\n";
    file << std::fixed << std::setw(10) << std::setprecision(3)
        << character.a
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.b
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.c
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.alpha * (180.0/3.141592653589793238463)
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.beta * (180.0/3.141592653589793238463)
        << std::fixed << std::setw(10) << std::setprecision(3)
        << character.gamma * (180.0/3.141592653589793238463)
        << " " <<  symbol
        << std::endl;

    for (const nsx::MergedPeak& peak : mergedData->mergedPeakSet()){
        const auto hkl = peak.index();
        nsx::Intensity I = peak.intensity();
        const double intensity = I.value();
        const double sigma_intensity = I.sigma();
        
        file << std::fixed << std::setw(4) 
            << hkl(0)
            << std::fixed << std::setw(4) 
            << hkl(1)
            << std::fixed << std::setw(4) 
            << hkl(2)<< " " << std::setprecision(1);
        
        if (abs(intensity) > 100000 - 1){
            file << std::fixed << std::setw(7) << std::setprecision(1)
                << std::scientific << intensity << " ";
        }else{
            file << std::fixed << std::setw(7)
                << intensity << " ";
        }

        if (abs(sigma_intensity) > 100000 - 1){
            file << std::fixed << std::setw(7) << std::setprecision(1)
                << std::scientific << sigma_intensity
                << std::endl;
        }else{
            file << std::fixed << std::setw(7)
                << sigma_intensity
                << std::endl;
        }

        
    }
    file.close();
}

    
}