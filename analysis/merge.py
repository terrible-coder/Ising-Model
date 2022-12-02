import os
import pandas as pd

exp_path = "../data/Para2Ferro/"
qty = "magnet"
outF = qty + ".csv"

directories = os.listdir(exp_path)
directories = sorted(directories)

for d in directories:
	path = exp_path + d + "/"
	print("doing " + d)

	file_list = os.listdir(path)
	file_list = [f for f in file_list if f.startswith(qty) and f!=outF]
	file_list = sorted(file_list)
	file_list = [path + f for f in file_list]

	csv_list = [pd.read_csv(f, header=None) for f in file_list]
	csv_merged = pd.concat(csv_list, ignore_index=True)
	csv_merged.to_csv(path+outF, header=False, index=False)

