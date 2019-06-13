import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import struct
import os.path as osp
import glob

dataRoot = 'output'
pointFiles = glob.glob(osp.join(dataRoot, '*_point.dat') )
frameNum = len(pointFiles )


for n in range(1, frameNum+1):
    name = osp.join(dataRoot, '%08d_point.dat' % n)
    with open(name, 'rb') as fIn:
        byte = fIn.read()
        pointNum = struct.unpack('I', byte[0:4])[0]
        print('Frame %s Point %d' % (n, pointNum) )

        pointArr = np.array(struct.unpack(str(pointNum*3) + 'f', \
                byte[4:]), dtype=np.float32)
        pointArr = pointArr.reshape([pointNum, 3] )

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d' )
        ax.set_xlim(-2.1, 2.1)
        ax.set_ylim(-2.1, 2.1)
        ax.set_zlim(-1.2, 1.5)
        ax.view_init(10, 20)
        ax.scatter(pointArr[:, 0], pointArr[:, 2], pointArr[:, 1],
                marker='.', s = 0.5, alpha = 1)
        imName = osp.join(dataRoot, '%08d_point.png' % n)
        #fig.savefig(imName )
        #fig.clf()
        plt.show()
        plt.close()
