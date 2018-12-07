from matplotlib.patches import Ellipse

class EllipseInteractor(object):
    
    _UNSELECTED = (0,0,1)
    _SELECTED = (1,0,0)

    def __init__(self, figure, connect=True) :

        self._ellipse = None
        
        self._figure = figure

        self._keyPressEvent = None
        self._scrollEvent = None
        self._pickEvent = None
        self._buttonReleaseEvent = None
        
        self.connect(connect)
        

    def connect(self, state=True):
        
        if state:        
            self._keyPressEvent = self._figure.canvas.mpl_connect("key_press_event", self.on_rotate_ellipse)
            self._scrollEvent = self._figure.canvas.mpl_connect("scroll_event", self.on_resize_ellipse)
            self._pickEvent = self._figure.canvas.mpl_connect("pick_event", self.on_pick_ellipse)
            self._buttonReleaseEvent = self._figure.canvas.mpl_connect("button_release_event", self.on_move_ellipse)

        else:
            self._figure.canvas.mpl_disconnect(self._keyPressEvent)
            self._figure.canvas.mpl_disconnect(self._scrollEvent)
            self._figure.canvas.mpl_disconnect(self._pickEvent)
            self._figure.canvas.mpl_disconnect(self._buttonReleaseEvent)


    def on_rotate_ellipse(self, event):
        
        if event.key == "+":
            ccw = -1.0
        elif event.key == "-":
            ccw = 1.0
        else:
            return
                        
        if self._ellipse is not None:
            self._ellipse.angle += ccw
            self._figure.canvas.draw()
               

    def on_resize_ellipse(self, event):
        
        if self._ellipse is not None :
            self._ellipse.width *= (1.0 + event.step/10.0)
            self._ellipse.height *= (1.0 + event.step/10.0)
            self._figure.canvas.draw()


    def on_pick_ellipse(self, event):
                        
        if event.mouseevent.button == 3:
            
            if self._ellipse is not None:
                self._ellipse.set_edgecolor(EllipseInteractor._UNSELECTED)
                
            if self._ellipse != event.artist:                                                
                self._ellipse = event.artist        
                self._ellipse.set_edgecolor(EllipseInteractor._SELECTED)
            else:
                self._ellipse = None

            self._figure.canvas.draw()
                                        

    def on_move_ellipse(self, event):

        if event.button == 3:
            return
        
        if self._ellipse is not None :
            newCenter = (event.xdata, event.ydata)
            self._ellipse.center = newCenter
            self._figure.canvas.draw()
                
        