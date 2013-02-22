from numpy import * 
from matplotlib.pyplot import * 
import scipy.optimize as opt 

# Load the 8 mm vanadium data (sum of numors 95244 to 95259)
vandata=loadtxt("8mmVan-data")
# Skip all data in the noisy part of the detector 505-640 wires
vandata=vandata[:,0:505]
# Sum all counts in each wire to give a value of the electronic fluctation (wire from wire) 
s=sum(vandata,axis=0)
# Normalize
vandata/=s/256
# Fit each stip with the magic EPLC function
center=127.5
fitexp= lambda p,x: p[0]*cosh((x-center)/p[1])+p[2]
resexp= lambda p,x,y: fitexp(p,x)-y
pinit=[-4e-2,30.,1.0]
vanfit=zeros(vandata.shape)


for i in range(vandata.shape[1]): 
    pfinal, success = opt.leastsq(resexp, pinit, args=(arange(2,253),vandata[2:253,i]))
    vanfit[:,i]=fitexp(pfinal,arange(256))

# Normalize the fits to have approx. 1.0 in the center
vanfit/=average(vanfit[128,:])*s/average(s)