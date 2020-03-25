#!/usr/bin/env python3

'''
autoindextest.py

Class for testing autoindexer
'''

import sys
sys.path.append("/home/zamaan/codes/nsxtool/nsxtool/build/swig")
from experiment import Parameters, Experiment
import pynsx as nsx
from pdb import set_trace

class CellData:

    def __init__(self, name, numors, quality, cell_params, filenames):
        self.name = name
        self.numors = numors
        self.quality = quality
        self.cell_params = cell_params
        self.filenames = filenames

    def __str__(self):
        numors = str(self.numors) 
        a = self.cell_params[0]
        b = self.cell_params[1]
        c = self.cell_params[2]
        alp = self.cell_params[3]
        bet = self.cell_params[4]
        gam = self.cell_params[5]
        cell_str = f'{a:>10.3f}{b:>10.3f}{c:>10.3f}{alp:>9.3f}{bet:>9.3f}{gam:>9.3f}'
        return f'{self.name:<8s}{numors:<16s}{self.quality:>8.2f}{cell_str}\n'

class AutoIndexTest:

    def __init__(self, name, detector, params):
        self.expt = Experiment(name, detector, params)
        self.name = name
        self.filenames = None

    def autoindex(self, filenames, numors):
        self.filenames = filenames
        self.numors = numors
        self.expt.load_raw_data(filenames)
        self.expt.find_peaks()
        self.expt.integrate_peaks()
        self.expt.filter_peaks()
        self.expt.autoindex()
        self.unit_cells = self.expt.unit_cells

    def set_ref_cell(self, a, b, c, alpha, beta, gamma):
        '''
        Give the tester the expected cell parameters
        '''
        self.a_ref = a
        self.b_ref = b
        self.c_ref = c
        self.alpha_ref = alpha
        self.beta_ref = beta
        self.gamma_ref = gamma
        self.distance_tol = 0.5
        self.angle_tol = 0.1

    def set_tolerance(self, distance, angle):
        self.distance_tol = distance
        self.angle_tol = angle

    def check_cell(self, a, b, c, alpha, beta, gamma):
        passed = True
        if abs(a - self.a_ref) > self.distance_tol:
            passed = False
        if abs(b - self.b_ref) > self.distance_tol:
            passed = False
        if abs(c - self.c_ref) > self.distance_tol:
            passed = False
        if abs(alpha - self.alpha_ref) > self.angle_tol:
            passed = False
        if abs(beta - self.beta_ref) > self.angle_tol:
            passed = False
        if abs(gamma - self.gamma_ref) > self.angle_tol:
            passed = False
        return passed

    def check_unit_cells(self):
        good_cells = []
        for index, cell in enumerate(self.unit_cells):
            quality, params = cell
            if self.check_cell(*params):
                good_cell = CellData(self.name, self.numors, quality, params, self.filenames)
                good_cells.append(good_cell)
        return good_cells
