---
author: Alexandra Nilles
title: Weaselball motion tracking
execute: path_gen.py PathGen.hs
...


Importing and formatting data
-----------------------------

The IMU data is read by the arduino microcontroller and data is logged to the SD
card in the following format (as binary 16-bit numbers, no whitespace):
`time acc_x acc_y acc_z gyr_x gyr_y gyr_z`

`binary_to_str.py` unpacks the binary data and writes a tab-separated line of
values for each data point. Data points are separated by newlines.

```python{exec:path_gen.py}

#!/usr/bin/python

from numpy import linspace, fft, array
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter
import math

# quaternion math library, by me
import quaternions as q

# Import and format data
# ----------------------

# hardware-specific sensitivities and sampling rate
gyr_s = 32.8
acc_s = 2048
s_rate = 420 # Hz, approximate, for nyquist rate in filters

# stationary offset at room temp
gx_offset = 47
gy_offset = 45
gz_offset = -105

def deg_to_rad(deg):
    return (math.pi*deg)/180.0

# sensitivity depends on resolution settings of sensor
def unit_correct(ts, acc, gyr):

        gyr = [[dat[0] - gx_offset,
                dat[1] - gy_offset,
                dat[2] - gz_offset] for dat in gyr]

        # convert ts to seconds
        ts  = [t/1000.0 for t in ts]
        acc = [[a/acc_s for a in dat] for dat in acc]
        # radians/sec for gyro
        gyr = [[deg_to_rad(g/(gyr_s)) for g in dat] for dat in gyr]

        return ts, acc, gyr

# get timestamps, gyro and accelerometer data from IMU
def import_imu(fname, units=True):

    acc = gyr = ts = []

    with open(fname,'r') as file:

        lines = file.readlines()
        splitdat = [[float(n) for n in x.strip().split()] for x in lines]

        ts  = [line[0] for line in splitdat]
        acc = [[a for a in line[1:4]] for line in splitdat]
        gyr = [[g for g in line[4:]] for line in splitdat]

    if units:
        ts, acc, gyr = unit_correct(ts, acc, gyr)

    return ts,acc,gyr


# gyr_reading is a list of one x,y,z data point from IMU
# use direction, magnitude of w*dt to construct quaternion representation
def gyr_to_q(dt, gyr_reading):

    # deg/sec
    w_mag = math.sqrt(sum([g*g for g in gyr_reading]))

    # convert to rad/sec
    quaternion = q.mkq( dt*w_mag,
                        gyr_reading[0],
                        gyr_reading[1],
                        gyr_reading[2])
    return quaternion

# take gyro data, return quaternion representation
def gyr_data_to_qs(dts, gyr_data):

    quats = [gyr_to_q(dt, gyr_reading) for (dt, gyr_reading) in zip(dts, gyr_data[1:])]

    return quats

```

Filter data (low pass noise removal)
------------------------------------

Data is kind of noisy, smooth by removing high-frequency components (optional).

```python{exec:path_gen.py}

# finite impulse response low-pass filter
# produces phase shift in filtered data
def fir_filter(dat, cutoff, sample_rate=s_rate):

    N = len(dat)
    nyq = sample_rate/2.0

    filter = firwin(N, cutoff/nyq)

    dat_smooth = lfilter(filter, 1.0, dat)

    return dat_smooth

# fast fourier transform low pass filter
def fft_filter(dat, cutoff):
    rft = fft.rfft(dat)
    rft[cutoff:] = 0
    dat_smooth = fft.irfft(rft)

    return dat_smooth

def run_filter(fname, cutoff, save_f="filtered"):

    ts,acc,gyr = import_imu(fname)

    gx,gy,gz = get_axis(gyr)
    ax,ay,az = get_axis(acc)

    gx = fft_filter(gx, cutoff)
    gy = fft_filter(gy, cutoff)
    gz = fft_filter(gz, cutoff)
    ax = fft_filter(ax, cutoff)
    ay = fft_filter(ay, cutoff)
    az = fft_filter(az, cutoff)

    with open(save_f,'w') as file:

        for line in zip(ts,ax,ay,az,gx,gy,gz):
            for elem in line:
                file.write(str(elem)+'\t')
            file.write('\n')


```

Heading estimation
------------------

Take gyrometer readings and estimate orientation of IMU

```python{exec:path_gen.py}

# heading:      quaternion representation of heading in lab frame
# frame_rot:    quat rep of how far measurement frame has rotated from lab frame
# new_q:        quat rep of new gyro measurement
# returns:      quat, rotation and new heading in lab frame after measured rotation
def update_heading(heading, frame_rot, new_q):

    # rotate new_q back into lab frame
    lab_q = q.rot_vector(q.conj(frame_rot), new_q)

    # rotate heading
    new_heading = q.rot_vector(lab_q, heading)

    return lab_q, new_heading

# initial heading - set to "up" arbitrarily
# could calibrate with gravity vector
# h = 0 + xi + yj + zk
# returns list of headings, one per time stamp
def rotate_heading(qs, first_heading=[0,0,0,1]):

    # list of heading estimates, lab frame
    h_list = [[0.0,0.0,0.0,0.0]]*len(qs)
    h_list[0] = first_heading

    # list of total rotation from start, lab frame
    q_list = [[1.0,0.0,0.0,0.0]]*len(qs)

    for i in range(1,len(h_list)):

        q_list[i], h_list[i] = update_heading(h_list[i-1], q_list[i-1], qs[i])

    return q_list, h_list

def rotate_omega(dts, gyr, total_rot):

    q_rep = [[0]+g for g in gyr[1:]]

    frame_gs = [0]*len(q_rep)

    for (i,(t_rot, g)) in enumerate(zip(total_rot, q_rep)):

        frame_gs[i] = q.rot_vector(q.conj(t_rot), g)
        frame_gs[i][0] = dts[i]

    return frame_gs

def data_to_heading(fname, plot=False, units=True):

    ts, acc, gyr = import_imu(fname, units)

    dts = [t1-t0 for (t0, t1) in zip(ts, ts[1:])]

    qs = gyr_data_to_qs(dts, gyr)

    total_rot, headings = rotate_heading(qs)

    fixed_frame_omega = rotate_omega(dts, gyr, total_rot)

    print(headings)

    if plot:
        plot_heading(ts, headings)

    with open(fname+"_headings",'w') as file:

        for h in headings:
            file.write(str(h))
            file.write('\n')

    with open(fname+"_angvel",'w') as file:

        for h in fixed_frame_omega:
            file.write(str(h))
            file.write('\n')

def format_data(fname, units=True):

    ts, acc, gyr = import_imu(fname, units)

    dts = [t1-t0 for (t0, t1) in zip(ts, ts[1:])]

    gyr_data = [[dt]+g for (dt,g) in zip(dts, gyr)]

    with open(fname+"_data",'w') as file:

        for h in gyr_data:
            file.write(str(h))
            file.write('\n')


```

Path Reconstruction
-------------------


```haskell{exec:PathGen.hs}

#!/usr/bin/runhaskell

module PathGen where

import Quaternions

-- transform observed rotation back into inertial frame
change_frame :: Quat -> Quat -> Quat
change_frame frame_rot q = normalize $ rotate (qconj frame_rot) q

-- list of total frame rotation from start for each time step
total_rotation :: [Quat] -> [Quat]
total_rotation noninertial_qs = scanl change_frame (1,0,0,0) noninertial_qs

-- rotate angular velocities back into inertial frame
make_inertial :: [Quat] -> [Quat]
make_inertial av = [change_frame tr w | (tr, w) <- zip tots_rots av]
    where tots_rots = total_rotation av

import_vectors :: FilePath -> IO [[Double]]
import_vectors fname = do
    content <- readFile fname
    let dat = map read $ lines content
    return dat

--make_path :: FilePath -> IO [[Double]]
make_path fname = do
    let radius = 4.0 -- centimeters
    dat <- import_vectors fname
    let ts = map head dat
    -- direction of ang vel == rotation axis
    -- direction of last three elements of quat == rotation axis
    -- magnitude of ang vel == magnitude of rotation
    let noninertial_angvels = map (drop 1) dat
    -- mkq makes normalized quaternions even if input isn't normalized
    let noninertial_qs = [mkq theta x y z | (x:y:z:[]) <- noninertial_angvels,
                                            let theta = norm (0,x,y,z)]

    let inertial_qs = make_inertial noninertial_qs

    let rotation_vectors = [(b,c,d) | (a,b,c,d) <- inertial_qs]


    let writename = fname++"_rot_vectors" :: FilePath
    writeFile writename $ unlines (map show rotation_vectors)

```


Plotting
--------

```python{exec:path_gen.py}

import pygame
from time import sleep
from mpl_toolkits.mplot3d import Axes3D

XDIM = 500
YDIM = 500
WINSIZE = [XDIM, YDIM]

def window_scale(p):
     return (int(p[0]+XDIM/2.0),int(YDIM - p[1] - YDIM/2.0))

def plot_rot_axis(fname):

    num = -5000

    with open(fname,'r') as file:

        dat = file.readlines()
        nice_dat = [d.strip().strip("()").split(',') for d in dat]
        xs = [float(x[0]) for x in nice_dat]
        ys = [float(x[1]) for x in nice_dat]
        zs = [float(x[2]) for x in nice_dat]

    xs = xs[:num]
    ys = ys[:num]
    zs = zs[:num]

    arrow_x = [0]*len(xs)
    arrow_y = [0]*len(xs)
    arrow_z = [0]*len(xs)


    fig = plt.figure()
    ax = fig.gca(projection='3d')

    ax.set_xlim3d(left=-2.0,right=2.0)
    ax.set_ylim3d(bottom=-2.0,top=2.0)
    ax.set_zlim3d(bottom=-2.0,top=2.0)

    ax.quiver(arrow_x, arrow_y, arrow_z, xs, ys, zs, pivot='tail', length=1,
    arrow_length_ratio=0.01)

    plt.show()

    print(xs)


def plot_path(fname):

    with open(fname,'r') as file:

        dat = file.readlines()
        x = [float(xelem) for xelem in x.strip().strip("[]").split(',')]
        y = [float(xelem) for xelem in y.strip().strip("[]").split(',')]
        z = [float(xelem) for xelem in z.strip().strip("[]").split(',')]

    fig = plt.figure()
    ax = fig.gca(projection='3d')


    ax.plot(x, y, z)

    ax.set_xlabel('X (cm)')
    ax.set_ylabel('Y (cm)')
    ax.set_zlabel('Z (cm)')

    plt.show()



def plot_heading(ts, headings):

    xdat = [dat[1] for dat in headings]
    ydat = [dat[2] for dat in headings]
    zdat = [dat[3] for dat in headings]

    pygame.init()
    screen = pygame.display.set_mode(WINSIZE)

    white = (255, 255, 255)
    black = (0,0,0)
    prev_t=0.0

    for (t,x,y) in zip(ts,xdat,ydat):
        x, y = x*100.0, y*100.0
        screen.fill(white)
        pygame.draw.line(screen, black, window_scale((0,0)), window_scale((x,y)), 5)
        pygame.display.set_caption(str(t))
        pygame.display.update()
        dt = t-prev_t
        prev_t = t
        sleep(dt)


    pygame.display.quit()

# separate axis for one sensor
def get_axis(dat):

    x = [line[0] for line in dat]
    y = [line[1] for line in dat]
    z = [line[2] for line in dat]
    return x,y,z

def plot_dat(dat, x=[], offset=0, linestyle='o'):

    if x == []:
        x = linspace(0, len(dat)*(1/s_rate), len(dat))
    else:
        x = array(x)

    plt.plot(x-offset, dat, linestyle, linewidth=2)


def filt_plot(t, dat, cutoff, filter=fft_filter, sample_rate=s_rate):

    fdat = filter(dat, cutoff)

    # phase delay of FIR filtered signal
    delay = 0
    if filter == fir_filter:
        delay = 0.5 * (len(dat)-1) / sample_rate

    plot_dat(dat, t)
    plot_dat(fdat, t, offset=delay, linestyle='r-')
    plt.show()

def plot_sensor(fname, sensor="acc", units=True):

    t,acc,gyr = import_imu(fname, units)

    if sensor == "acc":
        dx,dy,dz = get_axis(acc)
        plt.ylabel("Acc measurement (m/sec^2)")
    else:
        dx,dy,dz = get_axis(gyr)
        plt.ylabel("Gyro measurement (Rad/sec)")

    plt.xlabel("Time (sec)")
    plot_dat(dx, t, 0, "-r")
    plot_dat(dy, t, 0, "-b")
    plot_dat(dz, t, 0, "-g")

    plt.show()
    plt.clf()

```
