import pynsx as nsx
import numpy as np
import ctypes as c
import unittest
import random

gruber_tolerance = 1e-4
niggli_tolerance = 1e-4
tolerance = 1e-6

class TestGruberBravais(unittest.TestCase):
    def test_random_orthogonal_matrix(self):
        A = np.array([(0,0,0),(0,0,0),(0,0,0)])
        i=0
        for i in range(0,3):
            j = 0
            for j in range(0,3):
                A[i,j] = random.uniform(-1.0,1.0)      
                j = j+1
            i=i+1
        # todo: QR code is not implemented in python

    # todo: cannot convert cell_from_paramas function into python
    #def test_cell_from_parmas(A,B,C,D,E,F):
     #   a = np.sqrt(A)
     #   b = np.sqrt(B)
     #   c = np.sqrt(C)
        
     #   alpha = arccos(D/b/c) 



if __name__ == '__main__':
    unittest.main()

  
