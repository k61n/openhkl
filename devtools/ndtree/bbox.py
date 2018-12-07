import matplotlib
import matplotlib.pyplot as plt
import numpy

N = 10

data = numpy.zeros((N,4),dtype=numpy.float64)
data[0,:] = [9.73565,299.059,10.7356,300.059]
data[1,:] = [638.366,116.34,639.366,117.34]
data[2,:] = [795.125,113.763,796.125,114.763]
data[3,:] = [831.659,139.122,832.659,140.122]
data[4,:] = [987.952,191.498,988.952,192.498]
data[5,:] = [546.748,310.085,547.748,311.085]
data[6,:] = [223.824,964.179,224.824,965.179]
data[7,:] = [976.299,723.581,977.299,724.581]
data[8,:] = [503.626,874.646,504.626,875.646]
data[9,:] = [963.018,991.497,964.018,992.497]


#data = numpy.random.uniform(0,950,4*N)
#data = numpy.reshape(data,(N,4))
#print data

fig = plt.figure()
ax = fig.add_subplot(111)
for i,d in enumerate(data):
	rect = matplotlib.patches.Rectangle((d[0],d[1]), 1, 1, fill=False, label="toto")
	ax.add_artist(rect)
	rx, ry = rect.get_xy()
	cx = rx + rect.get_width()/2.0
	cy = ry + rect.get_height()/2.0
	ax.annotate(i, (cx, cy), color='b', weight='bold', fontsize=10, ha='center', va='center')
ax.set_aspect('equal')
plt.xlim([0,1000])
plt.ylim([0,1000])
plt.show()


