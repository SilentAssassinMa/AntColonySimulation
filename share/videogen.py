import matplotlib

matplotlib.use('agg')
import matplotlib.pyplot as plt
import numpy as np

points = {'px': [], 'py': [], 'ox': [], 'oy': []}
with open("out.dat", 'r') as f:
    data = f.readlines()
    for mstr in data:
        i, pxi, pyi, oxi, oyi = mstr.strip('\n').split(' ')
        if int(i) == 0:
            points['px'].append([])
            points['py'].append([])
            points['ox'].append([])
            points['oy'].append([])
        points['px'][-1].append(float(pxi))
        points['py'][-1].append(float(pyi))
        points['ox'][-1].append(float(oxi))
        points['oy'][-1].append(float(oyi))

env = {
    'px': [20, 80, 45],
    'py': [20, 80, 40],
    's': [11000, 11000, 2700],
    'c': ['red', 'blue', 'green']
}

for i in range(len(points['px'])):
    print("drawing ", i, end="\r")
    frame = {
        'px': points['px'][i],
        'py': points['py'][i],
        'ox': points['ox'][i],
        'oy': points['oy'][i],
        's': 10 * np.ones(len(points['px'][i]))
    }
    plt.figure(figsize=(10, 10), dpi=100)
    plt.scatter('px', 'py', c='c', s='s', data=env)
    plt.scatter('px', 'py', c='black', s='s', data=frame)
    plt.axis([0, 100, 0, 100])
    plt.savefig('plots/plot%d' % i)
    plt.clf()
    plt.close()

import cv2

img_root = "plots/plot"
fps = 20
file_path = 'simulation.mp4'
size = (1000, 1000)

fourcc = cv2.VideoWriter_fourcc(*'mp4v')  # mp4

videoWriter = cv2.VideoWriter(file_path, fourcc, fps, size)

for i in range(0, 500):
    print("processing ", i, end="\r")
    img = cv2.imread(img_root + str(i) + ".png")
    videoWriter.write(img)

videoWriter.release()