# load_data.py
import numpy as np
import torch
from torch.utils.data import Dataset, DataLoader, random_split
from sklearn.preprocessing import MinMaxScaler
import pandas as pd

class TimeSeriesDataset(Dataset):
    def __init__(self, X, y):
        self.X = torch.tensor(X, dtype=torch.float32)  # (样本数, look_back, 1)
        self.y = torch.tensor(y, dtype=torch.float32)  # (样本数, predict_steps)

    def __len__(self):
        return len(self.X)

    def __getitem__(self, idx):
        return {'X': self.X[idx], 'Y': self.y[idx]}

def create_sequences(data, look_back=10, predict_steps=5):
    X, y = [], []
    for i in range(len(data) - look_back - predict_steps + 1):
        X.append(data[i:i+look_back])
        y.append(data[i+look_back:i+look_back+predict_steps])
    return np.array(X), np.array(y)

def load_time_series_data(csv_file=None, look_back=10, predict_steps=5, batch_size=16):
    if csv_file:
        df = pd.read_csv(csv_file, encoding='utf-8-sig')
        # 使用中文列名
        if '每日最高峰时人数' not in df.columns:
            raise ValueError("CSV 文件中必须包含 '每日最高峰时人数' 列")
        data = df['每日最高峰时人数'].values.astype(np.float32)
    else:
        # 模拟数据（正弦波+趋势）
        data = np.sin(np.linspace(0, 20, 200)) * 10 + 50
        data = data.astype(np.float32)

    scaler = MinMaxScaler()
    scaled_data = scaler.fit_transform(data.reshape(-1, 1)).flatten()

    X, y = create_sequences(scaled_data, look_back, predict_steps)
    X = X.reshape(-1, look_back, 1)

    dataset = TimeSeriesDataset(X, y)
    train_size = int(0.8 * len(dataset))
    test_size = len(dataset) - train_size
    train_dataset, test_dataset = random_split(dataset, [train_size, test_size])

    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(test_dataset, batch_size=batch_size, shuffle=False)
    return train_loader, test_loader, scaler, 1  # input_size=1
