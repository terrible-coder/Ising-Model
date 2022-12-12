import os
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

exp_name = "../data/BinMixSD/"
T = 2.0
N = 64
qty = "energy"

EVERY = 50
SKIP = 3

directory = exp_name + "Temp" + str(T) + "00000/"
path = directory + qty + ".csv"

data = pd.read_csv(path, header=None)
EN_SIZE = len(data)
mean = data.mean()
data = data.T

for i in range(0, EN_SIZE, SKIP):
	plt.plot(data[i], "*", alpha=0.8, label="En "+str(i+1), markevery=EVERY)
plt.plot(mean, "k-", linewidth=1, label="Ensemble average")
plt.grid()
plt.legend()
plt.xlabel("MC steps")
plt.ylabel(qty + " per spin")
plt.title(f"T = {T}, Size = {N}x{N}")
plt.show()
