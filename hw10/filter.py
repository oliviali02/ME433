# cd into your folder and make an env like this:
    # python3 -m venv env
    # source env/bin/activate

import csv
import numpy as np
import matplotlib.pyplot as plt 

def computeSampleRate(t_arr):
    sr = int(len(t_arr) / (t_arr[-1] - t_arr[0]))
    return sr

def takefft(t, data):
    Fs = computeSampleRate(t)
    dt = 1/Fs 
    print("dt = ", dt)

    Ts = 1/Fs; # sampling interval
    ts = np.arange(0,t[-1],Ts) # time vector

    y = data # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n)
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]

    return frq, Y

def computefft(t1, d1, t2, d2):
    frq1, Y1 = takefft(t1, d1)
    frq2, Y2 = takefft(t2, d2)

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t1,d1,'k', t2, d2, 'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    # ax1.set_title('Signal D: A = 0.9 and B = 0.1')
    # ax1.set_title('Signal D: Averaging Over 25 Data Points')
    ax2.loglog(frq1, abs(Y1),'k', frq2, abs(Y2),'r') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()

def plotfft(t, d, signalName):
    frq, Y = takefft(t, d)

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,d,'k')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f'Signal {signalName}')
    # ax1.set_title('Signal D: Averaging Over 25 Data Points')
    ax2.loglog(frq, abs(Y),'k') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()

# importing the data
def importData(filename):
    t = [] # column 0
    data = [] # column 1

    with open(filename) as f:
        # open the csv file
        reader = csv.reader(f)
        for row in reader:
            # read the rows 1 one by one
            t.append(float(row[0])) # leftmost column
            data.append(float(row[1])) # second column

    return t, data

# moving average filter
def maf(mafNum, t, data):
    mafA = []
    mafT = []
    avg = 0

    for i in range(mafNum, len(data)):
        # sum previous points
        avg = sum(data[i - mafNum : i])/mafNum
        mafA.append(avg)
        mafT.append(t[i])
    
    computefft(t, data, mafT, mafA)



# iir
def iir(A, B, t, data):
    if ((A+B) != 1):
        raise ValueError("A and B must add up to one!")
        
    iirA = []
    iirT = []
    avg = np.zeros(len(data))

    for i in range(len(data)):
        avg[i] = A * avg[i-1] + B * data[i] 
        iirA.append(avg[i])
        iirT.append(t[i])

    computefft(t, data, iirT, iirA)

# fir 

# extract data from csv files to lists     
tA, dataA = importData('sigA.csv')
tB, dataB = importData('sigB.csv')
tC, dataC = importData('sigC.csv')
tD, dataD = importData('sigD.csv')

# plotfft(tA, dataA, 'A')
# plotfft(tB, dataB, 'B')
# plotfft(tC, dataC, 'C')
# plotfft(tD, dataD, 'D')

# maf(50, tA, dataA)
# maf(100, tB, dataB)
# maf(50, tC, dataC)
# maf(25, tD, dataD)

# iir(0.9, 0.1, tA, dataA)
# iir(0.95, 0.05, tB, dataB)
# iir(0.5, 0.5, tC, dataC)
# iir(0.9, 0.1, tD, dataD)



