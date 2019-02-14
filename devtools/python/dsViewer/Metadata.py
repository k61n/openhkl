import wx
import wx.grid as  gridlib

class Metadata(wx.Panel):
    
    def __init__(self, parent, metadata, *args, **kwargs):
        
        wx.Panel.__init__(self, parent, *args, **kwargs)
        
        sizer = wx.BoxSizer(wx.HORIZONTAL)

        self._metadata = metadata
        
        self._grid = gridlib.Grid(self, style=gridlib.GRID_AUTOSIZE|gridlib.GRID_DEFAULT_ROW_LABEL_WIDTH)
        self._grid.SetRowLabelAlignment(wx.ALIGN_LEFT, wx.ALIGN_CENTRE)
        self._grid.SetRowLabelSize(150)
        self._grid.SetColLabelSize(0)
        
        self._grid.CreateGrid(len(self._metadata),1)
        
        for i, (k, v) in enumerate(self._metadata.items()):
            self._grid.SetCellAlignment(i, 0, wx.ALIGN_RIGHT, wx.ALIGN_CENTRE)
            self._grid.SetRowLabelValue(i, k)
            self._grid.SetCellValue(i, 0, v)
            self._grid.SetReadOnly(i,0,True)
        
        sizer.Add(self._grid, 1, wx.EXPAND|wx.ALL, 5)
         
        self.SetSizer(sizer)        

        sizer.Fit(self)
        
        self.Layout()
        