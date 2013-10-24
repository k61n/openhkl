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
        
        self._subplot = self._figure.add_subplot(111)
        
        self._background = self._figure.canvas.copy_from_bbox(self._subplot.bbox)

        self._toolbar = NavigationToolbar2WxAgg(self._canvas)

        self._ax = self._subplot.imshow(numpy.zeros((256,640)),cmap='Oranges',interpolation='None', vmax=5)

        self._cb = self._figure.colorbar(self._ax,orientation='horizontal')

        self._sizer.Add(self._canvas, 1, wx.ALL|wx.EXPAND, 5) 
        self._sizer.Add(self._toolbar, 0, wx.ALL|wx.EXPAND, 5)

        self._toolbar.Realize()
        
        self._canvas.draw()
                
        self.SetSizer(self._sizer)        

        self._sizer.Fit(self)
        
        self.Layout()
                                  
              
    def set_new_frame(self, scan):
                
        self._figure.canvas.restore_region(self._background)

        self._ax.set_data(scan.currentFrame.T)
        
        self._subplot.draw_artist(self._ax)
                          
        self.build_ellipses(scan.ellipses)
                                        
        self._figure.canvas.blit(self._subplot.bbox)
        
        
    def build_ellipses(self, ellipses):

        self._subplot.artists = []
                 
        if ellipses.any(): 
            for i in range(ellipses.shape[0]):
                e = Ellipse(xy=ellipses[i,:2], width=ellipses[i,3], height=ellipses[i,4], angle=ellipses[i,5])
                e.set_alpha(1.0)
                e.set_fill(False)
                e.set_linewidth(1)
                e.set_linestyle('solid')
                e.set_edgecolor('b')
                e.set_visible(False)
                self._subplot.add_artist(e)
                        
        
    def hide_show_ellipses(self, show=False):

        if self._subplot.artists:                 
            for a in self._subplot.artists:
                a.set_visible(show)
                self._subplot.draw_artist(a)
            self._figure.canvas.blit(self._subplot.bbox)
         
                
    def set_lut_range(self, mi, ma):
                         
        self._ax.set_clim(mi,ma)
        self._cb.update_bruteforce(self._ax)
        self._canvas.draw()
        
    
    def update_blobs_search(self, nBlobs):
        
        message = "Blob search completed successfully.\n%d blobs founds." % nBlobs
        
        d = wx.MessageDialog(self, message, style=wx.OK)
        
        d.ShowModal()
