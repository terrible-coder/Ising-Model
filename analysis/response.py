import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

begIdx = 5000
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

SZ = 64
EN = 25
q = 4
temperature = np.arange(1.0, 5.0, 0.2)
Tc = q / np.log(1 + np.sqrt(2))

avgE = []
Cv = []
tempStr = [
	"1.0", "1.2", "1.4", "1.6", "1.8",
	"2.0", "2.2", "2.4", "2.6", "2.8",
	"3.0", "3.2", "3.4", "3.6", "3.8",
	"4.0", "4.2", "4.4", "4.6", "4.8"
]
""",
	"5.0", "5.2", "5.4", "5.6", "5.8",
	"6.0", "6.2", "6.4", "6.6", "6.8",
]"""
print(Tc)

for t in tempStr:
	eData = readData(float(t), SZ, EN, "energy")
	meanE, U, stU = calcMean(eData)
	avgE.append(U)
	Cv.append(stU)
avgE = np.array(avgE)

plt.figure()
plt.plot(temperature, avgE, "rx")
plt.axvline(x=Tc, linestyle="--")
plt.grid()
plt.xlabel(r"Temperature (in $\frac{k_BT}{J}$)")
plt.ylabel(r"Average energy per spin $\frac{<E>}{N}$")
plt.title(f"{SZ}x{SZ} lattice, Ensemble size={EN}")

plt.figure()
plt.plot(temperature, Cv, "rx")
plt.axvline(x=Tc, linestyle="--")
plt.grid()
plt.xlabel(r"Temperature (in $\frac{k_BT}{J}$)")
plt.ylabel("Heat Capacity")
plt.title(f"{SZ}x{SZ} lattice, Ensemble size={EN}")

plt.show()
