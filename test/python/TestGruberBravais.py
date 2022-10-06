#!/usr/bin/env python3

##  ***********************************************************************************************
##
##  OpenHKL: data reduction for single crystal diffraction
##
##! @file      test/python/TestGruberBravais.py
##! @brief     Test ...
##!
##! @homepage  https://openhkl.org
##! @license   GNU General Public License v3 or higher (see COPYING)
##! @copyright Institut Laue-Langevin and Forschungszentrum Jülich GmbH 2016-
##! @authors   see CITATION, MAINTAINER
##
##  ***********************************************************************************************

import pyohkl as ohkl
import numpy as np
import unittest
import random

gruber_tolerance = 1e-4
niggli_tolerance = 1e-4

class TestGruberBravais(unittest.TestCase):

    def generateTestCases(self):
        A = 32.3232323232
        B = 43.23232323
        C = 35.35353535
        D = 10.10101010
        E = 5.15151515
        F = 8.8888888

        s = {}
        t = {}

        s[6]  = A / (2*D+F)
        s[7]  = A / (D+2*E)
        s[8]  = A / (D+E+F)
        s[16] = A / (2*D+F)
        s[17] = A / (D+E+F)
        s[24] = (B-A/3) / 2 / D

        t[43] = (2*D+F) / B
        s[43] = (2*(D+E+F)-t[43]*B) / A

        cases = [
        ["cF", [A, A, A, A/2, A/2, A/2]],  # condition 1
        ["hR", [A, A, A, D, D, D]],   # condition 2
        ["cP", [A, A, A, 0, 0, 0]],   # condition 3
        ["hR", [A, A, A, -D, -D, -D]],   # condition 4
        ["cI", [A, A, A, -A/3, -A/3, -A/3]],   # condition 5
        ["tI", [A, A, A, -D*s[6], -D*s[6], -F*s[6]]],   # condition 6
        ["tI", [A, A, A, -D*s[7], -E*s[7], -E*s[7]]],   # condition 7
        ["oI", [A, A, A, -D*s[8], -E*s[8], -F*s[8]]],   # condition 8
        ["hR", [A, A, C, A/2, A/2, A/2]],   # condition 9
        ["mC", [A, A, C, D, D, F]],   # condition 10
        ["tP", [A, A, C, 0, 0, 0]],   # condition 11
        ["hP", [A, A, C, 0, 0, -A/2]],   # condition 12
        ["oC", [A, A, C, 0, 0, F]],   # condition 13
        ["mC", [A, A, C, -D, -D, -F]],   # condition 14
        ["tI", [A, A, C, -A/2, -A/2, 0]],   # condition 15
        ["oF", [A, A, C, -D*s[16], -D*s[16], -F*s[16]]],   # condition 16
        ["mC", [A, A, C, -D*s[17], -E*s[17], -F*s[17]]],   # condition 17
        ["tI", [A, B, B, A/4, A/2, A/2]],   # condition 18
        ["oI", [A, B, B, D, A/2, A/2]],   # condition 19
        ["mC", [A, B, B, D, E, E]],   # condition 20
        ["tP", [A, B, B, 0, 0, 0]],   # condition 21
        ["hP", [A, B, B, -B/2, 0, 0]],   # condition 22
        ["oC", [A, B, B, D, 0, 0]],   # condition 23
        ["hR", [A, B, B, -D*s[24], -A/3, -A/3]],   # condition 24
        ["mC", [A, B, B, -D, E, E]],   # condition 25
        ["oF", [A, B, C, A/4, A/2, A/2]],   # condition 26
        ["mC", [A, B, C, D, A/2, A/2]],   # condition 27
        ["mC", [A, B, C, D, A/2, 2*D]],   # condition 28
        ["mC", [A, B, C, D, 2*D, A/2]],   # condition 29
        ["mC", [A, B, C, B/2, E, 2*E]],   # condition 30
        ["aP", [A, B, C, D, E, F]],   # condition 31
        ["oP", [A, B, C, 0, 0, 0]],   # condition 32
        ["mP", [A, B, C, 0, E, 0]],   # condition 33
        ["mP", [A, B, C, 0, 0, F]],   # condition 34
        ["mP", [A, B, C, D, 0, 0]],   # condition 35
        ["oC", [A, B, C, 0, -A/2, 0]],   # condition 36
        ["mC", [A, B, C, D, -A/2, 0]],   # condition 37
        ["oC", [A, B, C, 0, 0, -A/2]],   # condition 38
        ["mC", [A, B, C, D, 0, -A/2]],   # condition 39
        ["oC", [A, B, C, -B/2, 0, 0]],   # condition 40
        ["mC", [A, B, C, -B/2, E, 0]],   # condition 41
        ["oI", [A, B, C, -B/2, -A/2, 0]],   # condition 42
        ["mI", [A*s[43], B*t[43], C, -D, -E, -F]],   # condition 43
        ["aP", [A, B, C, -D, -E, -F]],   # condition 44
        ]
        return cases

    def getPerturbation(self):
        a = np.random.rand(6)
        a = a*2*0.1*gruber_tolerance - 0.1*gruber_tolerance
        return a

    # todo: cannot convert cell_from_paramas function into python
    def test_cell_from_parmas(self):

       condition = 1
       test_cases = self.generateTestCases()

       for case in test_cases:
           expected_bravais = case[0]
           p = case[1]

           for i in range(10):
               d = self.getPerturbation()
               A = p[0]+d[0]
               B = p[1]+d[1]
               C = p[2]+d[2]
               D = p[3]+d[3]
               E = p[4]+d[4]
               F = p[5]+d[5]

               cell = ohkl.UnitCell()

               # todo: fix Gruber::reduce (Jonathan)


if __name__ == '__main__':
    unittest.main()
