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
                                  
              
    def set_new_frame(self, data):

        self._figure.clear()
        self._subplot = self._figure.add_subplot( 111 )
        self._ax = self._subplot.imshow(data.currentFrame.T, vmax=5)
        if self._cb is None:
            self._cb = self._figure.colorbar(self._ax)
        
        ell = data.scan.getEllipses(data.idx)
                                
        if ell.any():            
            for i in range(ell.shape[0]):
                e = Ellipse(xy=ell[i,:2], width=ell[i,3], height=ell[i,4], angle=ell[i,5])
                e.set_alpha(1.0)
                e.set_fill(False)
                e.set_linewidth(2)
                e.set_edgecolor('r')
                self._subplot.add_artist(e)
        
        self._canvas.draw()
        
                
    def set_lut_range(self, mi, ma):
        
        self._ax.set_clim(mi,ma)
        
        self._cb.update_bruteforce(self._ax)
        
        self._canvas.draw()