

import time
import os
import sys
import csv
import json
import shutil
import threading
from datetime import datetime, timedelta

sys.path.insert(0, os.path.dirname(__file__))

from flask import Flask, request, jsonify
import torch
import paho.mqtt.client as mqtt

from detect import count_people
from lstm.predict import predict_future_seq
from models.experimental import attempt_load

app = Flask(__name__, static_folder='.\\runs', static_url_path='/runs')

PREDICT_NUM     = 4             # 预测未来 4 个时刻
ALERT_THRESHOLD = 150
DOMAIN          = 'http://172.20.10.3:5000'
SAMPLE_MINUTES  = 2             # 与小程序拍照间隔对齐，用于生成 predict_time
MAX_HISTORY     = 30            # 保留最近 30 条历史 (2min×30 = 60min)

MQTT_BROKER     = '127.0.0.1'
MQTT_PORT       = 1883
MQTT_TOPIC      = 'hotel/lobby/data'
MQTT_TOPIC_SENSOR = 'hotel/lobby/sensor'

persons = []
detect_lock = threading.Lock()
sensor_lock = threading.Lock()
sensor_cache = {}

yolo_model = None
device = None
mqtt_client = None

# ─── 工具函数 ───
def _cleanup():
    """启动时清理旧的上传/结果图"""
    upload_img = '.\\data\\images\\receive.jpg'
    if os.path.exists(upload_img):
        try:
            os.remove(upload_img)
        except Exception as e:
            print(f'删除失败 {upload_img}: {e}')
    runs_dir = '.\\runs'
    if os.path.exists(runs_dir):
        try:
            shutil.rmtree(runs_dir)
            os.makedirs(runs_dir)
        except Exception as e:
            print(f'清空 runs 目录失败: {e}')

def _init_yolo():
    global yolo_model, device
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    yolo_model = attempt_load('yolov5s.pt', map_location=device)
    yolo_model.eval()
    print(f'[YOLO]   yolov5s.pt -> {device}')

def _parse_sensor_frame(frame):
    """$MQ2:512|FLAME:1|TEMP:265|HUMI:605# -> dict"""
    try:
        body = frame.strip('$#\n\r ')
        result = {}
        for part in body.split('|'):
            if ':' not in part:
                continue
            k, v = part.split(':', 1)
            result[k.lower()] = int(v)
        return result
    except Exception as e:
        print(f'[WARN] sensor parse: {e}')
        return {}

def _on_sensor_msg(client, userdata, msg):
    d = _parse_sensor_frame(msg.payload.decode(errors='replace'))
    if d:
        with sensor_lock:
            sensor_cache.update(d)

def _init_mqtt():
    global mqtt_client
    mqtt_client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2,
                              client_id='hotel_server_v2')
    mqtt_client.connect(MQTT_BROKER, MQTT_PORT, keepalive=60)
    mqtt_client.message_callback_add(MQTT_TOPIC_SENSOR, _on_sensor_msg)
    mqtt_client.subscribe(MQTT_TOPIC_SENSOR, qos=0)
    mqtt_client.loop_start()
    print(f'[MQTT]   {MQTT_BROKER}:{MQTT_PORT}  pub={MQTT_TOPIC}  sub={MQTT_TOPIC_SENSOR}')

def _append_csv(count_val):
    """每收到一张图，追加一行 时间,人数 到 lstm/collect.csv"""
    csv_path = os.path.join(os.path.dirname(__file__), 'lstm', 'collect.csv')
    try:
        need_header = not os.path.exists(csv_path)
        os.makedirs(os.path.dirname(csv_path), exist_ok=True)
        with open(csv_path, 'a', newline='', encoding='utf-8-sig') as f:
            writer = csv.writer(f)
            if need_header:
                writer.writerow(['时间', '人数'])
            writer.writerow([datetime.now().strftime('%Y-%m-%d %H:%M:%S'), count_val])
    except Exception as e:
        print(f'[WARN] CSV: {e}')

if os.environ.get('WERKZEUG_RUN_MAIN') == 'true':
    _cleanup()

# ─── HTTP 路由 ───
@app.route('/', methods=['GET'])
def hello():
    img_dir = '.\\data\\images'
    img_exts = ('.bmp', '.jpg', '.jpeg', '.png', '.tif', '.tiff', '.dng', '.webp', '.mpo')
    images = [f for f in os.listdir(img_dir) if f.lower().endswith(img_exts)] if os.path.isdir(img_dir) else []
    if not images:
        return "<h1 style='color:blue'>Hello! 服务已启动，请通过 POST 上传图片进行人数检测</h1>"
    return "<h1 style='color:blue'>Hello! 服务已启动，检测接口已就绪</h1>"

@app.route('/', methods=['POST'])
def process_request():
    with detect_lock:
        try:
            # 1. 保存上传图（用时间戳命名避免并发覆盖）
            if 'img' not in request.files:
                return jsonify({'error': 'no img field'}), 400
            file = request.files['img']
            if not file or file.filename == '':
                return jsonify({'error': 'empty image'}), 400

            now_ts = round(time.time(), 2)
            tmp_path = f'.\\data\\images\\receive_{now_ts}.jpg'
            result_path = f'.\\runs\\receive_{now_ts}.jpg'
            file.save(tmp_path)

            # 2. YOLOv5 数人 + 保存结果图
            results = count_people(tmp_path, yolo_model, device,
                                   imgsz=640, conf_thres=0.25, iou_thres=0.45,
                                   save_path=result_path)

            # 3. 更新历史
            persons.append([now_ts, results])
            while len(persons) > MAX_HISTORY:
                persons.pop(0)
            _append_csv(results)

            # 4. LSTM 预测未来 4 个时刻
            pred_list, pred_time_list = [], []
            history_data = [item[1] for item in persons]
            if len(history_data) >= 3:
                try:
                    raw_pred = predict_future_seq(history_data, pred_count=PREDICT_NUM)
                    pred_list = [max(0, int(round(p))) for p in raw_pred]
                    base_time = datetime.fromtimestamp(persons[-1][0])
                    pred_time_list = [round((base_time + timedelta(minutes=(i + 1) * SAMPLE_MINUTES)).timestamp(), 2)
                                      for i in range(len(pred_list))]
                except Exception as e:
                    print('LSTM预测失败:', e)

            # 5. 生成可访问的 result 图片 URL
            img_url = f'{DOMAIN}/runs/receive_{now_ts}.jpg?timestamp={now_ts}'
            img_files = sorted([f for f in os.listdir('.\\runs\\')
                                if f.startswith('receive_') and f.endswith('.jpg')])
            if len(img_files) > 20:
                for old_file in img_files[:-20]:
                    os.remove(os.path.join('.\\runs\\', old_file))

            # 6. MQTT 发布给 ESP01S
            try:
                mqtt_msg = json.dumps({
                    'now_count':       results,
                    'predict_count':   pred_list,
                    'alert_threshold': ALERT_THRESHOLD,
                    'timestamp':       int(time.time())
                })
                mqtt_client.publish(MQTT_TOPIC, mqtt_msg, qos=0)
                print(f'[MQTT] count={results} pred={pred_list}')
            except Exception as e:
                print(f'[ERR] MQTT publish: {e}')

            # 7. 读最新传感器数据
            with sensor_lock:
                sensor = dict(sensor_cache)

            # 8. 返回小程序
            return jsonify({
                'img_url':         img_url,
                'now_count':       results,
                'alert_threshold': ALERT_THRESHOLD,
                'real_time':       [x[0] for x in persons],
                'real_count':      [x[1] for x in persons],
                'predict_time':    pred_time_list,
                'predict_count':   pred_list,
                'sensor': {
                    'mq2':   sensor.get('mq2',   0),
                    'flame': sensor.get('flame', 0),
                    'temp':  sensor.get('temp',  0),
                    'humi':  sensor.get('humi',  0)
                }
            })
        finally:
            # 清理本次临时上传图
            try:
                if os.path.exists(tmp_path):
                    os.remove(tmp_path)
            except Exception:
                pass

@app.route('/health', methods=['GET'])
def health():
    with sensor_lock:
        sensor = dict(sensor_cache)
    return jsonify({
        'status':      'ok',
        'history_len': len(persons),
        'sensor':      sensor
    })

if __name__ == '__main__':
    # ---- 自动清理旧实例 ----
    import socket, subprocess
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    if s.connect_ex(('127.0.0.1', 5000)) == 0:
        s.close()
        print('[BOOT] 检测到旧实例，正在关闭...')
        subprocess.run('taskkill /F /IM python.exe', shell=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        time.sleep(1.5)
    else:
        s.close()

    print('=' * 50)
    print(' Hotel Lobby Detection Server v2.0')
    print(' TestDetectionImg.py + YOLO + LSTM + MQTT + Sensor')
    print('=' * 50)
    _init_yolo()
    _init_mqtt()
    app.run(host='0.0.0.0', port=5000, debug=False)
