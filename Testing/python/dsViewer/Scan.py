import collections

from wx.lib.pubsub import Publisher as pub

from libD19 import Scan2D

class Scan(object):
    
    def __init__(self, filename):
        
        self._scan = Scan2D()
        
        self._filename = filename
        
        self._scan.readFromFile(str(filename))
                
        self._metadata = collections.OrderedDict([(k,self._scan.getKeyAsString(k)) for k in self._scan.getAllKeys()])
                                
        self._currentFrame = None
        
        self._idx = None
        
        self._ellipses = None
        

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

    
    def set_frame(self, idx):
        
        self._idx = idx
        
        self._currentFrame = self._scan.getFrame(idx)
        
        pub.sendMessage("SET NEW FRAME", self)
        
        
    def search_blobs(self, threshold, minSize, maxSize):
        
        nBlobs = self._scan.labelling3D(threshold, minSize, maxSize)

        pub.sendMessage("BLOB SEARCH", nBlobs)
        
                
                
    def get_ellipses(self):
                
        self._ellipses = self._scan.getEllipses(self._idx)
        if self._ellipses.any():
            pub.sendMessage("GET ELLIPSES", self)
