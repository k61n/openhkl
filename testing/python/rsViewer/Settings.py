import wx

class Settings(wx.ScrolledWindow):
    
    def __init__(self, parent, *args, **kwargs):
        
        wx.ScrolledWindow.__init__(self, parent, *args, **kwargs)

        self._sizer = wx.GridBagSizer(5,5)
        self._sizer.AddGrowableCol(1)
        self._sizer.AddGrowableCol(2)
        self._sizer.AddGrowableCol(3)
        
        st1 = wx.StaticText(self, wx.ID_ANY, label="Volume settings")
        st1.SetForegroundColour(wx.BLUE)

        self._colorSlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=100, minValue=0, maxValue=100)
        self._colorFactor = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="1.00")

        self._opacitySlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=100, minValue=0, maxValue=100)
        self._opacityFactor = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="1.00")

        st2 = wx.StaticText(self, wx.ID_ANY, label="Cutplane settings")
        st2.SetForegroundColour(wx.BLUE)

        self._origin = []
        self._origin.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._origin.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._origin.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))

        self._point1 = []
        self._point1.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._point1.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._point1.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
                
        self._point2 = []
        self._point2.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._point2.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))
        self._point2.append(wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER))

        self._resolution = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=100, minValue=0, maxValue=500)

        self._intensitySlider = wx.Slider(self, wx.ID_ANY, style=wx.SL_HORIZONTAL, value=0, minValue=0, maxValue=100)
        self._intensityFactor = wx.TextCtrl(self, wx.ID_ANY, style=wx.TE_PROCESS_ENTER, value="1.00")

        self._sizer.Add(st1,  (0,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Color"), (1,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Opacity"), (2,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(st2,  (3,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Origin"), (4,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Endpoint 1"), (5,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Endpoint 2"), (6,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Resolution"), (7,0), flag=wx.ALIGN_CENTER_VERTICAL)
        self._sizer.Add(wx.StaticText(self, wx.ID_ANY, label="Intensity range"), (8,0), flag=wx.ALIGN_CENTER_VERTICAL)

        self._sizer.Add(self._colorFactor, (1,1), flag=wx.EXPAND)
        self._sizer.Add(self._colorSlider, (1,2), span=(1,2), flag=wx.EXPAND)
        self._sizer.Add(self._opacityFactor, (2,1), flag=wx.EXPAND)
        self._sizer.Add(self._opacitySlider, (2,2), span=(1,2), flag=wx.EXPAND)
        self._sizer.Add(self._origin[0], (4,1), flag=wx.EXPAND)
        self._sizer.Add(self._origin[1], (4,2), flag=wx.EXPAND)
        self._sizer.Add(self._origin[2], (4,3), flag=wx.EXPAND)
        self._sizer.Add(self._point1[0], (5,1), flag=wx.EXPAND)
        self._sizer.Add(self._point1[1], (5,2), flag=wx.EXPAND)
        self._sizer.Add(self._point1[2], (5,3), flag=wx.EXPAND)
        self._sizer.Add(self._point2[0], (6,1), flag=wx.EXPAND)
        self._sizer.Add(self._point2[1], (6,2), flag=wx.EXPAND)
        self._sizer.Add(self._point2[2], (6,3), flag=wx.EXPAND)
        self._sizer.Add(self._resolution, (7,1), span=(1,3), flag=wx.EXPAND)
        self._sizer.Add(self._intensityFactor, (8,1), flag=wx.EXPAND)
        self._sizer.Add(self._intensitySlider, (8,2), span=(1,2), flag=wx.EXPAND)
         
        self.SetSizer(self._sizer)        

        self._sizer.Fit(self)
        
        self.Layout()
        
    @property
    def origin(self):
        
        return self._origin


    @property
    def point1(self):
        
        return self._point1
  

    @property
    def point2(self):
        
        return self._point2


    @property
    def resolution(self):
        
        return self._resolution


    @property
    def intensitySlider(self):
        
        return self._intensitySlider


    @property
    def intensityFactor(self):
        
        return self._intensityFactor


    @property
    def opacityFactor(self):
        
        return self._opacityFactor


    @property
    def opacitySlider(self):
        
        return self._opacitySlider


    @property
    def colorFactor(self):
        
        return self._colorFactor


    @property
    def colorSlider(self):
        
        return self._colorSlider


    def set_entries(self, origin, point1, point2, resolution):
        
        self.set_origin(origin)
        self.set_point1(point1)
        self.set_point2(point2)
        self.set_resolution(resolution)
        

    def set_origin(self, origin):
        
        for i,v in enumerate(origin):
            self._origin[i].SetValue('%.5g' % v)
        
        
    def set_point1(self, point1):
        
        for i,v in enumerate(point1):
            self._point1[i].SetValue('%.5g' % v)

        
    def set_point2(self, point2):
        
        for i,v in enumerate(point2):
            self._point2[i].SetValue('%.5g' % v)
            
            
    def set_resolution(self, resolution):
        
        self._resolution.SetValue(resolution)
    