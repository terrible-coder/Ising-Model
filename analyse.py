import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

Data = pd.read_csv("energy.tsv", delimiter="\t", header=None)
Data = Data.transpose()

print(Data)

ROWS = len(Data.axes[0])
COLS = len(Data.axes[1])

# T = 1.0
T  = np.arange(1.0, 4.01, 0.01)
U  = np.array([Data[i].mean() for i in range(COLS)])
U2 = np.array([(Data[i]**2).mean() for i in range(COLS)])
# varE = np.array([Data[i].std() for i in range(COLS)])
varE = U2 - U**2
Cv = varE / ((50*50) * T**2)

# for i in range(COLS):
# 	temp.append(T)
# 	avgE = Data[i].mean()
# 	E_sq = Data[i] * Data[i]
# 	E_sq_avg = E_sq.mean()
# 	varE = Data[i].std() ** 2
# 	U.append(avgE)
# 	cv = varE / T**2
# 	Cv.append(cv)
# 	T += 0.01

z = np.polyfit(T, U, 20)
p = np.poly1d(z)
U_fit = p(T)

# z = np.polyfit(T, U2, 10)
# p = np.poly1d(z)
# U2_fit = p(T)
# varE_fit = U2_fit - U_fit**2
# Cv_fit = varE_fit / T**2
# Cv_fit = np.poly1d(np.polyfit(T, Cv, 5))(T)
Cv_fit = p.deriv()(T)

plt.figure()
plt.plot(T, U)
plt.plot(T, U_fit)
plt.xlabel(r"$T$")
plt.ylabel(r"$U$")

plt.figure()
plt.plot(T, Cv)
# plt.plot(T, Cv_fit)
plt.xlabel(r"$T$")
plt.ylabel(r"$C_V$")

# plt.plot(magnetisation)
# plt.xlabel("MCS step")
# plt.ylabel("Magnetisation")
plt.show()
