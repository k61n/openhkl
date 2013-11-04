import wx
from wx.lib.pubsub import Publisher as pub

from Main import MainFrame

class Controller(object):
    
    def __init__(self, app):
        
        self._app = app
        
        self._mainFrame = MainFrame(None)
        
        self._mainFrame.Show()
        
        self.build_menu_events()
        
        self.build_data_events()
        
        
    def build_data_events(self):
        
        self._mainFrame.Bind(wx.EVT_TREE_ITEM_ACTIVATED, self.on_add_workspace, self._mainFrame._dataTree)

        pub.subscribe(self.on_display_3d_view, "display_3d_view")

        pub.subscribe(self.on_add_cutplane, "add_cutplane")
        
        
    def build_menu_events(self):
        
        self._mainFrame.Bind(wx.EVT_MENU, self.on_load_data, self._mainFrame._loadDataItem)
        self._mainFrame.Bind(wx.EVT_MENU, self.on_quit, self._mainFrame._quitItem)
        
        
    def on_add_cutplane(self, message):
        
        workspace = self._mainFrame.active_page
        
        workspace.add_cutplane()
        
        
    def on_display_3d_view(self, message):
        
        workspace = self._mainFrame.active_page
        
        workspace.display_3d_view()
                
        
    def on_add_workspace(self, event):
        
        filename = self._mainFrame._dataTree.GetItemData(event.GetItem()).GetData()

        self._mainFrame.add_workspace(filename)

        
    def on_load_data(self, event):
        
        self._mainFrame.load_data()

        
    def on_quit(self, event):
        
        self._mainFrame.quit()        
        