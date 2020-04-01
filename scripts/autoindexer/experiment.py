#!/usr/bin/env python3

'''
experiment.py

Generalised wrapper for Python/C++ (Swig) NSXTool interface.
'''

import sys
sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")
import scipy
import pynsx as nsx
from pdb import set_trace

def pynsxprint(printvalue):
    '''
    Print function to distinguish Python from C++ output
    '''
    print("PYNSX: " + str(printvalue))

class Experiment:
    '''
    This class is object is broadly similary to the nsx::Experiment class, with
    the data input simplified.
    '''

    def __init__(self, name, detector, params):
        '''
        Set up experiment object
        '''
        self.name = name
        self.name_peaks = "peaks"
        self.name_filtered = "filtered"
        self.expt = nsx.Experiment(name, detector)
        self.params = params
        self.nsxfile = self.name + ".nsx"
        self.solutions = None
        self.unit_cells = None

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

        self.found_collection = None
        self.filtered_collection = None

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
        ltype = nsx.listtype_FOUND
        integrator_type = "Pixel sum integrator"
        integrator = self.expt.getIntegrator(integrator_type)
        integrator.setPeakEnd(self.params.integration['peak_area'])
        integrator.setBkgBegin(self.params.integration['background_lower'])
        integrator.setBkgEnd(self.params.integration['background_upper'])

        self.expt.integrateFoundPeaks(integrator_type)
        self.expt.acceptFoundPeaks(self.name_peaks)
        self.found_collection = self.expt.getPeakCollection(self.name_peaks)
        n_peaks = self.found_collection.numberOfPeaks()
        return n_peaks

    def filter_peaks(self):
        '''
        Filter the peaks
        '''
        min_strength = self.params.filter['min_strength']
        max_strength = self.params.filter['max_strength']
        min_d_range = self.params.filter['min_d_range']
        max_d_range = self.params.filter['max_d_range']

        filter = self.expt.peakFilter()
        # Filter by d-range and strength
        filter.setFilterStrength(True)
        filter.setFilterDRange(True)
        filter.setDRange(min_d_range, max_d_range)
        filter.setStrength(min_strength, max_strength)

        filter.resetFiltering(self.found_collection)
        filter.filter(self.found_collection)
        self.expt.acceptFilter(self.name_filtered, self.found_collection)
        self.filtered_collection = self.expt.getPeakCollection(self.name_filtered)

        n_caught = self.found_collection.numberCaughtByFilter()
        return n_caught

    def autoindex(self):
        '''
        Compute the unit cell
        '''
        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = self.params.autoindexer['max_dim']
        autoindexer_params.nSolutions = self.params.autoindexer['n_solutions']
        autoindexer_params.nVertices = self.params.autoindexer['n_vertices']
        autoindexer_params.subdiv = self.params.autoindexer['n_subdiv']
        autoindexer_params.indexingTolerance = self.params.autoindexer['indexing_tol']
        autoindexer_params.minUnitCellVolume = self.params.autoindexer['min_vol']

        self.auto_indexer = self.expt.autoIndexer()
        self.auto_indexer.setParameters(autoindexer_params)
        self.auto_indexer.autoIndex(self.filtered_collection.getPeakList())
        self.solutions = self.auto_indexer.solutions()
        self.get_unit_cells()

    def get_unit_cells(self):
        deg = scipy.pi / 180.0
        self.unit_cells = []
        for cell in self.solutions:
            quality = cell[1]
            a = cell[0].character().a
            b = cell[0].character().b
            c = cell[0].character().c
            alpha = cell[0].character().alpha / deg
            beta = cell[0].character().beta / deg
            gamma = cell[0].character().gamma / deg
            self.unit_cells.append((quality, (a, b, c, alpha, beta, gamma)))

    def print_unit_cells(self):
        self.auto_indexer.printSolutions()

    def save(self):
        '''
        Save the experiment to self.name.nsx
        '''
        self.expt.saveToFile(self.nsxfile)

    def load(self):
        '''
        Load the experiment from self.name.nsx
        '''
        self.expt.loadFromFile(self.nsxfile)
        self.found_collection = self.expt.getPeakCollection(self.name_peaks)
        self.filtered_collection = self.expt.getPeakCollection(self.name_filtered)
