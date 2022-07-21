import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

T  = 2.3
SZ = 40
EN = 20
filename = f"data{SZ}x{SZ}en{EN}/energy{T}00000.csv"

data = pd.read_csv(filename, header=None) / (64**2)

ROWS = len(data.axes[0])
COLS = len(data.axes[1])
print(ROWS, COLS)
print(data)

data0 = data[0]
data0m = data0.mean()
data0s = data0.std()

data_mean = data.mean()
data_std = data.std()

data = data.transpose()
data_mean = data_mean.transpose()
data_std = data_std.transpose()

every = 100

plt.figure()
for i in range(ROWS-1):
	plt.plot(data[i], "--", markevery=every)
plt.plot(data_mean, "k-", markevery=every)

plt.grid()
plt.title(f"T={T}, Size={SZ}")
plt.xlabel("MC steps")
plt.ylabel("Energy per spin")

plt.figure()
plt.plot(data_std, markevery=every)

plt.grid()
plt.title(f"T={T}, Size={SZ}")
plt.xlabel("MC steps")
plt.ylabel("SD of energy")

plt.show()
