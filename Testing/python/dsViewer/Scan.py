import collections

import numpy

from wx.lib.pubsub import Publisher as pub

from libD19 import Scan2D

class Scan(object):
    
    def __init__(self, filename):
        
        self._scan = Scan2D()
        
        self._filename = filename
        
        self._scan.readFromFile(str(filename))
                
        self._metadata = collections.OrderedDict([(k,self._scan.getKeyAsString(k)) for k in self._scan.getAllKeys()])
                                
        self._currentFrame = None
        
        self._idx = 0
        
        self._nBlobs = -1
        
        self._ellipses = numpy.array([],dtype=numpy.float64)
        

    @property
    def ellipses(self):
        
        return self._ellipses


    @property
    def filename(self):
        
        return self._filename
    
        
    @property
    def scan(self):
        
        return self._scan
    
    
    @property
    def currentFrame(self):
        
        return self._currentFrame


    @property
    def metadata(self):
        
        return self._metadata
    
    
    @property
    def nFrames(self):
        
        return self._scan.getNFrames()

    
    @property
    def idx(self):
        
        return self._idx
    
    
    def has_ellipses(self):
        
        return self._ellipses.any()

    
    def set_frame(self, idx):
        
        self._idx = idx
        
        self._currentFrame = self._scan.getFrame(idx)
        
        pub.sendMessage("FRAME SELECTED", self)
        
        
    def search_blobs(self, threshold, minSize, maxSize, ci):
        
        self._nBlobs = self._scan.labelling3D(threshold, minSize, maxSize, ci)

        pub.sendMessage("BLOB FOUND", self._nBlobs)
        
        
    def has_blobs(self):
        
        return self._nBlobs > 0
        
                
    def define_ellipses(self):
                        
        self._ellipses = self._scan.getEllipses(self._idx)
#         if self._ellipses.any():
#             pub.sendMessage("ELLIPSES DEFINED", self)
