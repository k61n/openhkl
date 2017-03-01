###############################################################################
# 
# author: Jonathan Fisher
#         j.fisher@fz-juelich.de
#
# description:
#     generates (obviously fake) simulated biodiff data,
#  with and without slowly varying background. This is used
#  in functional tests of nsxlib.
#
###############################################################################

import numpy
import math
from matplotlib import pyplot

g_degree = math.pi / 180.0
g_deltaw = 0.5

def rotation(omega):
    rot = [
        [math.cos(omega), -math.sin(omega), 0.0],
        [math.sin(omega), math.cos(omega), 0.0],
        [0.0, 0.0, 1.0]
        ]
    return numpy.array(rot)

class UnitCell:
    def __init__(self, u, v, w):
        self.u = numpy.array(u)
        self.v = numpy.array(v)
        self.w = numpy.array(w)

        A = numpy.array([self.u, self.v, self.w])
        AI = numpy.linalg.inv(A)
                
        self.a = AI[0] 
        self.b = AI[1] 
        self.c = AI[2] 

    def getQ(self, h, k, l):
        return h*self.a + k*self.b + l*self.c


class CylindricalDetector:
    def __init__(self, radius, height, nrows, ncols):
        self.radius = radius
        self.height = height
        self.nrows = float(nrows)
        self.ncols = float(ncols)

    def getDetectorCoords(self, q, wavelength):
        ki = 2*math.pi / wavelength * numpy.array([0, 1, 0])
        kf = q + ki
        scale = self.radius / math.sqrt(kf[0]**2 + kf[1]**2)
        pp = kf*scale        
        return self.getDetectorCoordsFromReal(pp)
   
    def getQ(self, px, py, wavelength):
        norm = 2 * math.pi / wavelength
        ki = norm * numpy.array([0, 1, 0])
        p = self.getRealCoords(px, py)
        kf = norm * p / numpy.linalg.norm(p)
        return kf - ki

    # tested and works
    def getDetectorCoordsFromReal(self, p):
        py = 0.5*(1.0 - 2.0*(p[2] / self.height)) * self.nrows # good
        theta = math.atan2(p[1], p[0])
        px = (1+theta/math.pi)*self.ncols
        return [px, py]

    # tested and works
    def getRealCoords(self, px, py):
        v = 1.0 - 2.0*(py / self.nrows) #good
        z = v*self.height / 2.0 #good
        theta = -math.pi*(1.0- px / self.ncols) # good
        x = math.cos(theta)*self.radius
        y = math.sin(theta)*self.radius
        return numpy.array([x, y, z])

def test():
    a = [10.0, 0.0, 0.0]
    b = [7.0, 7.0, 0.0]
    c = [5.0, 5.0, 5.0]
    uc = UnitCell(a, b, c)
    wav = 2.67

    hkls = [ [h, k, l] for h in range(0, 3) for k in range(0, 3) for l in range(0,3) ]
    qs = [ uc.getQ(h, k, l) for [h, k, l] in hkls]

    det = CylindricalDetector(1.0, 1.0, 100, 100)

    for q in qs:
        [px, py] = det.getDetectorCoords(q, wav)
        p = det.getRealCoords(px, py)
        [new_px, new_py] = det.getDetectorCoordsFromReal(p)

        ki = 2*math.pi / wav * numpy.array([0, 1, 0])
        kf = q + ki
        pp = kf * 1.0 / math.sqrt(kf[0]**2 + kf[1]**2)

        print("p: {}".format(p))
        print("pp: {}".format(pp))
        print("dp: {}".format(numpy.linalg.norm(p-pp)))
        print("----------------------")

        print("px, py = {}, {}".format(px, py))
        print("px, py = {}, {}".format(new_px, new_py))
        print("------------------------------------------------")
        
        new_q = det.getQ(px, py, wav)
        dq = q-new_q
        print("{}".format(dq))
        print("percent difference = {}".format(numpy.linalg.norm(dq) / numpy.linalg.norm(q) * 100.0))
        
        print("q = {}".format(q))
        print("real space = {}".format(det.getDetectorCoords(q, wav)))
        print("----------------------------------------------------------------------")
    
