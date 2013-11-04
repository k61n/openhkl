import wx

from Controller import Controller

if __name__ == "__main__":
    
    app = wx.App(False)
    
    controller = Controller(app)
    
    app.MainLoop()