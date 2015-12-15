#!/usr/bin/python


from numpy import linspace, fft, array
import matplotlib.pyplot as plt
from scipy.signal import firwin, lfilter

gyr_s = 65.5
acc_s = 2048
sample_rate = 1000

def import_imu(fname, time=True):

    acc = gyr = ts = []

    with open(fname,'r') as file:

        lines = file.readlines()
        splitdat = [[float(n) for n in x.strip().split("\t")] for x in lines]

        if time:
            ts  = [line[0] for line in splitdat]
            acc = [line[1:4] for line in splitdat]
            gyr = [line[4:] for line in splitdat]
        else:
            ts = [i for i in len(splitdat)]
            acc = [line[:3] for line in splitdat]
            gyr = [line[3:] for line in splitdat]

    return ts,acc,gyr

def get_axis(dat, sensitivity):

    x = [line[0]/sensitivity for line in dat]
    y = [line[1]/sensitivity for line in dat]
    z = [line[2]/sensitivity for line in dat]
    return x,y,z

def euler(v, dt, x0=0):

    x = [0]*(len(v)+1)
    x[0] = x0

    for i in range(len(x)-1):
        x[i+1] = x[i] + dt*v[i]

    return x

def fir_filter(dat, cutoff, sample_rate=1000):

    N = len(dat)
    nyq = sample_rate/2.0

    filter = firwin(N, cutoff/nyq)

    dat_smooth = lfilter(filter, 1.0, dat)

    return dat_smooth

def fft_filter(dat, cutoff):
    rft = fft.rfft(dat)
    rft[cutoff:] = 0
    dat_smooth = fft.irfft(rft)

    return dat_smooth

def plot_dat(dat, x=[], offset=0, linestyle='o'):

    if x == []:
        x = linspace(0, len(dat)*(1/sample_rate), len(dat))
    else:
        x = array(x)

    plt.plot(x-offset, dat, linestyle, linewidth=2)

def filt_plot(t, dat, cutoff, filter=fft_filter, sample_rate=1000):

    fdat = filter(dat, cutoff)

    # phase delay of FIR filtered signal
    delay = 0
    if filter == fir_filter:
        delay = 0.5 * (len(dat)-1) / sample_rate

    plot_dat(dat, t)
    plot_dat(fdat, t, offset=delay, linestyle='r-')
    plt.show()

def plot_sensor(fname, sensor="acc"):

    t,acc,gyr = import_imu(fname)


    if sensor == "acc":
        dx,dy,dz = get_axis(acc, acc_s)
    else:
        dx,dy,dz = get_axis(gyr, gyr_s)

    plot_dat(dx, t, 0, "-r")
    plot_dat(dy, t, 0, "-b")
    plot_dat(dz, t, 0, "-g")

    plt.show()
    plt.clf()