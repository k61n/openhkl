import scipy.optimize as opt 
from numpy import * 
from matplotlib.pyplot import * 

def readSimulation(filename):
    x,y=loadtxt(filename,unpack=True)
    return x,y
   
fitstrip=  lambda p,x: p[0]*(cos((x-127.5)/127.5*15.0*pi/180.0))**p[1]    
residuals= lambda p,x,y: fitstrip(p,x)-y 

vana=zeros((256,640))

#f=figure()

for i,v in enumerate(range(0,640,10)+[639]):
    x,y=readSimulation("strip_%s"%v) 
    pinit=[5.0e-8,3.0]
    pfinal, success = opt.leastsq(residuals, pinit, args=(x,y))
#    ax=f.add_subplot(8,9,i)
#    ax.plot(x,y,'ro')
#    ax.plot(fitstrip(pfinal,arange(256)))
    vana[:,v]=fitstrip(pfinal,arange(256))
    
#show()

#f2=figure() 
#f2.add_subplot(111)
#imshow(vana,cmap='hot')

# Here starts the interpolation

for i in range(1,630):
    if (i%10==0):
        continue
    x=10*(i/10)
    y=x+10
    print x, y
    vana[:,i]=vana[:,x]+(vana[:,y]-vana[:,x])/10.0*(i-x)
    
for i in range(631,639):
    vana[:,i]=vana[:,630]+(vana[:,639]-vana[:,630])/9.0*(i-630)
