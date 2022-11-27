import os
import pandas as pd

exp_path = "../data/BinMixSD/"

directories = os.listdir(exp_path)
directories = sorted(directories)

for d in directories:
	path = exp_path + d + "/"
	print("doing " + d)

	file_list = os.listdir(path)
	file_list = [f for f in file_list if f.startswith("energy") and f!="energy.csv"]
	file_list = sorted(file_list)
	file_list = [path + f for f in file_list]

	csv_list = [pd.read_csv(f, header=None) for f in file_list]
	csv_merged = pd.concat(csv_list, ignore_index=True)
	csv_merged.to_csv(path+"energy.csv", header=False, index=False)
