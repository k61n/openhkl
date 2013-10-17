import numpy

import wx
import wx.aui as aui
from wx.lib.pubsub import Publisher as pub

from libD19 import Scan2D

from Settings import Settings
from FrameViewer import FrameViewer

class WorkspaceData(object):
    
    def __init__(self, filename):
        
        self._scan = Scan2D()
        
        self._scan.readFromFile(str(filename))
        
        self._currentFrame = None
                                        
        pub.sendMessage("LOAD SCAN", self)
                        
        
    @property
    def scan(self):
        
        return self._scan
    
    
    @property
    def currentFrame(self):
        
        return self._currentFrame
    
    
    def set_frame(self, idx):
        
        self._currentFrame = self._scan.getFrame(idx)
        
        pub.sendMessage("SET NEW FRAME", self)
    


class WorkspacePanel(wx.Panel):
    
    def __init__(self, *args, **kwargs):
        
        wx.Panel.__init__(self, *args, **kwargs)
        
        self._workspace = aui.AuiManager(self)


    def add_settings(self, panel):

        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Right().Caption("Settings").CaptionVisible(True).CloseButton(False).MinSize((300,300)))
        self._workspace.Update()

        
    def add_frame_viewer(self, panel):

        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Center().CaptionVisible(False).CloseButton(False).MinSize((400,300)))
        self._workspace.Update()


class WorkspaceController(object):
    
    def __init__(self, parent, filename):
        
        self._data = WorkspaceData(filename)
                
        self._workspace = WorkspacePanel(parent)
                                
        self._frameViewer = FrameViewer(self._workspace)
        self._workspace.add_frame_viewer(self._frameViewer)

        self._settings = Settings(self._workspace)
        self._workspace.add_settings(self._settings)
        
        # wx events
        self._workspace.Bind(wx.EVT_SCROLL, self.on_set_new_frame, self._settings.frameSlider)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_set_new_frame, self._settings.frameNumber)
        pub.subscribe(self.msg_set_new_frame, "SET NEW FRAME")


        self._settings.set_frame_range(self._data.scan.getNFrames())      
        self._data.set_frame(0)
                
        
    @property
    def workspace(self):
        
        return self._workspace
            

    def msg_set_new_frame(self, message):
        
        data = message.data

        if data != self._data:
            return       
                        
        self._frameViewer.set_new_frame(data.currentFrame)
        
        
    def on_set_new_frame(self, event):
        
        val = int(event.GetEventObject().GetValue())
        
        self._settings.frameSlider.SetValue(val)
        self._settings.frameNumber.SetValue(str(val))
        
        self._data.set_frame(val)
        
        
        
