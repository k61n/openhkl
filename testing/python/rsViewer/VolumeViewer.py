import numpy

import vtk
from vtk.wx.wxVTKRenderWindowInteractor import wxVTKRenderWindowInteractor 
from vtk.util import numpy_support

import wx
from wx.lib.pubsub import Publisher as pub

class VolumeViewer(wx.Panel):
    
    def __init__(self, parent, *args, **kwargs):
        
        wx.Panel.__init__(self, parent, wx.ID_ANY)
                
        self.build_panel()
    
    
    def build_panel(self):

        self._sizer = wx.BoxSizer(wx.VERTICAL)
                
        self.build_renderer()
                
        self.build_axes()
        
                
        
    def build_renderer(self):

        self._iren = wxVTKRenderWindowInteractor(self, wx.ID_ANY, size=self.GetSize(), flag=wx.EXPAND)
        
        self._iren.SetPosition((0,0))
                        
        self._iren.GetInteractorStyle().SetCurrentStyleToTrackballCamera()
        
        self._iren.Enable(True)
        
        self._renderer = vtk.vtkRenderer()
        
        self._iren.GetRenderWindow().AddRenderer(self._renderer)
            
        self._camera=vtk.vtkCamera()

        self._renderer.SetActiveCamera(self._camera)
        
        self._camera.SetFocalPoint(0, 0, 0)
        
        self._camera.SetPosition(0, 0, 5)

        self._sizer.Add(self._iren,1, wx.EXPAND)

        self.SetSizer(self._sizer)        

        self._sizer.Fit(self)
        
        self.Layout()
    
    
    @property    
    def iren(self):
    
        return self._iren
    
    
    def build_axes(self):
            
        self._axes = vtk.vtkAxesActor()

        self._axes.SetShaftTypeToCylinder()
        
        self._axes.SetTotalLength(100,100,100)

        self._axes.SetNormalizedShaftLength(0.6, 0.6, 0.6)
        
        self._axes.GetXAxisTipProperty().SetColor( 0, 0, 1)
        self._axes.GetXAxisShaftProperty().SetColor( 0, 0, 1)
        
        self._axes.GetYAxisTipProperty().SetColor( 0, 1, 0)
        self._axes.GetYAxisShaftProperty().SetColor( 0, 1, 0)
        
        self._axes.GetZAxisTipProperty().SetColor( 1, 0, 0)
        self._axes.GetZAxisShaftProperty().SetColor(1, 0, 0)
        
        self._axes.SetCylinderResolution(4)
        
        self._axes.SetConeResolution(4)
        
        self._axes.SetNormalizedTipLength(0.4, 0.4, 0.4)
        
        self._axes.AxisLabelsOff()
                    
        self._renderer.AddActor(self._axes)

        self._iren.Render()
        
        
    def set_plane_lut_range(self, mi, ma):
        
        self._lut.SetRange(mi, ma)
                
        self._iren.Render()
        
        
    def get_plane_range(self):
        
        return self._lut.GetRange()
        
        
    def set_volume_opacity(self, mi, ma):
        
        self._opacityTransferFunction.RemoveAllPoints()
        
        self._opacityTransferFunction.AddPoint(mi, 0.0)
        
        self._opacityTransferFunction.AddPoint(ma, 1.0)

        self._iren.Render()


    def set_volume_color(self, mi, ma):
        
        self._colorTransferFunction.RemoveAllPoints()

        self._colorTransferFunction.AddHSVPoint(mi, 0.7, 1, 1)
        
        self._colorTransferFunction.AddHSVPoint(ma, 0, 1, 1)
        
        self._iren.Render()

        
    def set_data(self, data):
        
        self._lut = vtk.vtkLookupTable()
        self._lut.SetHueRange(0.7,0)
        self._lut.Build()
        
        # The image plane widget are used to probe the dataset.
        self._planeWidget = vtk.vtkPlaneWidget()
        self._planeWidget.SetInput(data.GetOutput())

        self._planeWidget.UpdatePlacement()
        self._planeWidget.SetRepresentationToOutline()
        
        self._plane = vtk.vtkPolyData()
        self._planeWidget.GetPolyData(self._plane)
        
        self._probe = vtk.vtkProbeFilter()
        self._probe.SetInput(self._plane)
        self._probe.SetSource(data.GetOutput())
        self._probe.Update()
        
        self._cutplaneMapper = vtk.vtkPolyDataMapper()
        self._cutplaneMapper.SetInputConnection(self._probe.GetOutputPort())
        self._cutplaneMapper.SetLookupTable(self._lut)
        self._cutplaneMapper.UseLookupTableScalarRangeOn()
        
        contourActor = vtk.vtkActor()
        contourActor.SetMapper(self._cutplaneMapper)
        contourActor.VisibilityOn()
        
        # An outline is shown for context.
        outline = vtk.vtkOutlineFilter()
        outline.SetInput(data.GetOutput())
        outlineMapper = vtk.vtkPolyDataMapper()
        outlineMapper.SetInput(outline.GetOutput())
        outlineActor = vtk.vtkActor()
        outlineActor.SetMapper(outlineMapper)
        
        self._opacityTransferFunction = vtk.vtkPiecewiseFunction()
                 
        self._colorTransferFunction = vtk.vtkColorTransferFunction()
        self._colorTransferFunction.SetColorSpaceToHSV()
        
        volumeProperty = vtk.vtkVolumeProperty()
        volumeProperty.SetColor(self._colorTransferFunction)
        volumeProperty.SetScalarOpacity(self._opacityTransferFunction)
        volumeProperty.ShadeOn()
        
        compositeFunction = vtk.vtkVolumeRayCastCompositeFunction()
        volumeMapper = vtk.vtkFixedPointVolumeRayCastMapper()
        volumeMapper.SetInput(data.GetOutput())

        volume = vtk.vtkVolume()
        volume.SetMapper(volumeMapper)
        volume.SetProperty(volumeProperty)       

        # Associate the widget with the interactor        
        self._planeWidget.SetInteractor(self._iren)
        self._planeWidget.On()
        self._planeWidget.UpdatePlacement()
        self._planeWidget.PlaceWidget()
        
        # Add the actors to the renderer, set the background and size
        self._renderer.AddActor(outlineActor)
        self._renderer.AddActor(contourActor)
        self._renderer.AddVolume(volume)
        
        self._renderer.SetBackground(205./255, 201./255, 201./255)
        
        cam1 = self._renderer.GetActiveCamera()
        cam1.SetFocalPoint(0,0,0)
        cam1.SetPosition(0,0,300)
        
        self._iren.Render()
        
    
    def update_plane(self):  
          
        self._planeWidget.GetPolyData(self._plane)
        
        self._iren.Render()
            
            
    def get_plane_data(self):
        
        self._probe.Update()
        
        scalars = self._probe.GetOutput().GetPointData().GetScalars()
           
        img = numpy_support.vtk_to_numpy(scalars)
        
        dim = self._planeWidget.GetResolution() + 1
        
        img = numpy.reshape(img,(dim,dim))
        
        return img
            
        
    def set_plane_settings(self, origin, point1, point2, resolution):
        
        self._planeWidget.SetOrigin(origin)        
        
        self._planeWidget.SetPoint1(point1)        
        
        self._planeWidget.SetPoint2(point2)
        
        self._planeWidget.SetResolution(resolution)
                
        self._planeWidget.UpdatePlacement()

        self._planeWidget.GetPolyData(self._plane)        
        
        self._iren.Render()
    
    
    @property
    def planeWidget(self):
        
        return self._planeWidget
    
    
    @property
    def plane(self):
        
        return self._plane
            
        
                
        