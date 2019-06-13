import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from skimage import measure
import numpy as np
import struct
import os.path as osp
import glob

dataRoot = 'output'
pointFiles = glob.glob(osp.join(dataRoot, '*_volume.dat') )
frameNum = len(pointFiles )


for n in range(1, frameNum+1):
    name = osp.join(dataRoot, '%08d_volume.dat' % n)
    with open(name, 'rb') as fIn:
        byte = fIn.read()
        res = np.array(struct.unpack('3I', byte[0:12]), np.uint32)
        volumeNum = res[0] * res[1] * res[2]
        print('Frame %d/%d P %d' % (n, frameNum, volumeNum) )

        volumeArr = np.array(struct.unpack(str(volumeNum*4) + 'f', \
                byte[12:]), dtype=np.float32)
        volumeArr = volumeArr.reshape([volumeNum, 4] )
        densityArr = volumeArr[:, 3]
        print(densityArr )
        pointArr = volumeArr[:, 0:3]
        pointArr = pointArr[densityArr > 0, :]

        fig = plt.figure()
        ax = fig.add_subplot(111, projection='3d' )

        ax.set_xlim(-2.1, 2.1)
        ax.set_ylim(-2.1, 2.1)
        ax.set_zlim(-1.2, 1.5)
        ax.view_init(10, 20)
        ax.scatter(pointArr[:, 0], pointArr[:, 2], pointArr[:, 1],
                marker = '.', s = 1, alpha = 1 )
        imName = osp.join(dataRoot, '%08d_volumeAsPoint.png' % n)
        plt.show()
        fig.savefig(imName )
        fig.clf()
        plt.close()
