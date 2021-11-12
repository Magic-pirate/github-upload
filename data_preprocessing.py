import numpy as np
import pandas as pd
from data_tool import data_preprocessing_class as dpc
import xlrd

data = pd.read_csv("water.csv")
new_data, n_data = dpc.miss_treat(data)
new_data.to_csv("miss_treat_data.csv")
new_data = dpc.drop_noisy(new_data)
new_data.to_csv("drop_noisy_data.csv")
n_data = dpc.data_normalization(n_data, 1, 0)
n_data.to_csv("normalization_data.csv")
