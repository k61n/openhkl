import wx
import wx.lib.intctrl as intctrl

class Settings(wx.ScrolledWindow):
    
    def __init__(self, parent, *args, **kwargs):
        
        wx.ScrolledWindow.__init__(self, parent, *args, **kwargs)

        self._sizer = wx.BoxSizer(wx.VERTICAL)
        
        frameStaticText = wx.StaticText(self, wx.ID_ANY, label="Frame")
        self._frameNumber = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="0")
        self._frameSlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=100, minValue=0, maxValue=100)

        thresholdText = wx.StaticText(self, wx.ID_ANY, label="Threshold")
        self._threshold = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="10.0")
        minSizeText = wx.StaticText(self, wx.ID_ANY, label="Min. size")
        self._minSize = intctrl.IntCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value=1, min=1, limited=True)
        maxSizeText = wx.StaticText(self, wx.ID_ANY, label="Max. size")
        self._maxSize = intctrl.IntCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value=100, min=1, limited=True)
        self._searchBlobs = wx.Button(self, wx.ID_ANY, label="Search")

        self._showEllipse = wx.CheckBox(self, wx.ID_ANY, label="Show ellipse")

        self._frameSizer = wx.BoxSizer(wx.HORIZONTAL)
        self._frameSizer.Add(frameStaticText, 0, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        self._frameSizer.Add(self._frameNumber, 1, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        self._frameSizer.Add(self._frameSlider, 2, wx.ALL|wx.ALIGN_CENTER_VERTICAL, 5)
        
        self._parameters = wx.StaticBox(self, wx.ID_ANY, "Blob search")
        staticSizer = wx.StaticBoxSizer(self._parameters, wx.VERTICAL)
        gbSizer = wx.GridBagSizer(5,5)
        gbSizer.Add(thresholdText, (0,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._threshold, (0,1))
        gbSizer.Add(minSizeText, (1,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._minSize, (1,1))
        gbSizer.Add(maxSizeText, (2,0), flag=wx.ALIGN_CENTER_VERTICAL)
        gbSizer.Add(self._maxSize, (2,1))
        gbSizer.Add(self._searchBlobs, (0,2), span=(3,1), flag=wx.EXPAND)
        staticSizer.Add(gbSizer, 1, wx.ALL|wx.EXPAND, 5)

        self._sizer.Add(self._frameSizer, 0, wx.EXPAND|wx.ALL, 5)
        self._sizer.Add(staticSizer, 0, wx.EXPAND|wx.ALL, 5)
        self._sizer.Add(self._showEllipse, 0, wx.EXPAND|wx.ALL, 5)
         
        self.SetSizer(self._sizer)

        self._sizer.Fit(self)
        
        self.Layout()


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
                  