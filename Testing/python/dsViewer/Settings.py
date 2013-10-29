import weakref

import wx
import wx.grid as wxgrid
import wx.lib.intctrl as intctrl

class EllipsesTable(wxgrid.PyGridTableBase):
    
    def __init__(self, data, *args, **kwargs):
        
        wxgrid.PyGridTableBase.__init__(self, *args, **kwargs)
        
        self._data = data
    
    def GetNumberRows(self):        

        return 20

    def GetNumberCols(self):

        return 3

    def IsEmptyCell(self, row, col):

        return False

    def GetTypeName(self, row, col):

        return None

    def GetValue(self, row, col):

        return "a cell"

    def SetValue(self, row, col, value):
        
        pass


class EllipsesGrid(wxgrid.Grid):
        
        def SetTable( self, object, *args):
                self.tableRef = weakref.ref(object)
                return wxgrid.Grid.SetTable(self, object, *args)
        
        def GetTable( self ):
                return self.tableRef()
            

class Settings(wx.ScrolledWindow):
    
    def __init__(self, parent, *args, **kwargs):
        
        wx.ScrolledWindow.__init__(self, parent, *args, **kwargs)

        self._sizer = wx.BoxSizer(wx.VERTICAL)

        maxIntensityText = wx.StaticText(self, wx.ID_ANY, label="Max. intensity")
        self._maxIntensityValue = wx.TextCtrl(self, wx.ID_ANY, value="5.0", style=wx.TE_PROCESS_ENTER)
        
        frameStaticText = wx.StaticText(self, wx.ID_ANY, label="Frame")
        self._frameNumber = intctrl.IntCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value=0, min=0, limited=True)
        self._frameSlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=100, minValue=0, maxValue=100)

        thresholdText = wx.StaticText(self, wx.ID_ANY, label="Threshold")
        self._threshold = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="10.0")
        minSizeText = wx.StaticText(self, wx.ID_ANY, label="Min. size")
        self._minSize = intctrl.IntCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value=10, min=1, limited=True)
        maxSizeText = wx.StaticText(self, wx.ID_ANY, label="Max. size")
        self._maxSize = intctrl.IntCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value=1000, min=1, limited=True)
        ciText = wx.StaticText(self, wx.ID_ANY, label="Confidence Interval")
        self._ciValue = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="0.9")
        self._searchBlobs = wx.Button(self, wx.ID_ANY, label="Search")

        self._showEllipse = wx.CheckBox(self, wx.ID_ANY, label="Show ellipse")

#         self._ellipsesTable = EllipsesTable()
#         self._ellipsesGrid = EllipsesGrid(self, style=wxgrid.GRID_AUTOSIZE|wxgrid.GRID_DEFAULT_ROW_LABEL_WIDTH)
#         self._ellipsesGrid.SetTable(self._ellipsesTable)

        self._maxIntensitySizer = wx.BoxSizer(wx.HORIZONTAL)
        self._maxIntensitySizer.Add(maxIntensityText, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        self._maxIntensitySizer.Add(self._maxIntensityValue, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)

        self._frameSizer = wx.BoxSizer(wx.HORIZONTAL)
        self._frameSizer.Add(frameStaticText, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        self._frameSizer.Add(self._frameNumber, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        self._frameSizer.Add(self._frameSlider, 2, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        
        self._blobsStaticBox = wx.StaticBox(self, wx.ID_ANY, "Blob search")
        self._blobsStaticSizer = wx.StaticBoxSizer(self._blobsStaticBox, wx.VERTICAL)
        gbSizer = wx.GridBagSizer(5,5)
        gbSizer.AddGrowableCol(2)

        gbSizer.Add(thresholdText, (0,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._threshold, (0,1))
        gbSizer.Add(minSizeText, (1,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._minSize, (1,1))
        gbSizer.Add(maxSizeText, (2,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._maxSize, (2,1))
        gbSizer.Add(ciText, (3,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._ciValue, (3,1), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._searchBlobs, (4,0), span=(1,3), flag=wx.EXPAND)
        gbSizer.Add(self._showEllipse, (5,0), flag=wx.ALIGN_CENTER_VERTICAL)
#         gbSizer.Add(self._ellipsesGrid, (6,0), flag=wx.ALIGN_CENTER_VERTICAL)

        self._blobsStaticSizer.Add(gbSizer, 1, wx.ALL|wx.EXPAND, 5)

        self._sizer.Add(self._maxIntensitySizer, 0, wx.EXPAND|wx.ALL, 5)
        self._sizer.Add(self._frameSizer, 0, wx.EXPAND|wx.ALL, 5)
        self._sizer.Add(self._blobsStaticSizer, 0, wx.EXPAND|wx.ALL, 5)
         
        self.SetSizer(self._sizer)

        self._sizer.Fit(self)
        
        self.Layout()

        self.SetScrollbars(pixelsPerUnitX=20, pixelsPerUnitY=20, noUnitsX=500, noUnitsY=500)


    @property
    def searchBlobs(self):
        
        return self._searchBlobs


    @property
    def showEllipse(self):
        
        return self._showEllipse
    

    @property
    def threshold(self):
        
        return self._threshold


    @property
    def minSize(self):
        
        return self._minSize


    @property
    def maxIntensityValue(self):
        
        return self._maxIntensityValue


    @property
    def ciValue(self):
        
        return self._ciValue


    @property
    def minSize(self):
        
        return self._minSize


    @property
    def maxSize(self):
        
        return self._maxSize


    @property
    def frameSlider(self):
        
        return self._frameSlider


    @property
    def frameNumber(self):
        
        return self._frameNumber


    def set_frame_range(self, max):
                                                
        self._frameSlider.SetMin(0)
        self._frameSlider.SetMax(max)
        self._frameSlider.SetValue(0)
        self._frameSlider.SetTickFreq(1)
                  