import wx

class Settings(wx.ScrolledWindow):
    
    def __init__(self, parent, *args, **kwargs):
        
        wx.ScrolledWindow.__init__(self, parent, *args, **kwargs)

        self._sizer = wx.GridBagSizer(5,5)
        self._sizer.AddGrowableCol(1)
        self._sizer.AddGrowableCol(2)
        self._sizer.AddGrowableCol(3)
        
        st = wx.StaticText(self, wx.ID_ANY, label="Frame")
        self._frameSlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL|wx.SL_LABELS, value=100, minValue=0, maxValue=100)
        self._frameNumber = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="0")

        s = wx.BoxSizer(wx.HORIZONTAL)

        s.Add(st, 0, flag=wx.ALIGN_CENTER_VERTICAL)
        s.Add(self._frameNumber, flag=wx.ALIGN_CENTER_VERTICAL)
        
        self._sizer.Add(s, (0,0), flag=wx.ALIGN_BOTTOM)
        self._sizer.Add(self._frameSlider, (0,1), span=(1,2), flag=wx.EXPAND)
         
        self.SetSizer(self._sizer)        

        self._sizer.Fit(self)
        
        self.Layout()
        

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
                  