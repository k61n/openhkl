#!/usr/bin/env python3

'''
experiment.py

Generalised wrapper for Python/C++ (Swig) NSXTool interface.
'''

import sys
import os.path
import logging
from pdb import set_trace
sys.path.append("/home/zamaan/codes/nsxtool/current/build/swig")
sys.path.append("/G/sw/nsx/build/swig") # Joachim
import pynsx as nsx


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

    _found_peaks = "peaks"
    _filtered_peaks = "filtered"
    _predicted_peaks = "predicted"
    _merged_peaks = "merged"
    _found_collection = None
    _filtered_collection = None
    _predicted_collection = None
    _merged_collection = None
    _data_quality = None
    _data_resolution = None

    def __init__(self, name, detector, params, verbose=False):
        '''
        Set up experiment object
        '''
        self._name = name
        self._nsxfile = self._name + ".nsx"
        self._verbose = verbose
        self._detector = detector
        self._dataformat = "raw"
        self._expt = nsx.Experiment(name, detector)
        self._params = params
        self._ref_cell = params.cell
        self._data_sets = []
        self._data_reader_factory = nsx.DataReaderFactory()
        self.set_reference_cell()

    def get_nsxfile(self):
        return self._nsxfile

    def set_logger(self, logger=None, format=None):
        if logger:
            self._logger = logger
        else:
            if not format:
                format = '%(name)s - %(levelname)s - %(message)s'
            logging.basicConfig(filename='nsx.log', filemode='w',
                                level=logging.INFO, format=format)
            self._logger = logging.getLogger("expt_logger")

    def get_logger(self):
        return self._logger

    def set_reference_cell(self):
        '''
        Add the reference unit cell to the nsx::experiment object
        '''
        a = self._params.cell['a']
        b = self._params.cell['b']
        c = self._params.cell['c']
        alpha = self._params.cell['alpha']
        beta = self._params.cell['beta']
        gamma = self._params.cell['gamma']
        self._expt.setReferenceCell(a, b, c, alpha, beta, gamma)

    def get_data(self, data_name):
        return self._expt.getData(data_name)

    def set_data_format(self, dataformat):
        self._dataformat = dataformat

    def add_data_set(self, data_name, filenames):
        if self._dataformat == 'raw':
            data = self.read_raw_data(data_name, filenames)
        elif self._dataformat == 'nexus':
            data = self.read_nexus_data(data_name, filenames)
        else:
            raise RuntimeError("No valid data reader specified")

        self.log(f'dataset {data_name}: nframes = {data.nFrames()}')
        self._expt.addData(data_name, data)
        self._data_sets.append(data_name)

    def read_raw_data(self, data_name, filenames):
        '''
        Load raw datafiles (.tiff)
        '''
        data_params = nsx.RawDataReaderParameters()

        data_params.wavelength = self._params.detector['wavelength']
        data_params.delta_omega = self._params.detector['delta_omega']
        data_params.row_major = self._params.detector['row_major']
        data_params.swap_endian = self._params.detector['swap_endian']
        data_params.bpp = self._params.detector['bpp']

        reader = nsx.RawDataReader(filenames[0], self._expt.diffractometer())
        reader.setParameters(data_params)

        for filename in filenames[1:]:
            reader.addFrame(filename)
        reader.end()
        data = nsx.DataSet(reader)
        return data

    def read_nexus_data(self, data_name, filename):
        '''
        Load ILL Nexus datafiles (.nxs)
        '''
        diffractometer = nsx.Diffractometer.create(self._detector)
        extension = 'nxs'
        return self._data_reader_factory.create(extension, filename, diffractometer)

    def find_peaks(self, dataset, start_frame, end_frame):
        '''
        Find the peaks
        '''
        self.finder = self._expt.peakFinder()
        self.finder.setFramesBegin(start_frame)
        self.finder.setFramesEnd(end_frame)
        convolver = nsx.AnnularConvolver()
        self.finder.setConvolver(convolver)

        self.finder.setMinSize(self._params.finder['min_size'])
        self.finder.setMaxSize(self._params.finder['max_size'])
        self.finder.setPeakScale(self._params.finder['peak_scale'])
        self.finder.setThreshold(self._params.finder['threshold'])

        self.finder.find(dataset)

    def integrate_peaks(self):
        '''
        Integrate the peaks
        '''
        ltype = nsx.listtype_FOUND
        integrator_type = "Pixel sum integrator"
        integrator = self._expt.getIntegrator(integrator_type)
        integrator.setPeakEnd(self._params.integration['peak_area'])
        integrator.setBkgBegin(self._params.integration['background_lower'])
        integrator.setBkgEnd(self._params.integration['background_upper'])

        self._expt.integrateFoundPeaks(integrator_type)
        self._expt.acceptFoundPeaks(self._found_peaks)
        self.found_collection = self._expt.getPeakCollection(self._found_peaks)
        n_peaks = self.found_collection.numberOfPeaks()
        return n_peaks

    def filter_peaks(self, filter_params):
        '''
        Filter the peaks
        '''
        min_strength = filter_params['min_strength']
        max_strength = filter_params['max_strength']
        min_d_range = filter_params['min_d_range']
        max_d_range = filter_params['max_d_range']

        filter = self._expt.peakFilter()
        # Filter by d-range and strength
        filter.setFilterStrength(True)
        filter.setFilterDRange(True)
        filter.setDRange(min_d_range, max_d_range)
        filter.setStrength(min_strength, max_strength)

        self.found_collection = self._expt.getPeakCollection(self._found_peaks)
        filter.resetFiltering(self.found_collection)
        filter.filter(self.found_collection)
        self._expt.acceptFilter(self._filtered_peaks, self.found_collection)
        self.filtered_collection = self._expt.getPeakCollection(self._filtered_peaks)

        n_caught = self.found_collection.numberCaughtByFilter()
        return n_caught

    def accept_unit_cell(self, peak_collection):
        length_tol = self._params.autoindexer['length_tol']
        angle_tol = self._params.autoindexer['angle_tol']
        return self._expt.acceptUnitCell(peak_collection, length_tol, angle_tol)

    def autoindex_dataset(self, dataset, start_frame, end_frame, length_tol, angle_tol):
        '''
        Compute the unit cells from the peaks most a given dataset.
        Returns True if unit cell found.
        '''

        self.length_tol = length_tol
        self.angle_tol = angle_tol
        self.find_peaks([dataset], start_frame, end_frame)
        npeaks = self.integrate_peaks()
        ncaught = self.filter_peaks(self._params.filter)
        self.log(f'Autoindex: {ncaught}/{npeaks} peaks caught by filter')
        self.log(f'Autoindex: {ncaught}/{npeaks} peaks caught by filter')
        return self.autoindex_peaks(self.filtered_collection, length_tol, angle_tol)

    def autoindex_peaks(self, peak_collection, length_tol, angle_tol):
        '''
        Overload autoindex to take a peak collection instead of a dataset
        '''
        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = self._params.autoindexer['max_dim']
        autoindexer_params.nSolutions = self._params.autoindexer['n_solutions']
        autoindexer_params.nVertices = self._params.autoindexer['n_vertices']
        autoindexer_params.subdiv = self._params.autoindexer['n_subdiv']
        autoindexer_params.indexingTolerance = \
            self._params.autoindexer['indexing_tol']
        autoindexer_params.minUnitCellVolume = \
            self._params.autoindexer['min_vol']
        self.auto_indexer = self._expt.autoIndexer()
        self.auto_indexer.setParameters(autoindexer_params)
        try:
            self.auto_indexer.autoIndex(peak_collection.getPeakList())
            solutions = self.auto_indexer.solutions()
            if self._verbose:
                self.log(f'Autoindex: cells')
                self.print_unit_cells()
            return self.accept_unit_cell(peak_collection)
        except RuntimeError:
            return None


    def get_accepted_cell(self):
        return self._expt.getAcceptedCell()

    def build_shape_library(self, data):
        '''
        Build the shape library for predicting the weak peaks
        '''
        shapelib_params = nsx.ShapeLibParameters()

        shapelib_params.kabsch = self._params.shapelib['kabsch']
        shapelib_params.sigma_m = self._params.shapelib['sigma_m']
        shapelib_params.sigma_d = self._params.shapelib['sigma_d']
        shapelib_params.nx = self._params.shapelib['nx']
        shapelib_params.ny = self._params.shapelib['ny']
        shapelib_params.nz = self._params.shapelib['nz']
        shapelib_params.peak_scale = self._params.shapelib['peak_scale']
        shapelib_params.d_min = self._params.shapelib['d_min']
        shapelib_params.d_max = self._params.shapelib['d_max']
        shapelib_params.bkg_begin = self._params.shapelib['bkg_begin']
        shapelib_params.bkg_end = self._params.shapelib['bkg_end']
        self.accept_unit_cell(self.filtered_collection)
        self._expt.buildShapeLibrary(self.filtered_collection, data, shapelib_params)

    def predict_peaks(self, data, interpolation):
        '''
        Predict shapes of weak peaks
        '''
        interpolation_types = {'None' : nsx.PeakInterpolation_NoInterpolation,
                               'InverseDistance:': nsx.PeakInterpolation_InverseDistance,
                               'Intensity:': nsx.PeakInterpolation_Intensity }
        interpol = interpolation_types[interpolation]
        prediction_params = nsx.PredictionParameters()
        prediction_params.d_min = self._params.prediction['d_min']
        prediction_params.d_max = self._params.prediction['d_max']
        prediction_params.radius = self._params.prediction['radius']
        prediction_params.frames = self._params.prediction['frames']
        prediction_params.min_neighbours = self._params.prediction['neighbours']
        self._expt.predictPeaks(self._predicted_peaks, data, prediction_params, interpol)
        self._predicted_collection = self._expt.getPeakCollection(self._predicted_peaks)

    def get_peak_collection(self, name):
        return self._expt.getPeakCollection(name)

    def merge_peaks(self):
        '''
        Merge strong peaks and predicted peaks
        '''
        friedel = self._params.merging['friedel']
        self._expt.setMergedPeaks(self._filtered_collection,
                                  self._predicted_collection, friedel)

    def get_statistics(self):
        '''
        Calculate R-factors and CC1/2, CC*
        '''
        d_min = self._params.merging['d_min']
        d_max = self._params.merging['d_max']
        n_shells = self._params.merging['n_shells']
        friedel = self._params.merging['friedel']

        self._expt.computeQuality(d_min, d_max, n_shells,
                                 self._predicted_collection,
                                 self._filtered_collection,
                                 friedel)
        self._data_resolution = self._expt.getResolution()
        self._data_quality = self._expt.getQuality
        self.log(f'R_merge          = {self._data_quality.Rmerge}')
        self.log(f'Expected R_merge = {self._data_quality.expectedRmerge}')
        self.log(f'R_meas           = {self._data_quality.Rmeas}')
        self.log(f'Expected R_meas  = {self._data_quality.expectedRmeas}')
        self.log(f'R_pim            = {self._data_quality.Rpim}')
        self.log(f'Expected R_pim   = {self._data_quality.expectedRpim}')
        self.log(f'CC_half          = {self._data_quality.CChalf}')
        self.log(f'CC_*             = {self._data_quality.CCstar}')


    def print_unit_cells(self):
        self.auto_indexer.printSolutions()

    def save(self):
        '''
        Save the experiment to self._name.nsx
        '''
        self.log(f"Saving experiment to file {self._nsxfile}")
        self._expt.saveToFile(self._nsxfile)

    def load(self):
        '''
        Load the experiment from self._name.nsx
        '''
        self.log(f"Loading experiment from {self._nsxfile}")
        if not os.path.isfile(self._nsxfile):
            raise OSError("f{self._nsxfile} not found")
        self._expt.loadFromFile(self._nsxfile)
        self.found_collection = self._expt.getPeakCollection(self._found_peaks)
        self.filtered_collection = \
            self._expt.getPeakCollection(self._filtered_peaks)

    def remove_peak_collection(self, name):
        '''
        Delete a peak collection
        '''
        self._expt.removePeakCollection(name)

    def set_parameter(self, key, value):
        '''
        Set a parameter for the experiment
        '''
        self._params.set_parameter(key, value)

    def log(self, message, level=logging.INFO):
        '''
        Write a message to the log
        '''
        self._logger.log(level, message)
