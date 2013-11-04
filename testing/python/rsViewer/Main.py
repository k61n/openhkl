import collections
import os

import wx
import wx.aui as aui
from wx.lib.pubsub import Publisher as pub

from Workspace import WorkspaceController

class Data(object):
    
    def __init__(self):
        
        self._data = []


    def add_data(self, name):
        
        if name in self._data:
            return
        
        self._data.append(name)
        
        pub.sendMessage("ADD DATA", name)
        
        
    def remove_data(self, name):
        
        idx = self._data.index(name)
        
        del self._data[idx]


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
    
    def __init__(self, parent, title="Reciprocal Space Viewer (demo version)"):
        
        wx.Frame.__init__(self, parent, wx.ID_ANY, title, size = (1300,800))

        # The menubar        
        self._menubar = MyMenuBar()
        self.SetMenuBar(self._menubar)
        
        # The pane manager
        self._paneManager = aui.AuiManager(self)

        # The data tree        
        self._dataTree = wx.TreeCtrl(self, wx.ID_ANY, style=wx.TR_HIDE_ROOT)
        self._dataTreeRoot = self._dataTree.AddRoot("root")

        self._paneManager.AddPane(self._dataTree, aui.AuiPaneInfo().Caption("Data").Name("data").Left().CloseButton(False).DestroyOnClose(False).MinSize((250,-1)))        

        self._notebook = aui.AuiNotebook(self)
        self._paneManager.AddPane(self._notebook, aui.AuiPaneInfo().Name("viewer").Center().CloseButton(False).DestroyOnClose(False).MinSize((250,-1)))        
        
        self._paneManager.Update()
        
        
    @property
    def menubar(self):
        return self._menubar
    
            
    def create_workspace(self, wksp, name):

        self._notebook.AddPage(wksp, name)         
        

    def add_data(self, data):
       
       key = os.path.basename(data)
        
       data = wx.TreeItemData(data) 
       
       self._dataTree.AppendItem(self._dataTreeRoot, key, data=data)

        
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
        
        self._data = Data()
        
        self._mainFrame = MainFrame(None)
        
        self._workspaces = collections.OrderedDict()
        
        self._mainFrame.Show()

        self._mainFrame.Bind(wx.EVT_MENU, self.on_load_data, self._mainFrame.menubar.load)
        self._mainFrame.Bind(wx.EVT_MENU, self.on_quit, self._mainFrame.menubar.quit)
       
        self._mainFrame.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.on_create_workspace, self._mainFrame._dataTree)

        pub.subscribe(self.on_add_data, "ADD DATA")
        
    
    def on_add_data(self, message):
        
        self._mainFrame.add_data(message.data)
        
        
    def on_create_workspace(self, event):
        
        filename = self._mainFrame._dataTree.GetItemData(event.GetItem()).GetData()
        
        self._workspaces[filename] = wksp = WorkspaceController(self._mainFrame._notebook, filename)
                
        name = os.path.basename(filename)

        self._mainFrame.create_workspace(wksp.workspace, name)
        
        
    def on_load_data(self, event):
        
        dialog = wx.FileDialog (None, message='Load data ...', style=wx.OPEN)
        
        if dialog.ShowModal() == wx.ID_CANCEL:
            return
                                        
        filename = dialog.GetPath()
        
        if not filename:
            return
        
        self._data.add_data(filename)

    
        
    def on_quit(self, event):
        
        self._mainFrame.quit()
        
        
        
        
#    def on_add_workspace(self, event):
#        
#        filename = self._main._dataTree.GetItemData(event.GetItem()).GetData()
#
#        self._main.add_workspace(filename)
        

        
        
if __name__ == "__main__":
    
    app = wx.App(False)
    
    controller = MainController(app)
    
    app.MainLoop()        
