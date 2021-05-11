#!/usr/bin/env python3

'''
parameters.py

Container for NSXTool parameters.
'''

class Parameters:
    '''
    Class for storing input parameters for a NSXTool data reduction. The
    keywords correspond to the NSXTool GUI. A parameter can be added as
    a constructor argument in the form of a key/value pair, and will be
    inserted into the correct dictionary if possible.
    '''

    cell =        { 'a'                : 0.0,
                    'b'                : 0.0,
                    'c'                : 0.0,
                    'alpha'            : 0.0,
                    'beta'             : 0.0,
                    'gamma'            : 0.0,
                    'spacegroup'       : 'P_1' }

    detector =    { 'wavelength'       : 2.67, 
                    'delta_omega'      : 0.32,
                    'row_major'        : True,
                    'swap_endian'      : True,
                    'bpp'              : 2 }

    finder =      { 'min_size'         : 30,
                    'max_size'         : 10000,
                    'peak_end'       : 1.0,
                    'threshold'        : 80.0 }

    integration = { 'peak_area'        : 3.0,
                    'background_lower' : 3.0,
                    'background_upper' : 6.0 }

    filter =      { 'min_strength'     : 1.0,
                    'max_strength'     : 1.0e6,
                    'min_d_range'      : 1.5,
                    'max_d_range'      : 50,
                    'extinct'          : False }

    autoindexer = { 'max_dim'          : 200.0,
                    'n_solutions'      : 10,
                    'n_vertices'       : 1000,
                    'n_subdiv'         : 30,
                    'indexing_tol'     : 0.2,
                    'min_vol'          : 100.0,
                    'length_tol'       : 1.0,
                    'angle_tol'        : 1.0 }

    shapelib    = { 'peak_end'       : 3.0,
                    'bkg_begin'        : 3.0,
                    'bkg_end'          : 4.5,
                    'kabsch'           : True,
                    'shapelib_d_min'   : 1.5,
                    'shapelib_d_max'   : 50.0,
                    'nx'               : 20,
                    'ny'               : 20,
                    'nz'               : 20 }

    prediction  = { 'prediction_d_min' : 1.5,
                    'prediction_d_max' : 50.0,
                    'radius'           : 400.0,
                    'neighbours'       : 10,
                    'frames'           : 10.0,
                    'prediction_bkg_begin' : 3.0,
                    'prediction_bkg_end'   : 3.0,
                    'prediction_scale'     : 6.0,
                    'fit_center'       : True,
                    'fit_covariance'       : True }

    merging     = { 'friedel'          : True,
                    'n_shells'         : 10,
                    'merging_d_min'    : 1.0,
                    'merging_d_max'    : 50.0 }

    def __init__(self, **kwargs):
        self._dicts = [self.cell, self.detector, self.finder, 
                       self.integration, self.filter, self.autoindexer,
                       self.shapelib, self.merging]

        for key in kwargs:
            for d in self._dicts:
                if key in d:
                    d[key] = kwargs[key]

    def load(self, filename):
        '''
        Load Parameters from a file
        '''
        params = {}
        with open(filename, 'r') as infile:
            for line in infile:
                key, value = line.split()
                for d in self._dicts:
                    if key in d:
                        # ensure that the types are correct for Swig
                        if value == 'True':
                            d[key] = True
                        elif value == 'False':
                            d[key] = False
                        else:
                            t = type(d[key])
                            d[key] = t(value)

    def dump(self, filename):
        '''
        Dump parameters to a file
        '''
        with open(filename, 'w') as outfile:
            for d in self._dicts:
                for key in d:
                    outfile.write(f'{key}     {d[key]}\n')

    def set_parameter(self, key, value):
        for d in self._dicts:
            if key in d:
                d[key] = value
                break
