import collections
import os

import wx
import wx.aui as aui
from wx.lib.pubsub import Publisher as pub

from Workspace import WorkspaceController
from Scan import Scan

class ScansCollection(collections.OrderedDict):

    def add_scan(self, filename):
        
        if filename in self:
            return
                
        self[filename] = Scan(filename)
        
        pub.sendMessage("ADD SCAN", self[filename])

        
    def remove_scan(self, name):
        
        pass
        
        

class MyMenuBar(wx.MenuBar):

    def __init__(self):
        
        wx.MenuBar.__init__(self)

        fileMenu = wx.Menu()
        self._load = fileMenu.Append(wx.ID_ANY, 'Load data')
        fileMenu.AppendSeparator()
        self._quit = fileMenu.Append(wx.ID_ANY, 'Quit')

        self.Append(fileMenu, 'File')
        
    
    @property
    def load(self):
        
        return self._load


    @property
    def quit(self):
        
        return self._quit


class MainFrame(wx.Frame):
    
    def __init__(self, parent, title="Detector Space Viewer (demo version)"):

        self._workspaces = collections.OrderedDict()
        
        wx.Frame.__init__(self, parent, wx.ID_ANY, title, size = (1300,800))

        # The menubar        
        self._menubar = MyMenuBar()
        self.SetMenuBar(self._menubar)
        
        # The pane manager
        self._paneManager = aui.AuiManager(self)

        # The data tree        
        self._dataTree = wx.TreeCtrl(self, wx.ID_ANY, style=wx.TR_HIDE_ROOT)
        self._dataTreeRoot = self._dataTree.AddRoot("root")

        self._paneManager.AddPane(self._dataTree, aui.AuiPaneInfo().Caption("Data").Name("data").Left().CloseButton(True).DestroyOnClose(False).MinSize((250,-1)))        

        self._notebook = aui.AuiNotebook(self)
        self._paneManager.AddPane(self._notebook, aui.AuiPaneInfo().Name("viewer").Center().CloseButton(True).DestroyOnClose(False).MinSize((250,-1)))        
        
        self._paneManager.Update()
        
        
    @property
    def menubar(self):
        return self._menubar
    

    def add_scan(self, scan):

        name = os.path.basename(scan.filename)
       
        self._dataTree.AppendItem(self._dataTreeRoot, name)

        self._workspaces[name] = WorkspaceController(self._notebook, scan)

        self._notebook.AddPage(self._workspaces[name].workspace, name)         

        
    def load(self):
    
        data = wx.TreeItemData(filename)
        
    
    def quit(self):
        
        d = wx.MessageDialog(None, 'Do you really want to quit ?', 'Question', wx.YES_NO|wx.YES_DEFAULT|wx.ICON_QUESTION)
        if d.ShowModal() == wx.ID_YES:
            self.Destroy()  
            
            
    @property
    def active_page(self):
        
        page = self._notebook.GetPage(self._notebook.GetSelection())
        
        return page
    
    
class MainController(object):
    
    def __init__(self, app):
        
        self._scans = ScansCollection()
        
        self._mainFrame = MainFrame(None)
                
        self._mainFrame.Show()

        # wx events
        self._mainFrame.Bind(wx.EVT_MENU, self.on_load_data, self._mainFrame.menubar.load)
        self._mainFrame.Bind(wx.EVT_MENU, self.on_quit, self._mainFrame.menubar.quit)
        pub.subscribe(self.msg_add_scan, "ADD SCAN")
                
                
    def msg_add_scan(self, message):
        
        self._mainFrame.add_scan(message.data)
        
        
    def on_load_data(self, event):
        
        dialog = wx.FileDialog (None, message='Load data ...', style=wx.OPEN)
        
        if dialog.ShowModal() == wx.ID_CANCEL:
            return
                                        
        filename = dialog.GetPath()
        
        if not filename:
            return        
        
        self._scans.add_scan(filename)
                                
        
    def on_quit(self, event):
        
        self._mainFrame.quit()
        
        
if __name__ == "__main__":
    
    app = wx.App(False)
    
    controller = MainController(app)
    
    app.MainLoop()        
