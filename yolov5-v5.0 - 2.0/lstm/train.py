# train.py
import torch
import torch.nn as nn
import torch.optim as optim
from load_data import load_time_series_data
from LSTMmodel import LSTMModel
import joblib

def main():
    look_back = 10       # 用过去10天的高峰
    predict_steps = 5    # 预测未来5天的高峰
    hidden_size = 32
    num_layers = 2
    learning_rate = 0.001
    epochs = 1500
    csv_file = 'original.csv'   # 你的CSV文件

    train_loader, test_loader, scaler, input_size = load_time_series_data(
        csv_file=csv_file,
        look_back=look_back,
        predict_steps=predict_steps,
        batch_size=8
    )

    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    model = LSTMModel(input_size=1, hidden_size=hidden_size,
                      output_size=predict_steps, num_layers=num_layers).to(device)
    criterion = nn.MSELoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)

    model.train()
    for epoch in range(epochs):
        total_loss = 0
        for batch in train_loader:
            x = batch['X'].to(device)  # (batch, look_back, 1)
            y = batch['Y'].to(device)  # (batch, predict_steps)

            optimizer.zero_grad()
            outputs = model(x)
            loss = criterion(outputs, y)
            loss.backward()
            optimizer.step()
            total_loss += loss.item()

        if (epoch+1) % 20 == 0:
            print(f'Epoch [{epoch+1}/{epochs}], Loss: {total_loss/len(train_loader):.4f}')

    torch.save(model.state_dict(), 'people_lstm.pth')
    joblib.dump(scaler, 'people_scaler.save')
    print("模型和归一化器已保存。")

if __name__ == '__main__':
    main()