import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

exp_name = "../data/test/"
qty = "magnet"
also_load = "energy"

EqPt = 5000

directories = [f for f in os.listdir(exp_name) if f.startswith("Temp")]
directories = sorted(directories)

csvQty_list = []
csvEng_list = []
T = []

print("Loading data...")
for d in directories:
	path = exp_name + d + "/" + qty + ".csv"
	print("Reading " + path + " ...")
	data = pd.read_csv(path, header=None)
	csvQty_list.append(data)
	data = pd.read_csv(exp_name + d + "/" + also_load + ".csv", header=None)
	csvEng_list.append(data)

	T.append(float(d[4:]))
T = np.array(T)
N = len(directories)

print("Loading done")
print("Analysing...")
average = np.zeros(len(csvQty_list))
order1T = np.zeros(len(csvQty_list))
for i in range(N):
	dataQty = csvQty_list[i]
	dataEng = csvEng_list[i]
	# Ensemble average
	engAvg = dataEng.mean()
	proAvg = (dataQty * dataEng).mean()
	if qty == "magnet":
		qtyAvg = dataQty.abs().mean()
	else:
		qtyAvg = dataQty.mean()
	qtyAvg = qtyAvg[EqPt:]	# Equilibrium data
	engAvg = engAvg[EqPt:]	# Equilibrium data
	proAvg = proAvg[EqPt:]	# Equilibrium data
	# print(en_avg)
	average[i] = qtyAvg.mean()
	order1T[i] = (engAvg * qtyAvg).mean() - engAvg.mean() * qtyAvg.mean()

order1T = -T**2 * order1T

plt.plot(T, order1T)
plt.show()

avgData = pd.DataFrame(average)
o1tData = pd.DataFrame(order1T)

print("Writing...")
avgData.to_csv(exp_name + qty + ".csv", header=None, index=False)
o1tData.to_csv(exp_name + qty + "Dev.csv", header=None, index=False)

# Tc = 2 / np.arctanh( 1 / np.sqrt(2) )
# Tm = T[T < Tc]
# Tp = T[T > Tc]
# avgM = average[T < Tc]
# avgP = average[T > Tc]
# epsM = 1 - Tm/Tc
# epsP = Tp/Tc - 1

# plt.figure(1)
# plt.plot(np.log(epsM), avgM)
# plt.xlabel(r"$\ln \epsilon$")
# plt.ylabel(r"$\ln C$")
# plt.show()

"""
if qty == "energy":
	term = r"Specific heat, $C_V$"
else:
	term = r"Susceptibility, $\chi$"

plt.figure(1)
plt.plot(T, average)
plt.xlabel(r"Temperature, $T$")
plt.ylabel(f"{qty} per spin")
plt.grid()

plt.figure(2)
plt.plot(T, T**2 * deviate)
plt.xlabel(r"Temperature, $T$")
plt.ylabel(term)
plt.grid()

plt.show()
"""