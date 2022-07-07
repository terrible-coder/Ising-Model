import pandas as pd
import matplotlib.pyplot as plt

Data = pd.read_csv("energy.tsv", delimiter="\t", header=None)
Data = Data.transpose()

print(Data)

ROWS = len(Data.axes[0])
COLS = len(Data.axes[1])

T = 0.01
temp = []
Cv = []
avgE = []

for i in range(COLS):
	temp.append(T)
	cv = Data[i].std()**2 / T**2
	Cv.append(cv)
	T += 0.02

plt.figure()
for i in range(COLS):
	avgE.append(Data[i].mean())
plt.plot(temp, avgE)
plt.xlabel(r"$T$")
plt.ylabel(r"$U$")

plt.figure()
plt.plot(temp, Cv)

plt.xlabel(r"$T$")
plt.ylabel(r"$C_V$")

# plt.plot(magnetisation)
# plt.xlabel("MCS step")
# plt.ylabel("Magnetisation")
plt.show()
