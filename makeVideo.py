import os.path as osp
import glob
import numpy as np
import cv2
import os


dataRoot1 = 'output'
dataRoot2 = 'outputNoDivergence'
dataRoot = 'CompareSurface'
suffix = '_fluid_1'
os.system('mkdir %s' % dataRoot )

for n in range(1, 211):
    im1 = cv2.imread(osp.join(dataRoot1, '%08d' % n + suffix + '.png') )
    im2 = cv2.imread(osp.join(dataRoot2, '%08d' % n + suffix + '.png') )
    im = np.zeros((360, 480 * 2 + 10, 3), dtype=np.uint8) + 255
    im[:, 0:480, :] = im1
    im[:, 490:, :] = im2
    cv2.imwrite(osp.join(dataRoot, '%08d' % n + suffix + '.png'), im)

frameRate = 30
cmd = 'ffmpeg -r %d -f image2 -s 480x320 -i %s -vcodec libx264 -crf 25 %s' \
        % (frameRate, osp.join(dataRoot, '%08d' + '%s.png' % suffix ), dataRoot + suffix + '.avi')
os.system(cmd )

