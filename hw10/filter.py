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

def plotfir(t, data, firT, firA, sigName, num, filter, cutoff, bandwidth):
    frq1, Y1 = takefft(t, data)
    frq2, Y2 = takefft(firT, firA)

    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.plot(t,data,'k', firT, firA, 'r')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f'Signal {sigName}: {filter} filter with {num} weights, cutoff frequency of {cutoff} Hz, and bandwidth of {bandwidth}')
    ax2.loglog(frq1, abs(Y1),'k', frq2, abs(Y2),'r') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()

# fir 
def fir(t, data, num, weights):
    filter = 'Blackman'
    cutoff = 4
    bandwidth = 100
    sigName = 'D'
    firA = []
    firT = []
    avg = 0

    data_np = np.array(data)
    weights_np = np.array(weights)

    for i in range(num, len(data)):
        avg = np.sum(weights_np * data_np[i - num: i])
        firA.append(avg)
        firT.append(t[i])

    plotfir(t, data, firT, firA, sigName, num, filter, cutoff, bandwidth)


# extract data from csv files to lists     
tA, dataA = importData('sigA.csv')
tB, dataB = importData('sigB.csv')
tC, dataC = importData('sigC.csv')
tD, dataD = importData('sigD.csv')

# determine sample rates 
# srA = computeSampleRate(tA) 10000
# srB = computeSampleRate(tB) 3300
# srC = computeSampleRate(tC) 2500
# srD = computeSampleRate(tD) 400

# print(srA, srB, srC, srD)
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


weightsA = [
    -0.000000000000000002,
    0.002116428440784334,
    0.009696733412607869,
    0.025297432075007394,
    0.050278944029643752,
    0.082419424191264606,
    0.115301681223568278,
    0.140165553138591248,
    0.149447606977064856,
    0.140165553138591248,
    0.115301681223568306,
    0.082419424191264676,
    0.050278944029643780,
    0.025297432075007384,
    0.009696733412607881,
    0.002116428440784338,
    -0.000000000000000002,
]
numA = 17

# fir(tA, dataA, numA, weightsA)

weightsB = h = [
    -0.000000000000000001,
    0.000282970589546487,
    0.001204877787865870,
    0.002941370083944624,
    0.005726606005070869,
    0.009797711077828329,
    0.015326522879102166,
    0.022352296249991133,
    0.030729748201279557,
    0.040104674753698104,
    0.049924704168249935,
    0.059486405988859063,
    0.068013147274924704,
    0.074752090746301278,
    0.079074740770195606,
    0.080564266846284163,
    0.079074740770195606,
    0.074752090746301264,
    0.068013147274924718,
    0.059486405988859098,
    0.049924704168249963,
    0.040104674753698111,
    0.030729748201279598,
    0.022352296249991150,
    0.015326522879102172,
    0.009797711077828329,
    0.005726606005070864,
    0.002941370083944625,
    0.001204877787865866,
    0.000282970589546489,
    -0.000000000000000001,
]
numB = 31
# fir(tB, dataB, numB, weightsB)

weightsC = [
    -0.000000000000000003,
    0.005246839840133090,
    0.025492605815363854,
    0.067118762410120408,
    0.124959158144569399,
    0.177631837132627146,
    0.199101593314371939,
    0.177631837132627146,
    0.124959158144569468,
    0.067118762410120450,
    0.025492605815363854,
    0.005246839840133108,
    -0.000000000000000003,
]
numC = 13
# fir(tC, dataC, numC, weightsC)

weightsD = [
    -0.000000000000000002,
    0.001457826841675081,
    0.006549295164930914,
    0.016882484264206098,
    0.033750668331437424,
    0.056803882125556170,
    0.083290418346488163,
    0.108358278141449257,
    0.126410562133999788,
    0.132993169300514108,
    0.126410562133999788,
    0.108358278141449299,
    0.083290418346488204,
    0.056803882125556170,
    0.033750668331437444,
    0.016882484264206136,
    0.006549295164930921,
    0.001457826841675083,
    -0.000000000000000002,
]
numD = 19
# fir(tD, dataD, numD, weightsD)




