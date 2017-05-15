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

g_angstrom = 1e-10
g_cm = 1e-2 / g_angstrom # 1 centimeter in angstroms
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
                
        self.a = AI[0,:] 
        self.b = AI[1,:] 
        self.c = AI[2,:] 

    def getQ(self, h, k, l):
        return (h*self.a + k*self.b + l*self.c)

    def index(self, p):
        h = round(self.a.dot(p))
        k = round(self.b.dot(p))
        l = round(self.c.dot(p))
        return [h, k, l]


class CylindricalDetector:
    def __init__(self, radius, height, nrows, ncols):
        self.radius = radius
        self.height = height
        self.nrows = float(nrows)
        self.ncols = float(ncols)

    # tested and works
    def getDetectorCoords(self, q, wavelength):
        ki = 1.0 / wavelength * numpy.array([0, 1, 0])
        kf = q + ki
        scale = self.radius / math.sqrt(kf[0]**2 + kf[1]**2)
        pp = kf*scale        
        return self.getDetectorCoordsFromReal(pp)

    # tested and woords
    def getQ(self, px, py, wavelength):
        norm = 1.0 / wavelength
        ki = norm * numpy.array([0, 1, 0])
        p = self.getRealCoords(px, py)
        kf = norm * p / numpy.linalg.norm(p)
        return kf - ki

    # tested and works
    def getDetectorCoordsFromReal(self, p):
        py = 0.5*(1.0 - 2.0*(p[2] / self.height)) * self.nrows # good
        theta = math.atan2(p[1], p[0])
        px = (1+theta/math.pi)*self.ncols
        while (px >= self.ncols): px -= self.ncols
        while (px < 0): px += self.ncols
        return [px, py]

    # tested and works
    def getRealCoords(self, px, py):
        v = 1.0 - 2.0*(py / self.nrows) #good
        z = v*self.height / 2.0 #good
        theta = -math.pi*(1.0- px / self.ncols) # good
        x = math.cos(theta)*self.radius
        y = math.sin(theta)*self.radius
        return numpy.array([x, y, z])

    def insideEwald(self, q, wav):
        norm = 1.0 / wav
        ki = norm * numpy.array([0, 1, 0])
        kf = q + ki
        return numpy.linalg.norm(kf) <= norm

    def getKf(self, q, wave):
        norm = 1.0 / wav
        ki = norm * numpy.array([0, 1, 0])
        return q+ki

    def getFrame(self, q, wav, rotation_frames):
        # rotation_frames is a set of rotation matrices
        assert(len(rotation_frames) > 0)

        # inital state
        inside = self.insideEwald(rotation_frames[0].dot(q), wav)

        # loop through frames and find collision
        for i in range(len(rotation_frames)):
            new_inside = self.insideEwald(rotation_frames[i].dot(q), wav)

            if (inside != new_inside):
                return i-0.5
        return None
        
def test():
    a = [ 36.801, 0.73207, 5.27977 ]
    b = [ 3.3859, 49.765, -30.002 ]
    c = [ -8.17193, 33.3248, 53.713 ]
    uc = UnitCell(a, b, c)
    wav = 2.67
    nrows = 900
    ncols = 2500
    # testing units...
    scale = 1.0
    height = 45 * g_cm
    radius = 19.95 * g_cm
    
    dmin = 2.0
    dmax = 50.0
    
    hkls = [ [h, k, l, uc.getQ(h, k, l)] for h in range(-15, 15) for k in range(-15, 15) for l in range(-15,15) ]

    frames = [ rotation(0.5 * i * g_degree) for i in range(120) ]
    images = [ numpy.zeros(shape=(nrows, ncols), dtype='int32') for f in frames ]

    det = CylindricalDetector(radius, height, nrows, ncols)

    for [h, k, l, q] in hkls:
                
        t = det.getFrame(q, wav, frames)

        if (t == None):
            continue

        t0 = int(round(math.floor(t)))
        t1 = int(round(math.ceil(t)))
        
        [px, py] = det.getDetectorCoords(q, wav)

        # testing:
        p = numpy.array([px, py, t])
        RI = numpy.linalg.inv(frames[t0])
        
        [m, n, o] = uc.index(RI.dot(p))

        print("{} {} | {} {} | {} {}".format(h, m, k, n, l, o))
        
        px = int(round(px))
        py = int(round(py))

        qrot = frames[t0].dot(q)
        qobs = det.getQ(px, py, wav)

        d = 1.0 / numpy.linalg.norm(qobs)

        #print("{} {}".format(1.0 / numpy.linalg.norm(qrot), d))
        
        if (d < dmin or d > dmax):
            continue
        
        if px < 5 or py < 5:
            continue

        if px+5 > ncols or py+5 > nrows:
            continue

        if t1 >= len(frames):
            continue
            
        print("({:.1f}, {:.1f}, {:.1f})".format(px, py, t))

        for f in [t0, t1]:
            for i in range(-5, 5):
                for j in range(-5, 5):
                    #print("{} {} {} {} {}".format(f, px, py, i, j))
                    images[f][py+i, px+j] = 1

    return images
    
