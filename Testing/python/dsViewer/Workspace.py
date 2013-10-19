import collections

import numpy

import wx
import wx.aui as aui
from wx.lib.pubsub import Publisher as pub

from libD19 import Scan2D

from Metadata import Metadata
from Settings import Settings
from FrameViewer import FrameViewer


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


    def add_metadata(self, panel):

        self._workspace.AddPane(panel, aui.AuiPaneInfo().Dockable().Floatable().Right().Caption("Metadata").CaptionVisible(True).CloseButton(False).MinSize((400,300)))
        self._workspace.Update()


class WorkspaceController(object):
    
    def __init__(self, parent, scan):
        
        self._scan = scan
                
        self._workspace = WorkspacePanel(parent)
                                
        self._frameViewer = FrameViewer(self._workspace)
        self._workspace.add_frame_viewer(self._frameViewer)

        self._settings = Settings(self._workspace)
        self._workspace.add_settings(self._settings)

        self._metadata = Metadata(self._workspace, self._scan.metadata)
        self._workspace.add_metadata(self._metadata)
        
        # wx events
        self._workspace.Bind(wx.EVT_CHECKBOX, self.on_show_ellipse, self._settings.showEllipse)
        self._workspace.Bind(wx.EVT_SCROLL_CHANGED, self.on_set_new_frame, self._settings.frameSlider)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_set_new_frame, self._settings.frameNumber)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_search_blobs, self._settings.threshold)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_search_blobs, self._settings.minSize)
        self._workspace.Bind(wx.EVT_TEXT_ENTER, self.on_search_blobs, self._settings.maxSize)
        self._workspace.Bind(wx.EVT_BUTTON, self.on_search_blobs, self._settings.searchBlobs)
        pub.subscribe(self.msg_set_new_frame, "SET NEW FRAME")
        pub.subscribe(self.msg_show_ellipses, "GET ELLIPSES")
        pub.subscribe(self.msg_blobs_search, "BLOB SEARCH")

        self._settings.set_frame_range(self._scan.nFrames-1)
        
        self.set_frame(0)


    def set_frame(self, idx):

        self._scan.set_frame(idx)
                
        
    @property
    def workspace(self):
        
        return self._workspace
            

    def msg_set_new_frame(self, message):
        
        scan = message.data

        if scan != self._scan:
            return       
                        
        self._frameViewer.set_new_frame(scan)
        
        if self._settings.showEllipse.GetValue():
            scan.get_ellipses()


    def msg_show_ellipses(self, message):
        
        scan = message.data

        if scan != self._scan:
            return
                                
        self._frameViewer.show_ellipses(scan.ellipses)
        
        
    def on_set_new_frame(self, event):
                
        val = int(event.GetEventObject().GetValue())
        
        self._settings.frameSlider.SetValue(val)
        self._settings.frameNumber.SetValue(str(val))
        
        self._scan.set_frame(val)
        
        
    def on_show_ellipse(self, event):
        
        if event.GetEventObject().GetValue():
            self._scan.get_ellipses()
        
        
    def on_search_blobs(self, event):
        
        threshold = float(self._settings.threshold.GetValue())
        minSize = self._settings.minSize.GetValue()
        maxSize = self._settings.maxSize.GetValue()
                
        nBlobs = self._scan.search_blobs(threshold, minSize, maxSize)
                
        
    def msg_blobs_search(self, message):
        
        self._frameViewer.update_blobs_search(message.data)
                
        
        
        

        
        
        
        
