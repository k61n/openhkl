#!/usr/bin/env python3

import sys
sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")
import pynsx as nsx
from pdb import set_trace

class Parameters:

    def __init__(self, **kwargs):
        self.detector =    { 'wavelength'       : 2.67, 
                             'delta_omega'      : 0.32,
                             'row_major'        : True,
                             'swap_endian'      : True,
                             'bpp'              : 2 }

        self.finder =      { 'min_size'         : 30,
                             'max_size'         : 10000,
                             'peak_scale'       : 1.0,
                             'threshold'        : 80.0 }

        self.integration = { 'peak_area'        : 3.0,
                             'background_lower' : 3.0,
                             'background_upper' : 6.0 }

        self.filter =      { 'min_strength'     : 1.0,
                             'max_strength'     : 1.0e6,
                             'min_d_range'      : 1.5,
                             'max_d_range'      : 50 }

        self.autoindexer = { 'max_dim'          : 200.0,
                             'n_solutions'      : 10,
                             'n_vertices'       : 1000,
                             'n_subdiv'         : 30,
                             'indexing_tol'     : 0.2,
                             'min_vol'          : 100.0 }

        for key in kwargs:
            if key in self.detector:
                self.detector[key] = kwargs[key]
            if key in self.finder:
                self.finder[key] = kwargs[key]
            if key in self.integration:
                self.integration[key] = kwargs[key]
            if key in self.filter:
                self.filter[key] = kwargs[key]
            if key in self.autoindexer:
                self.autoindexer[key] = kwargs[key]

class Experiment:

    def __init__(self, name, detector, params):
        '''
        Set up experiment object
        '''
        self.expt = nsx.Experiment(name, detector)
        self.params = params

    def load_raw_data(self, filenames):
        '''
        Load raw datafile (.tiff)
        '''
        data_params = nsx.RawDataReaderParameters()

        data_params.wavelength = self.params.detector['wavelength']
        data_params.delta_omega = self.params.detector['delta_omega']
        data_params.row_major = self.params.detector['row_major']
        data_params.swap_endian = self.params.detector['swap_endian']
        data_params.bpp = self.params.detector['bpp']

        reader = nsx.RawDataReader(filenames[0], self.expt.diffractometer())
        reader.setParameters(data_params)

        for filename in filenames[1:]:
            reader.addFrame(filename)

        reader.end()
        self.data = nsx.DataSet(reader)
        self.expt.addData(self.data)

    def find_peaks(self):
        '''
        Find the peaks
        '''
        self.finder = self.expt.peakFinder()
        convolver = nsx.AnnularConvolver()
        self.finder.setConvolver(convolver)

        self.finder.setMinSize(self.params.finder['min_size'])
        self.finder.setMaxSize(self.params.finder['max_size'])
        self.finder.setPeakScale(self.params.finder['peak_scale'])
        self.finder.setThreshold(self.params.finder['threshold'])

        self.finder.find([self.data])

    def integrate_peaks(self):
        '''
        Integrate the peaks
        '''
        integrator_type = "Pixel sum integrator"
        integrator = self.expt.getIntegrator(integrator_type)
        integrator.setPeakEnd(self.params.integration['peak_area'])
        integrator.setBkgBegin(self.params.integration['background_lower'])
        integrator.setBkgEnd(self.params.integration['background_upper'])

        self.expt.integrateFoundPeaks(integrator_type)
        self.peak_collection = self.expt.peakFinder().getPeakCollection()

    def get_peak_collection(self):
        '''
        Return the peak collection object from the finder
        '''
        return self.expt.peakFinder().getPeakCollection()

    def filter_peaks(self):
        '''
        Filter the peaks
        '''
        min_strength = self.params.filter['min_strength']
        max_strength = self.params.filter['max_strength']
        min_d_range = self.params.filter['min_d_range']
        max_d_range = self.params.filter['max_d_range']

        filter = self.expt.peakFilter()
        filter.setFilterDRange(True)
        filter.setFilterStrength(True)
        filter.setDRange(min_d_range, max_d_range)
        filter.setStrength(min_strength, max_strength)

        filter.resetFiltering(self.peak_collection)
        filter.filter(self.peak_collection)

        print(str(self.peak_collection.numberOfPeaks()) + " peaks")
        print(str(self.peak_collection.numberCaughtByFilter()) + " peaks caught by filter")

    def autoindex(self):
        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = self.params.autoindexer['max_dim']
        autoindexer_params.nSolutions = self.params.autoindexer['n_solutions']
        autoindexer_params.nVertices = self.params.autoindexer['n_vertices']
        autoindexer_params.subdiv = self.params.autoindexer['n_subdiv']
        autoindexer_params.indexingTolerance = self.params.autoindexer['indexing_tol']
        autoindexer_params.minUnitCellVolume = self.params.autoindexer['min_vol']

        auto_indexer = self.expt.autoIndexer()
        auto_indexer.setParameters(autoindexer_params)
        auto_indexer.autoIndex(self.peak_collection.getPeakList())
        auto_indexer.rankSolutions()
        auto_indexer.printSolutions()
