import pandas as pd
import matplotlib.pyplot as plt

Data = pd.read_csv("energy.tsv", delimiter="\t", header=None)
Data = Data.transpose()

print(Data)

T = 0.01
Cv = []
temp = []

for i in range(16):
	temp.append(T)
	cv = Data[i].std()**2 / T**2
	Cv.append(cv)
	T += 0.02

plt.plot(temp, Cv)

plt.xlabel(r"$T$")
plt.ylabel(r"$C_V$")

# plt.plot(magnetisation)
# plt.xlabel("MCS step")
# plt.ylabel("Magnetisation")
plt.show()
