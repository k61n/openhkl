import numpy

import vtk
from vtk.util import numpy_support

import wx
from wx.lib.pubsub import Publisher as pub

import matplotlib
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg, NavigationToolbar2WxAgg
from matplotlib.figure import Figure
from matplotlib.patches import Ellipse

class FrameViewer(wx.Panel):
    
    def __init__(self, parent):
        
        wx.Panel.__init__(self, parent, wx.ID_ANY)
        
        self.build_panel()
                
        
    def build_panel(self):
        
        self._ellipses = []
        
        self._sizer = wx.BoxSizer(wx.VERTICAL)
        
        self._figure = Figure(figsize=(5,4), dpi=None)
        
        self._axes = self._figure.add_axes(frameon=True, axisbg='b')
        
        self._canvas = FigureCanvasWxAgg(self, wx.ID_ANY, self._figure )

        self._toolbar = NavigationToolbar2WxAgg(self._canvas)

        self._cb = None

        self._sizer.Add(self._canvas, 1, wx.ALL|wx.EXPAND, 5) 
        self._sizer.Add(self._toolbar, 0, wx.ALL|wx.EXPAND, 5)

        self._toolbar.Realize()
        
        self.SetSizer(self._sizer)        

        self._sizer.Fit(self)
        
        self.Layout()
                                  
              
    def set_new_frame(self, scan):

        self._figure.clear()
        self._subplot = self._figure.add_subplot( 111 )
        self._ax = self._subplot.imshow(scan.currentFrame.T,cmap='Blues',interpolation='None', vmax=10)

        if self._cb is None:
            self._cb = self._figure.colorbar(self._ax)
                                        
        self._canvas.draw()
        
                
    def show_ellipses(self, ellipses):

        if ellipses.any():   
            for i in range(ellipses.shape[0]):
                e = Ellipse(xy=ellipses[i,:2], width=ellipses[i,3], height=ellipses[i,4], angle=ellipses[i,5])
                e.set_alpha(1.0)
                e.set_fill(False)
                e.set_linewidth(2)
                e.set_linestyle('dotted')
                e.set_edgecolor('r')
                self._subplot.add_artist(e)
            self._canvas.draw()
        
        
                
    def set_lut_range(self, mi, ma):
        
        self._ax.set_clim(mi,ma)
        
        self._cb.update_bruteforce(self._ax)
        
        self._canvas.draw()
        
    
    def update_blobs_search(self, nBlobs):
        
        message = "Blob search completed successfully.\n%d blobs founds." % nBlobs
        
        d = wx.MessageDialog(self, message, style=wx.OK)
        
        d.ShowModal()
