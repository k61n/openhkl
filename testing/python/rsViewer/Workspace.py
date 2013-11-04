import numpy

import vtk
from vtk.util import numpy_support
from vtk.util import numpy_support

import wx
import wx.aui as aui
from wx.lib.pubsub import Publisher as pub

from VolumeViewer import VolumeViewer
from Settings import Settings
from SliceViewer import SliceViewer

class WorkspaceData(object):
    
    def __init__(self, filename):
        
        self._data = vtk.vtkXMLImageDataReader()
        
        self._data.SetFileName(filename)
        
        self._data.Update()
        
        self._data.GetOutput().GetPointData().SetActiveScalars("Intensity")
        
        self._bounds = b = self._data.GetOutput().GetBounds()

        self._origin = numpy.array([b[0],b[2],b[4]])
        
        self._center = numpy.array([(b[0] + b[1])/2.0, (b[2] + b[3])/2.0, (b[4] + b[5])/2.0])
        
        self._extent = numpy.array([b[1]-b[0], b[3]-b[2], b[5]-b[4]])
        
        self._maxDist = numpy.sqrt(numpy.sum(self._extent**2))
        
        img = numpy_support.vtk_to_numpy(self._data.GetOutput().GetPointData().GetScalars())
        
        self._min = numpy.min(img)
        
        self._mean = numpy.mean(img)
        
        self._median = numpy.median(img)
        
        self._max = numpy.max(img)
                
        
    @property
    def data(self):
        
        return self._data
    
    @property
    def bounds(self):
        
        return self._bounds
    
    
    @property
    def center(self):
        
        return self._center
    
    @property
    def extent(self):
        
        return self._extent
    
    
    @property
    def maxDist(self):
        
        return self._maxDist
    
    
    @property
    def halfMaxDist(self):
        
        return self._maxDist/2.0


    @property
    def min(self):
        
        return self._min


    @property
    def max(self):
        
        return self._max


    @property
    def mean(self):
        
        return self._mean


    @property
    def median(self):
        
        return self._median
    
    
    @property
    def valueRange(self):
        
        return self._max - self._min


class WorkspacePanel(wx.Panel):
    
    def __init__(self, *args, **kwargs):
        
        wx.Panel.__init__(self, *args, **kwargs)
        
        self._workspace = aui.AuiManager(self)


    def add_settings(self, panel):

        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Right().CaptionVisible(True).CloseButton(False).MinSize((300,300)))
        self._workspace.Update()

        
    def add_slice_viewer(self, panel):

        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Center().CaptionVisible(True).CloseButton(False).MinSize((300,300)))
        self._workspace.Update()

        
    def add_volume_viewer(self, panel):
        
        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Right().CaptionVisible(True).CloseButton(False).MinSize((400,300)))
        self._workspace.Update()


class WorkspaceController(object):
    
    def __init__(self, parent, filename):
        
        self._data = WorkspaceData(filename)
        
        xMin, xMax, yMin, yMax, zMin, zMax = self._data.bounds
        origin = numpy.array([xMin, yMin, zMin])
        point1 = numpy.array([xMax, yMin, zMin])
        point2 = numpy.array([xMin, yMax, zMin])
        resolution = 100
        
        self._workspace = WorkspacePanel(parent)
                                
        self._volumeViewer = VolumeViewer(self._workspace)
        self._volumeViewer.set_data(self._data.data)
        self._volumeViewer.set_plane_settings(origin, point1, point2, resolution)

        self._sliceViewer = SliceViewer(self._workspace)
        self._sliceViewer.set_data(self._volumeViewer.get_plane_data())

        self._settings = Settings(self._workspace)
        self._settings.set_entries(origin, point1, point2, resolution)
        self._settings.colorFactor.SetValue(str(self._data.max))
        self._settings.opacityFactor.SetValue(str(self._data.max))
        self._settings.intensityFactor.SetValue(str(self._data.max))
        
        self._workspace.add_slice_viewer(self._sliceViewer)
        self._workspace.add_volume_viewer(self._volumeViewer)
        self._workspace.add_settings(self._settings)
        
        self._volumeViewer._iren.RemoveObservers("CharEvent")
#        self._volumeViewer._iren.AddObserver("CharEvent", self.on_keyboard_input)   
        self._volumeViewer.planeWidget.AddObserver('InteractionEvent', self.on_interact_with_plane)   
        
        self.on_update_plane_lut()
        self.on_update_volume_opacity() 
        self.on_update_volume_color() 

        # wx events
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.origin[0])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.origin[1])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.origin[2])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point1[0])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point1[1])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point1[2])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point2[0])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point2[1])
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_settings, self._settings.point2[2])
        
        self._workspace.Bind(wx.EVT_SCROLL, self.on_update_plane_settings, self._settings.resolution)

        self._workspace.Bind(wx.EVT_SCROLL, self.on_update_volume_color, self._settings.colorSlider)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_volume_color, self._settings.colorFactor)
        
        self._workspace.Bind(wx.EVT_SCROLL, self.on_update_volume_opacity, self._settings.opacitySlider)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_volume_opacity, self._settings.opacityFactor)
        
        self._workspace.Bind(wx.EVT_SCROLL, self.on_update_plane_lut, self._settings.intensitySlider)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_update_plane_lut, self._settings.intensityFactor)
        
        
    @property
    def workspace(self):
        
        return self._workspace
            
            
    def on_update_plane_settings(self, event):
                
        origin = numpy.array([float(v.GetValue()) for v in self._settings.origin])
        point1 = numpy.array([float(v.GetValue()) for v in self._settings.point1])
        point2 = numpy.array([float(v.GetValue()) for v in self._settings.point2])
        resolution = self._settings.resolution.GetValue()
        
        self._volumeViewer.set_plane_settings(origin, point1, point2, resolution)
        
        self._sliceViewer.set_data(self._volumeViewer.get_plane_data())
        
        
    def on_interact_with_plane(self, obj, event):
        
        self._volumeViewer.update_plane()
        
        origin = self._volumeViewer.planeWidget.GetOrigin()
        point1 = self._volumeViewer.planeWidget.GetPoint1()
        point2 = self._volumeViewer.planeWidget.GetPoint2()
        resolution = self._volumeViewer.planeWidget.GetResolution()
        
        self._sliceViewer.set_data(self._volumeViewer.get_plane_data())

        self._settings.set_entries(origin, point1, point2, resolution)
        
        self.on_update_plane_lut()
        
        
    def on_update_plane_lut(self, event=None):

        fraction = self._settings.intensitySlider.GetValue()/100.0
        factor = float(self._settings.intensityFactor.GetValue())
                                           
        vmax = fraction*factor
                                                
        self._volumeViewer.set_plane_lut_range(0, vmax)
        
        self._sliceViewer.set_lut_range(0, vmax)
        

    def on_update_volume_opacity(self, event=None):
        
        fraction = self._settings.opacitySlider.GetValue()/100.0
        factor = float(self._settings.opacityFactor.GetValue())
                                
        self._volumeViewer.set_volume_opacity(0, fraction*factor)


    def on_update_volume_color(self, event=None):
        
        fraction = self._settings.colorSlider.GetValue()/100.0
        factor = float(self._settings.colorFactor.GetValue())
                                
        self._volumeViewer.set_volume_color(0, fraction*factor)

                  