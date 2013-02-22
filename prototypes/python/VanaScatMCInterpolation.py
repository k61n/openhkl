import scipy.optimize as opt 
from numpy import * 
from matplotlib.pyplot import * 

def readSimulation(filename):
    x,y=loadtxt(filename,unpack=True)
    return x,y
   
#fitstrip=  lambda p,x: p[0]*(cos((x-127.5)/127.5*15.0*pi/180.0))**p[1]    
#residuals= lambda p,x,y: fitstrip(p,x)-y 

fitexp= lambda p,x: p[0]*cosh((x-127.5)/p[1])+p[2]
resexp= lambda p,x,y: fitexp(p,x)-y
pinit=[-4e-2,30.,1.0]

vana=zeros((256,640))

f=figure()

for i,v in enumerate(range(0,640,10)+[639]):
    x,y=readSimulation("/home/chapon/Dropbox/D19-project/VanadiumNormalizationProcedure/MC_8mmVan_10mmDiaphragm/strip_%s"%v) 
    #pinit=[5.0e-8,3.0]
    pfinal, success = opt.leastsq(resexp, pinit, args=(x,y))
    vana[:,v]=fitexp(pfinal,arange(256))
    
ax=f.add_subplot(111)
ax.plot(x,y,'ro')
ax.plot(fitexp(pfinal,arange(256)))
show()

f2=figure() 
f2.add_subplot(111)
imshow(vana,cmap='hot')

# Here starts the interpolation

for i in range(1,630):
    if (i%10==0):
        continue
    x=10*(i/10)
    y=x+10
    vana[:,i]=vana[:,x]+(vana[:,y]-vana[:,x])/10.0*(i-x)
    
for i in range(631,639):
    vana[:,i]=vana[:,630]+(vana[:,639]-vana[:,630])/9.0*(i-630)
    
# vana dans le bon sens 
vana=vana[:,::-1]
vana/=average(vana[128,:])
vana=vana[:,0:505]
