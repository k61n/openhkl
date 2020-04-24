#!/usr/bin/env python3

'''
experiment.py

Generalised wrapper for Python/C++ (Swig) NSXTool interface.
'''

import sys
import scipy
from pdb import set_trace
sys.path.append("/home/zamaan/codes/nsxtool/current/build/swig")
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

    def __init__(self, name, detector, params):
        '''
        Set up experiment object
        '''
        self.name = name
        self.name_peaks = "peaks"
        self.name_filtered = "filtered"
        self.name_fit = "fit"
        self.expt = nsx.Experiment(name, detector)
        self.params = params
        self.nsxfile = self.name + ".nsx"
        self.unit_cells = None
        self.ref_cell = params.cell
        self.min_indexing_frames = 5
        self.max_indexing_frames = 12

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

        self.reader = nsx.RawDataReader(filenames[0], self.expt.diffractometer())
        self.reader.setParameters(data_params)

        for filename in filenames[1:]:
            self.reader.addFrame(filename)
        self.reader.end()

        self.data = nsx.DataSet(self.reader)
        nframes = self.data.nFrames()
        print(f'nframes = {nframes}')
        self.expt.addData(self.name, self.data)

        self.found_collection = None
        self.filtered_collection = None

    def add_raw_data_frames(self, filenames):
        data_params = nsx.RawDataReaderParameters()

        data_params.wavelength = self.params.detector['wavelength']
        data_params.delta_omega = self.params.detector['delta_omega']
        data_params.row_major = self.params.detector['row_major']
        data_params.swap_endian = self.params.detector['swap_endian']
        data_params.bpp = self.params.detector['bpp']
        for file in filenames:
            self.reader.addFrame(file)
        self.reader.setParameters(data_params)
        self.reader.end()
        self.expt.addData(self.data)
        self.data = nsx.DataSet(self.reader)
        nframes = self.data.nFrames()
        print(f'nframes = {nframes}')

    def find_peaks(self, dataset):
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

        self.finder.find(dataset)

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

        self.found_collection = self.expt.getPeakCollection(self.name_peaks)
        filter.resetFiltering(self.found_collection)
        filter.filter(self.found_collection)
        self.expt.acceptFilter(self.name_filtered, self.found_collection)
        self.filtered_collection = self.expt.getPeakCollection(self.name_filtered)

        n_caught = self.found_collection.numberCaughtByFilter()
        return n_caught

    def autoindex(self, length_tol, angle_tol):
        '''
        Compute the unit cells from the peaks most recently found/integrated/filtered
        '''

        self.find_peaks([self.data])
        npeaks = self.integrate_peaks()
        ncaught = self.filter_peaks()

        autoindexer_params = nsx.IndexerParameters()
        autoindexer_params.maxdim = self.params.autoindexer['max_dim']
        autoindexer_params.nSolutions = self.params.autoindexer['n_solutions']
        autoindexer_params.nVertices = self.params.autoindexer['n_vertices']
        autoindexer_params.subdiv = self.params.autoindexer['n_subdiv']
        autoindexer_params.indexingTolerance = \
            self.params.autoindexer['indexing_tol']
        autoindexer_params.minUnitCellVolume = \
            self.params.autoindexer['min_vol']
        self.auto_indexer = self.expt.autoIndexer()
        self.auto_indexer.setParameters(autoindexer_params)
        self.auto_indexer.autoIndex(self.filtered_collection.getPeakList())
        solutions = self.auto_indexer.solutions()
        print(f'Autoindex: {ncaught}/{npeaks} peaks caught by filter')
        print(f'Autoindex: cells')
        self.print_unit_cells()
        return self.accept_cell(length_tol, angle_tol, solutions)

    def accept_solution(self, collection, solution):
        peak_list = collection.getPeakList()
        for peak in peak_list:
            peak.setUnitCell(solution)

    def get_unit_cells(self, solutions):
        '''
        Get a list of unit cells from the C++ solutions
        '''
        unit_cells = []
        for solution in solutions:
            cell = self.solution2cell(solution)
            unit_cells.append(cell)
        return unit_cells

    def solution2cell(self, solution):
        '''
        Convert the C++ solution to a tuple containing the quality and cell
        parameters tuple
        '''
        deg = scipy.pi / 180.0
        quality = solution[1]
        a = solution[0].character().a
        b = solution[0].character().b
        c = solution[0].character().c
        alpha = solution[0].character().alpha / deg
        beta = solution[0].character().beta / deg
        gamma = solution[0].character().gamma / deg
        return (quality, (a, b, c, alpha, beta, gamma))

    def accept_cell(self, length_tol, angle_tol, solutions):
        '''
        Return solution if it matches the reference cell, otherwise return None
        '''
        accepted = None
        for solution in solutions:
            accept = True
            quality, cell = self.solution2cell(solution)
            if (cell[0] - self.ref_cell['a']) > length_tol:
                accept = False
                continue
            if (cell[1] - self.ref_cell['b']) > length_tol:
                accept = False
                continue
            if (cell[2] - self.ref_cell['c']) > length_tol:
                accept = False
                continue
            if (cell[3] - self.ref_cell['alpha']) > angle_tol:
                accept = False
                continue
            if (cell[4] - self.ref_cell['beta']) > angle_tol:
                accept = False
                continue
            if (cell[5] - self.ref_cell['gamma']) > angle_tol:
                accept = False
                continue
            if accept:
                accepted = solution
                break
        return accepted

    def build_shape_library(self):

        # Filter the weak peaks out

        kabsch = self.params.shapelib['kabsch']
        peak_scale = self.params.shapelib['peak_scale']
        sigma_m = self.params.shapelib['sigma_m']
        sigma_d = self.params.shapelib['sigma_d']
        nx = self.params.shapelib['nx']
        ny = self.params.shapelib['ny']
        nz = self.params.shapelib['nz']

        aabb = nsx.AABB()
        if kabsch:
            aabb.setLower(-peak_scale*sigma_d, -peak_scale*sigma_d,
                          -peak_scale*sigma_m)
            aabb.setUpper(peak_scale*sigma_d, peak_scale*sigma_d,
                          peak_scale*sigma_m)
        else:
            aabb.setLower(-0.5*nx, -0.5*ny, -0.5*nz)
            aabb.setUpper(0.5*nx, 0.5*ny, 0.5*nz)

        # shape_library = nsx.ShapeLibrary(not kabsch, peak_scale, bkg_begin, bkg_end))
        # shape_integrator = nsx.ShapeIntegrator(shape_library, aabb, nx, ny, nz))

        # integrator.setPeakEnd(peak_scale);
        # integrator.setBkgBegin(bkg_begin);
        # integrator.setBkgEnd(bkg_end);

        # for data in self.data:
        #     integrator.integrate(self.filtered_peaks, shape_library, data);

        # shape_library = integrator.library()
        # shape_library.updateFit(1000);

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
        self.filtered_collection = \
            self.expt.getPeakCollection(self.name_filtered)
