import numpy as np
from scipy import optimize as opt
import pandas as pd
import matplotlib.pyplot as plt

begIdx = 0
endIdx = 15000
dataIdxRange = [i for i in range(begIdx, endIdx)]
EVERY = 50
SKIP = 5

def readData(temperature:float, size:int, ensemble:int, qty):
	datafile = f"../data/exchange{size}x{size}en{ensemble}/Temp{temperature}00000/{qty}.csv"
	data:pd.DataFrame = pd.read_csv(datafile, header=None, usecols=dataIdxRange)
	return data

def calcMean(data:pd.DataFrame):
	mean_data = data.mean()
	mean = mean_data.mean()
	stDv = mean_data.std()
	return mean_data, mean, stDv

def plotMeans(data, mean, QtyPerSpin):
	for i in range(SKIP-1, ROWS, SKIP):
		plt.plot(data[i], "*", markevery=EVERY, label=f"En {i+1}")
	plt.plot(mean, "k-")
	plt.grid()
	plt.title(f"T={T}, Size={sz}")
	plt.xlabel("MC steps")
	plt.ylabel(QtyPerSpin + " per spin")
	plt.legend()

def vary(p, n):
	return p[0] / np.sqrt(n) + p[1]

def error(p, n, y):
	return vary(p, n) - y

# sizeList = np.array([32, 48, 64, 80, 96, 112])
sizeList = np.array([64, 128])
stEList  = []
stMList  = []

T  = float(1.0)
EN = 25

for sz in sizeList:
	eData = readData(T, sz, EN, "energy")
	# mData = readData(T, sz, EN, "magnet").abs()

	ROWS = len(eData.axes[0])
	COLS = len(eData.axes[1])

	# ROWS = len(mData.axes[0])
	# COLS = len(mData.axes[1])
	print(ROWS, COLS)

	meanE, U, stU = calcMean(eData)
	stEList.append(stU)
	print(stU)
	# meanM, M, stM = calcMean(mData)
	# stMList.append(stM)
	# print(stM)

	eData = eData.transpose()
	# mData = mData.transpose()

	plt.figure()
	plotMeans(eData, meanE, "Energy")
	print("<E> = ", U, end="\t")
	print(r"$\Delta E$ = ", stU)

	# plt.figure()
	# plotMeans(mData, meanM, "Magnetisation")
	# print("<M> = ", M, end="\t")
	# print(r"\Delta M = ", stM)

stEList = np.array(stEList)

plt.figure()
plt.plot(sizeList, stEList, "rx")
p0 = [1, 1]
p_fit = opt.least_squares(error, p0[:], args=(sizeList, stEList)).x
print(p_fit)

# stMList = np.array(stMList)

# plt.figure()
# plt.plot(sizeList, stMList, "rx")
# p0 = [1, 1]
# p_fit = opt.least_squares(error, p0[:], args=(sizeList, stMList)).x
# print(p_fit)

N = np.arange(24, 128, 0.1)
plt.plot(N, vary(p_fit, N))
plt.grid()
plt.xlabel("Size of lattice")
plt.ylabel(r"$\frac{\Delta E}{E}$")
plt.title(f"T = {T}")

plt.show()
