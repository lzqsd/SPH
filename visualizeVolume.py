import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
from skimage import measure
import numpy as np
import struct
import os.path as osp
import glob
import cv2

dataRoot = 'outputNoDivergence'
pointFiles = glob.glob(osp.join(dataRoot, '*_volume.dat') )
frameNum = len(pointFiles )

for n in range(169, frameNum +1):
    name = osp.join(dataRoot, '%08d_volume.dat' % n)
    with open(name, 'rb') as fIn:
        byte = fIn.read()
        size = struct.unpack('f', byte[0:4])[0]
        res = np.array(struct.unpack('3I', byte[4:16]), np.uint32)
        origin = np.array(struct.unpack('3f', byte[16:28]), np.float32)
        zAxis = np.array(struct.unpack('3f', byte[28:40]), np.float32)
        yAxis = np.array(struct.unpack('3f', byte[40:52]), np.float32)
        xAxis = np.array(struct.unpack('3f', byte[52:64]), np.float32)

        volumeNum = res[0] * res[1] * res[2]
        print('Frame %d/%d P %d' % (n, frameNum, volumeNum) )
        print('Resolution: %d %d %d' % ( res[0], res[1], res[2] ) )

        xcoord, ycoord, zcoord = np.meshgrid(
                np.arange(0.5, res[2]), np.arange(0.5, res[1]), np.arange(0.5, res[0]) )
        xcoord = xcoord.transpose([2, 0, 1] )
        ycoord = ycoord.transpose([2, 0, 1] )
        zcoord = zcoord.transpose([2, 0, 1] )

        xcoord = (xcoord * size)[:, :, :, np.newaxis] * xAxis[np.newaxis, np.newaxis, np.newaxis, :]
        ycoord = (ycoord * size)[:, :, :, np.newaxis] * yAxis[np.newaxis, np.newaxis, np.newaxis, :]
        zcoord = (zcoord * size)[:, :, :, np.newaxis] * zAxis[np.newaxis, np.newaxis, np.newaxis, :]

        coord = xcoord + ycoord + zcoord + origin[np.newaxis, np.newaxis, np.newaxis, :]
        xcoord = coord[:, :, :, 0]
        ycoord = coord[:, :, :, 1]
        zcoord = coord[:, :, :, 2]

        densityArr = np.array(struct.unpack(str(volumeNum) + 'f', \
                byte[64:]), dtype=np.float32)
        densityArr = densityArr.reshape((res[0], res[1], res[2]) )

        minDensity = densityArr.min()
        densityArr[ycoord < -1.1] = minDensity
        densityArr[np.logical_or(xcoord < -2.3, xcoord > 2.3)] = minDensity
        densityArr[np.logical_or(zcoord < -2.3, zcoord > 2.3)] = minDensity

        verts, faces, normals, _ = measure.marching_cubes_lewiner(-densityArr, 0)

        verts[:, 0] = verts[:, 0]
        verts[:, 1] = verts[:, 1]
        verts[:, 2] = verts[:, 2]

        verts = verts[:, 0:1] * xAxis[np.newaxis, :] * size \
                + verts[:, 1:2] * yAxis[np.newaxis, :] * size\
                + verts[:, 2:3] * zAxis[np.newaxis, :] * size + origin[np.newaxis, :]

        normals = normals[:, 0:1] * xAxis[np.newaxis, :] \
                + normals[:, 1:2] * yAxis[np.newaxis, :] \
                + normals[:, 2:3] * zAxis[np.newaxis, :]

        print('Vertex Num: %d' % verts.shape[0] )
        print('Normal Num: %d' % normals.shape[0] )
        print('Face Num: %d' % faces.shape[0] )

        faces = faces + 1

        with open(osp.join(dataRoot, '%08d.obj' % n ), 'w') as fOut:
            for n in range(verts.shape[0] ):
                fOut.write('v %.4f %.4f %.4f\n' %
                        (verts[n, 0], verts[n, 1], verts[n, 2] ) )
            for n in range(normals.shape[0] ):
                fOut.write('vn %.4f %.4f %.4f\n' %
                        (normals[n, 0], normals[n, 1], normals[n, 2] ) )
            for n in range(faces.shape[0] ):
                fOut.write('f %d//%d %d//%d %d//%d\n' %
                        (faces[n, 0], faces[n, 0], faces[n,1], \
                                faces[n,1], faces[n,2], faces[n,2]) )
