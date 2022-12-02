import os
import numpy as np
import scipy.optimize as opt
import matplotlib.pyplot as plt
import pandas as pd

def stLine(x, a, b):
	return a * x + b

exp_name = "../data/test/"
qty = "magnet"

EN = 12
N = 64

directories = [f for f in os.listdir(exp_name) if f.startswith("Temp")]
directories = sorted(directories)

T = []
for d in directories:
	T.append(float(d[4:]))
T = np.array(T)

average = np.array(pd.read_csv(exp_name+qty+".csv", header=None))
order1T = np.array(pd.read_csv(exp_name+qty+"Dev.csv", header=None))

average = (average.T)[0]
order1T = (order1T.T)[0]

# Tc = 2 / np.arctanh( 1 / np.sqrt(2) )
# # Tc = Tc / 2
# print("Critical temperature:", Tc)

plt.figure(1)
plt.plot(T, average)
# plt.axvline(x=Tc, color="k", linestyle="--")
plt.axvline(x=2.4, color="g", linestyle="--")
plt.grid()
plt.xlabel(r"Temperature in $\frac{k_BT}{J}$")
plt.ylabel(f"{qty} per spin")
plt.title(f"lattice size: {N}x{N}, Ensemble size: {EN}")

if qty == "energy":
	term = r"$C_V$"
else:
	term = r"$\chi$"
plt.figure(2)
plt.plot(T, order1T)
# plt.axvline(x=Tc, color="k", linestyle="--")
plt.axvline(x=2.4, color="g", linestyle="--")
plt.grid()
plt.xlabel(r"Temperature in $\frac{k_BT}{J}$")
plt.ylabel(term)
plt.title(f"lattice size: {N}x{N}, Ensemble size: {EN}")

plt.show()

"""
Tm = T[T < Tc]
Tp = T[T > Tc]
od1M = order1T[T < Tc]
od1P = order1T[T > Tc]
epsM = 1 - Tm/Tc
epsP = Tp/Tc - 1

od1M = od1M[:5]
od1P = od1P[:5]
epsM = epsM[:5]
epsP = epsP[:5]

xData = np.log(epsM)
yData = np.log(od1M)
print(len(xData))

popt, pcov = opt.curve_fit(stLine, xData, yData)
a, b = popt
print(popt)

plt.plot(epsM, od1M)
# plt.plot(epsP, od1P)
plt.plot(epsM, np.exp(b) * epsM ** a)
plt.grid()
plt.show()
"""