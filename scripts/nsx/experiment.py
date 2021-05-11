#!/usr/bin/env python3

'''
experiment.py

Generalised wrapper for Python/C++ (Swig) NSXTool interface.
'''

import sys
import logging
from pathlib import Path
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

    _indexed = False
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
    _metadata = {}

    def __init__(self, name, detector, params, verbose=False):
        '''
        Set up experiment object
        '''
        self._name = name
        self._nsxfile = self._name + ".nsx"
        self._predictedfile = self._name + "-predicted.nsx"
        self._metafile = self._name + ".meta"
        self._verbose = verbose
        self._detector = detector
        self._expt = nsx.Experiment(name, detector)
        self._params = params
        self._ref_cell = params.cell
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
            logging.basicConfig(filename='pynsx.log', filemode='w',
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

    def get_data(self, data_name=None):
        if data_name:
            return self._expt.getData(data_name)
        else:
            return self._expt.getAllData()

    def add_data_set(self, filenames, dataformat):
        '''
        Read data file(s) and add them to the experiment as DataSet objects.
        filenames is a list of pathlib Path objects.
        Pass the *absolute path* to nsx.Experiment to ensure GUI compatibility.
        '''
        paths = [str(file.absolute()) for file in filenames]
        if dataformat == 'raw':
            data = self.read_raw_data('all', paths)
        elif dataformat == 'nexus':
            data = self.read_nexus_data(paths)
        elif dataformat == 'hdf5':
            data = self.read_hdf_data(paths[0])
        else:
            raise RuntimeError("No valid data reader specified")

        self.log(f'dataset: nframes = {data.nFrames()}')
        self._expt.addData(data)

    def get_number_of_peaks(self, collection):
        return self._expt.getPeakCollection(collection).numberOfPeaks()

    def read_raw_data(self, data_name, filenames):
        '''
        Load raw datafiles (.tiff)
        '''
        data_params = self._expt.data_params

        data_params.wavelength = self._params.detector['wavelength']
        data_params.delta_omega = self._params.detector['delta_omega']
        data_params.row_major = self._params.detector['row_major']
        data_params.swap_endian = self._params.detector['swap_endian']
        data_params.bpp = self._params.detector['bpp']

        reader = nsx.RawDataReader(filenames[0], self._expt.getDiffractometer())
        reader.setParameters(data_params)

        for filename in filenames:
            reader.addFrame(filename)
        reader.end()
        data = nsx.DataSet(reader)
        return data

    def read_nexus_data(self, filename):
        '''
        Load ILL Nexus datafiles (.nxs)
        '''
        diffractometer = nsx.Diffractometer.create(self._detector)
        extension = 'nxs'
        return self._data_reader_factory.create(extension, filename, diffractometer)

    def read_hdf_data(self, filename):
        diffractometer = nsx.Diffractometer.create(self._detector)
        extension = 'hdf'
        return self._data_reader_factory.create(extension, filename, diffractometer)


    def find_peaks(self, dataset, start_frame, end_frame):
        '''
        Find the peaks
        '''
        self.log(f"Finding peaks...")
        self.finder = self._expt.peakFinder()
        self.finder.setFramesBegin(start_frame)
        self.finder.setFramesEnd(end_frame)
        convolver = nsx.AnnularConvolver()
        self.finder.setConvolver(convolver)

        self.finder.setMinSize(self._params.finder['min_size'])
        self.finder.setMaxSize(self._params.finder['max_size'])
        self.finder.setPeakEnd(self._params.finder['peak_end'])
        self.finder.setThreshold(self._params.finder['threshold'])
        self.log(f"min_size = {self._params.finder['min_size']}")
        self.log(f"max_size = {self._params.finder['max_size']}")
        self.log(f"peak_end = {self._params.finder['peak_end']}")
        self.log(f"threshold = {self._params.finder['threshold']}")

        self.finder.find(dataset)

    def integrate_peaks(self, integrator_type):
        '''
        Integrate the peaks
        '''
        self.log(f"Integrating peaks {self._found_peaks}")
        ltype = nsx.listtype_FOUND
        integrator = self._expt.getIntegrator(integrator_type)
        integrator_params = self._expt.int_params
        integrator_params.peak_end = self._params.integration['peak_area']
        integrator_params.bkg_begin = self._params.integration['background_lower']
        integrator_params.bkg_end = self._params.integration['background_upper']
        self.log(f"Integrator type: {integrator_type}")
        self.log(f"peak_area = {self._params.integration['peak_area']}")
        self.log(f"background_lower = {self._params.integration['background_lower']}")
        self.log(f"background_upper = {self._params.integration['background_upper']}")

        self._expt.integrateFoundPeaks(integrator_type)
        self._expt.acceptFoundPeaks(self._found_peaks)
        self._found_collection = self._expt.getPeakCollection(self._found_peaks)
        n_peaks = self._found_collection.numberOfPeaks()
        return n_peaks

    def filter_peaks(self, filter_params, unfiltered_collection, filtered_collection_name):
        '''
        Filter the peaks
        '''
        self.log(f'Filtering peaks in {self._found_peaks}')
        min_strength = filter_params['min_strength']
        max_strength = filter_params['max_strength']
        min_d_range = filter_params['min_d_range']
        max_d_range = filter_params['max_d_range']
        self.log(f"min_strength = {filter_params['min_strength']}")
        self.log(f"max_strength = {filter_params['max_strength']}")
        self.log(f"d_min = {filter_params['min_d_range']}")
        self.log(f"d_max = {filter_params['max_d_range']}")

        filter = self._expt.peakFilter()
        filter.resetFilterFlags()
        # Filter by d-range, strength, and allowed by space group
        filter.setFilterStrength(True)
        filter.setFilterDRange(True)
        filter.setFilterExtinct(filter_params['extinct'])
        filter.setDRange(min_d_range, max_d_range)
        filter.setStrength(min_strength, max_strength)

        filter.resetFiltering(unfiltered_collection)
        filter.filter(unfiltered_collection)
        self._expt.acceptFilter(filtered_collection_name, unfiltered_collection, nsx.listtype_FILTERED)

        n_caught = self._expt.getPeakCollection(filtered_collection_name).numberCaughtByFilter()
        return n_caught

    def accept_unit_cell(self, peak_collection):
        return self._expt.checkAndAssignUnitCell(
            peak_collection,
            self._params.autoindexer['length_tol'],
            self._params.autoindexer['angle_tol'])

    def assign_unit_cell(self, peak_collection):
        cell = self.get_accepted_cell()
        self._expt.assignUnitCell(peak_collection)

    def set_space_group(self):
        correct_space_group = self._params.cell['spacegroup'].replace('_', ' ')
        self.log(f'Correct space group: {correct_space_group}')
        sg = self._expt.getCompatibleSpaceGroups()
        self.log('List of space groups compatible with Bravais lattice:')

        found = False
        for group in sg:
            self.log(f'{group}')
            if group == correct_space_group:
                found = True
        cell = self.get_accepted_cell()
        if found:
            cell.setSpaceGroup(nsx.SpaceGroup(correct_space_group))
        else:
            cell.setSpaceGroup(nsx.SpaceGroup(correct_space_group))
            self.log(f'WARNING: {correct_space_group} not found in list of compatible space groups')

    def set_unit_cell(self, a, b, c, alpha, beta, gamma):
        self._expt.addUnitCell("accepted", a, b, c, alpha, beta, gamma)
        self.set_space_group()

    def build_shape_collection(self, data):
        '''
        Build the shape collection for predicting the weak peaks
        '''
        self.log(f"Building shape collection...")
        params = self._expt.shape_params

        params.kabsch = self._params.shapelib['kabsch']
        params.nbins_x = self._params.shapelib['nx']
        params.nbins_y = self._params.shapelib['ny']
        params.nbins_z = self._params.shapelib['nz']
        params.peak_end = self._params.shapelib['peak_end']
        params.d_min = self._params.shapelib['shapelib_d_min']
        params.d_max = self._params.shapelib['shapelib_d_max']
        params.bkg_begin = self._params.shapelib['bkg_begin']
        params.bkg_end = self._params.shapelib['bkg_end']
        self.log(f"kabsch = {self._params.shapelib['kabsch']}")
        self.log(f"nx = {self._params.shapelib['nx']}")
        self.log(f"ny = {self._params.shapelib['ny']}")
        self.log(f"nz = {self._params.shapelib['nz']}")
        self.log(f"peak_end = {self._params.shapelib['peak_end']}")
        self.log(f"d_min = {self._params.shapelib['shapelib_d_min']}")
        self.log(f"d_max = {self._params.shapelib['shapelib_d_max']}")
        self.log(f"bkg_begin = {self._params.shapelib['bkg_begin']}")
        self.log(f"bkg_end = {self._params.shapelib['bkg_end']}")
        self._expt.assignUnitCell(self._found_collection)
        self._filtered_collection = self._expt.getPeakCollection(self._filtered_peaks)
        self._expt.assignUnitCell(self._filtered_collection)
        self._expt.buildShapeCollection(self._filtered_collection, params)
        self.log(f'Number of profiles = ' + str(self._filtered_collection.shapeCollection().numberOfPeaks()))

    def predict_peaks(self, data, integrator, interpolation):
        '''
        Predict shapes of weak peaks
        '''
        self.log(f"Predicting peaks...")
        interpolation_types = {'None' : nsx.PeakInterpolation_NoInterpolation,
                               'InverseDistance:': nsx.PeakInterpolation_InverseDistance,
                               'Intensity:': nsx.PeakInterpolation_Intensity }
        interpol = interpolation_types[interpolation]
        params = self._expt.predict_params
        params.d_min = self._params.prediction['prediction_d_min']
        params.d_max = self._params.prediction['prediction_d_max']
        params.neighbour_range_pixels = self._params.prediction['radius']
        params.neighbour_range_frames = self._params.prediction['frames']
        params.bkg_begin = self._params.prediction['prediction_bkg_begin']
        params.bkg_end = self._params.prediction['prediction_bkg_end']
        params.peak_end = self._params.prediction['prediction_scale']
        params.fit_center = self._params.prediction['fit_center']
        params.fit_covariance = self._params.prediction['fit_covariance']

        self.log(f"d_min = {self._params.prediction['prediction_d_min']}")
        self.log(f"d_max = {self._params.prediction['prediction_d_max']}")
        self.log(f"radius = {self._params.prediction['radius']}")
        self.log(f"frames = {self._params.prediction['frames']}")
        params.min_neighbours = self._params.prediction['neighbours']
        self._expt.predictPeaks(self._predicted_peaks, self._filtered_collection,
                                params, interpol)
        self._predicted_collection = self._expt.getPeakCollection(self._predicted_peaks)
        self.log(f"Integrating predicted peaks...")
        self._expt.integratePredictedPeaks(integrator, self._predicted_collection,
                                           self._filtered_collection.shapeCollection(), params)

    def get_peak_collection(self, name):
        return self._expt.getPeakCollection(name)

    def merge_peaks(self):
        '''
        Merge strong peaks and predicted peaks
        '''
        self.log(f"Merging collections {self._found_peaks} and {self._predicted_peaks}...")
        friedel = self._params.merging['friedel']
        self.log(f"friedel = {friedel}")
        self._filtered_collection = self.get_peak_collection(self._filtered_peaks)
        self._predicted_collection = self.get_peak_collection(self._predicted_peaks)
        self._expt.setMergedPeaks(self._filtered_collection,
                                  self._predicted_collection, friedel)

    def get_statistics(self):
        '''
        Calculate R-factors and CC1/2, CC*
        '''

        self.log(f"Computing quality metrics....")
        d_min = self._params.merging['merging_d_min']
        d_max = self._params.merging['merging_d_max']
        n_shells = self._params.merging['n_shells']
        friedel = self._params.merging['friedel']
        self.log(f"d_min = {d_min}")
        self.log(f"d_max = {d_max}")
        self.log(f"n_shells = {n_shells}")
        self.log(f"friedel = {friedel}")

        self._expt.computeQuality(d_min, d_max, n_shells,
                                 self._predicted_collection,
                                 self._found_collection,
                                 friedel)

        data_resolution = self._expt.getResolution()
        self.log("Resolution shells")
        for i, data in enumerate(data_resolution.shells):
            self.log(f'Shell {i}')
            self.log(f'd_min            = {data.dmin}')
            self.log(f'd_max            = {data.dmax}')
            self.log(f'R_merge          = {data.Rmerge}')
            self.log(f'Expected R_merge = {data.expectedRmerge}')
            self.log(f'R_meas           = {data.Rmeas}')
            self.log(f'Expected R_meas  = {data.expectedRmeas}')
            self.log(f'R_pim            = {data.Rpim}')
            self.log(f'Expected R_pim   = {data.expectedRpim}')
            self.log(f'CC_half          = {data.CChalf}')
            self.log(f'CC_*             = {data.CChalf}')

        data_quality = self._expt.getQuality()
        self.log("Overall:")
        self.log(f'R_merge          = {data_quality.Rmerge}')
        self.log(f'Expected R_merge = {data_quality.expectedRmerge}')
        self.log(f'R_meas           = {data_quality.Rmeas}')
        self.log(f'Expected R_meas  = {data_quality.expectedRmeas}')
        self.log(f'R_pim            = {data_quality.Rpim}')
        self.log(f'Expected R_pim   = {data_quality.expectedRpim}')
        self.log(f'CC_half          = {data_quality.CChalf}')
        self.log(f'CC_*             = {data_quality.CChalf}')

    def get_accepted_cell(self):
        return self._expt.getUnitCell("accepted")


    def print_unit_cells(self):
        pynsxprint(self.auto_indexer.solutionsToString())

    def save(self, predicted=False):
        '''
        Save the experiment to nsx file
        '''
        if predicted:
            fname = self._predictedfile
        else:
            fname = self._nsxfile
        self.log(f"Saving experiment to file {fname}")
        self._expt.saveToFile(fname)

    def load(self, filename=None, predicted=False):
        '''
        Load the experiment from nsx file
        '''
        if filename:
            fname = filename
        elif predicted:
            fname = self._predictedfile
        else:
            fname = self._nsxfile
        if not Path(fname).is_file():
            raise OSError(str(f"NSX file {fname} not found"))
        self.log(f"Loading experiment from {fname}")
        self._expt.loadFromFile(fname)
        self._found_collection = self._expt.getPeakCollection(self._found_peaks)
        if self._expt.hasUnitCell("accepted"):
            self._indexed = True
            self.set_space_group()
            self._expt.assignUnitCell(self._found_collection)
        if predicted:
            self._filtered_collection = \
                self._expt.getPeakCollection(self._filtered_peaks)
            self._predicted_collection = \
                self._expt.getPeakCollection(self._predicted_peaks)
            self._expt.assignUnitCell(self._filtered_collection)
            self._expt.assignUnitCell(self._predicted_collection)

    def loadpeaks(self):
        fname = self._nsxfile
        if not Path(fname).is_file():
            raise OSError(str(f"NSX file {fname} not found"))
        self.log(f"Loading experiment from {fname}")
        self._expt.loadFromFile(fname)
        self._found_collection = self._expt.getPeakCollection(self._found_peaks)

    def remove_peak_collection(self, name):
        '''
        Delete a peak collection
        '''
        self.log("Removing peak collection {name}")
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

    def add_metadata(self, key, value):
        self._metadata[key] = value

    def write_metadata(self):
        with open(self._metafile, 'w') as outfile:
            for key in self._metadata:
                if isinstance(self._metadata[key], list):
                    for item in self._metadata[key]:
                        outfile.write(f'{key}     {item}\n')
                else:
                    outfile.write(f'{key}     {self._metadata[key]}\n')

    def read_metadata(self):
        with open(self._metafile, 'r') as infile:
            for line in infile:
                key, value = line.split()
                if key in self._metadata:
                    if isinstance(self._metadata[key], list):
                        self._metadata[key].push_back(value)
                    else:
                        self._metadata[key] = [self._metadata[key], value]
                else:
                    self._metadata[key] = value

    def get_detector(self):
        return self._expt.getDiffractometer().detector()

    def get_found_peaks(self):
        return self.get_peak_collection(self._found_peaks)

    def get_collection_names(self):
        return self._expt.getCollectionNames()

    def check_peak_collections(self):
        self._expt.checkPeakCollections()

    def refine(self, n_batches, integrator_name):
        self.log(f"Refining...")
        peak_collection = self.get_peak_collection(self._filtered_peaks)
        cell = self.get_accepted_cell()
        data = self.get_data()[0]
        peaks = self.get_peak_collection(self._predicted_peaks)
        params = self._expt.refiner_params
        params.refine_ub = True
        params.refine_ki = True
        params.refine_sample_position = True
        params.refine_sample_orientation = True
        params.refine_detector_offset = True
        params.nbatches = n_batches
        success = self._expt.refine(peak_collection, cell, data, params)
        self.log(f"Refinement succeeded")
        if success:
            self._expt.updatePredictions(peaks)
            # Reintegrate found and predicted peaks
            integrator = self._expt.getIntegrator(integrator_name)
            integrator_params = self._expt.int_params
            prediction_params = self._expt.predict_params
            self.log(f"Integrating found peaks...")
            self._found_collection = self._expt.getPeakCollection(self._found_peaks)
            self._expt.integratePeaks(integrator, self._found_collection,
                                    integrator_params, self._filtered_collection.shapeCollection())
            self.log(f"Integrating predicted peaks...")
            self._predicted_collection = self._expt.getPeakCollection(self._predicted_peaks)
            self._expt.integratePeaks(integrator, self._predicted_collection,
                                    prediction_params, self._filtered_collection.shapeCollection())
        else:
            self.log(f"Refinement failed")

    def run_auto_indexer(self, peaks, length_tol, angle_tol, frame_min, frame_max):
        if self._expt.hasUnitCell("accepted"):
            return True;
        self.log(f"Autoindexing...")

        params = self._expt.indexer_params
        params.maxdim = self._params.autoindexer['max_dim']
        params.nSolutions = self._params.autoindexer['n_solutions']
        params.nVertices = self._params.autoindexer['n_vertices']
        params.subdiv = self._params.autoindexer['n_subdiv']
        params.indexingTolerance = \
            self._params.autoindexer['indexing_tol']
        params.minUnitCellVolume = \
            self._params.autoindexer['min_vol']
        params.length_tol = self._params.autoindexer['length_tol']
        params.angle_tol = self._params.autoindexer['angle_tol']
        self.log(f"max_dim = {self._params.autoindexer['max_dim']}")
        self.log(f"n_solutions = {self._params.autoindexer['n_solutions']}")
        self.log(f"n_vertices = {self._params.autoindexer['n_vertices']}")
        self.log(f"n_subdiv = {self._params.autoindexer['n_subdiv']}")
        self.log(f"indexing_tol = {self._params.autoindexer['indexing_tol']}")
        self.log(f"min_vol = {self._params.autoindexer['min_vol']}")
        auto_indexer = self._expt.autoIndexer()
        auto_indexer.setParameters(params)
        return self._expt.runAutoIndexer(peaks, params, length_tol, angle_tol, frame_min, frame_max)
