# predict.py
import os
import torch
import numpy as np
from .LSTMmodel import LSTMModel   # 注意：类名是 LSTMModel（两个 M 大写）
import joblib

BASE_DIR = os.path.dirname(__file__)          # 当前 predict.py 所在目录
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

# 模型文件路径（基于当前文件位置）
model_path = os.path.join(BASE_DIR, 'people_lstm.pth')
scaler_path = os.path.join(BASE_DIR, 'people_scaler.save')

model = None
scaler = None

if os.path.exists(model_path) and os.path.exists(scaler_path):
    model = LSTMModel(input_size=1, hidden_size=32, output_size=5, num_layers=2).to(device)
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    scaler = joblib.load(scaler_path)
    print("LSTM 模型加载成功")
else:
    print("警告：未找到 people_lstm.pth 或 people_scaler.save，预测将使用线性外推。")
    print("请先运行 train.py 生成模型文件。")

def predict_future_seq(history_num_list, pred_count=5, look_back=10):
    if model is None or scaler is None or len(history_num_list) < look_back:
        return fallback_predict(history_num_list, pred_count)

    recent = history_num_list[-look_back:]
    scaled = scaler.transform(np.array(recent).reshape(-1, 1)).flatten()
    input_tensor = torch.tensor(scaled, dtype=torch.float32).view(1, look_back, 1).to(device)

    with torch.no_grad():
        pred_scaled = model(input_tensor).cpu().numpy()[0]
    pred_original = scaler.inverse_transform(pred_scaled.reshape(-1, 1)).flatten()
    return [int(round(p)) for p in pred_original][:pred_count]

def fallback_predict(history, steps):
    if len(history) < 2:
        return [history[-1]] * steps
    diff = history[-1] - history[-2]
    return [history[-1] + diff * (i+1) for i in range(steps)]