import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

def fitting(x, y, N):
	return np.poly1d(np.polyfit(x, y, N))(x)

EnergyData = pd.read_csv("energy.tsv", delimiter="\t", header=None)
EnergyData = EnergyData.transpose()
MagnetData = pd.read_csv("magnet.tsv", delimiter="\t", header=None)
MagnetData = MagnetData.transpose()


ROWS = len(EnergyData.axes[0])
COLS = len(EnergyData.axes[1])

# T = 1.0
T  = np.arange(1.0, 4.01, 0.01)

print("Energy values")
print(EnergyData)
# print(EnergyData[0]**2)
# print("\n", EnergyData[0][0])

U  = np.array([EnergyData[i].mean() for i in range(COLS)])
U2 = np.array([(EnergyData[i]**2).mean() for i in range(COLS)])
# varE = np.array([Data[i].std() for i in range(COLS)])
varE = U2 - U**2
Cv = varE / (T**2)

U_fit  = fitting(T, U, 20)
U2_fit = fitting(T, U2, 20)

varE_fit = U2_fit - U_fit**2
Cv_fit = varE_fit / T**2
# Cv_fit = fitting(T, Cv, 20)

plt.figure()
plt.plot(T, U, "r.")
# plt.plot(T, U_fit)
plt.xlabel(r"$T$")
plt.ylabel(r"$<E>$")
plt.grid()

plt.figure()
plt.plot(T, U2, "r.")
# plt.plot(T, U2_fit)
plt.xlabel(r"$T$")
plt.ylabel(r"$<E^2>$")
plt.grid()

plt.figure()
plt.plot(T, Cv, "r.")
# plt.plot(T, Cv_fit)
plt.xlabel(r"$T$")
plt.ylabel(r"$C_V$")
plt.grid()

# print("Magnetisation values")
# print(MagnetData)

M  = np.array([MagnetData[i].mean() for i in range(COLS)])
M2 = np.array([(MagnetData[i]**2).mean() for i in range(COLS)])
# varE = np.array([Data[i].std() for i in range(COLS)])
varM = M2 - M**2
Chi = varM / (T)

M_fit = fitting(T, M, 20)

plt.figure()
plt.plot(T, M, "r.")
plt.xlabel(r"$T$")
plt.ylabel(r"$<M>$")
plt.grid()

plt.figure()
plt.plot(T, M2, "r.")
plt.xlabel(r"$T$")
plt.ylabel(r"$<M^2>$")
plt.grid()

plt.figure()
plt.plot(T, Chi, "r.")
plt.xlabel(r"$T$")
plt.ylabel(r"$\chi$")
plt.grid()

plt.show()
