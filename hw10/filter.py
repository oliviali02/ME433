# cd into your folder and make an env like this:
    # python3 -m venv env
    # source env/bin/activate

import csv
import numpy as np
import matplotlib.pyplot as plt 

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
    # for i in range(len(t)):
    #     # print the data to verify it was read
    #     print(str(t[i]) + ", " + str(data[i]))

def plotData(t, data):
    plt.plot(t,data,'b-*')
    plt.xlabel('Time [s]')
    plt.ylabel('Signal')
    plt.title('Signal vs Time')
    plt.show()

# moving average filter
def maf(mafnum, t, d):
    mafA = []
    mafTA = []
    avg = 0
    
    for i in range(mafnum, len(d)):
        # sum previous points
        avg = sum(d[i - mafnum: i])/mafnum

# iir

# fir 

# extract data from csv files to lists     
tA, dataA = importData('sigA.csv')
tB, dataB = importData('sigB.csv')
tC, dataC = importData('sigC.csv')
tD, dataD = importData('sigD.csv')

plotData(tA, dataA)
