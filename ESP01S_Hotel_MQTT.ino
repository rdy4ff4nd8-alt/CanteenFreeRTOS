/**
 * ESP01S_Hotel_MQTT - 酒店智能大厅人数显示系统 (MQTT 版) v1.6
 * 
 * v1.6 新增：
 *   - 每 30s 通过 UART 给 STM32 发 $HEARTBEAT# 心跳帧, STM32 收到后维持绿灯
 * v1.5 新增：
 *   - STM32 传感器上行：读 UART 收到的 $MQ2:...|...|...# 帧 → publish hotel/lobby/sensor
 * 
 * v1.1 修复：
 *   - 移除所有 Serial.printf 中的中文，避免 ESP8266 对齐异常 (Exception 9)
 *   - WiFi 等待循环加入 yield()，防止 WDT 复位
 *   - 修复 forwardToSTM32 重复发送帧的 bug
 *   - 改用 Serial.print + String 拼接，更稳定
 * 
 * 功能：WiFi连接 → MQTT订阅 → 接收JSON数据 → UART转发给STM32
 * 
 * 协议（ESP01S → STM32，UART 115200bps）：
 *   正常帧: $COUNT:42|PREDICT:45,43,41,40,38|THRESH:150|TIME:2026-08-03 14:30:25#
 *   错误帧: ERR#
 *   v1.4: TIME 字段改为 "YYYY-MM-DD HH:MM:SS"(含日期), 时区 +8 修正
 */

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ==================== WiFi 配置 ====================
const char* WIFI_SSID     = "Xiaomi 15 pro";
const char* WIFI_PASSWORD = "1111llll";

// ==================== MQTT Broker 配置 ====================
//直接连内网
const char* MQTT_SERVER = "172.20.10.3";
const int   MQTT_PORT   = 1883;

const char* MQTT_USER = "";
const char* MQTT_PASS = "";
const char* MQTT_CLIENT_ID = "esp01s_hotel_001";

const char* MQTT_TOPIC_SUB = "hotel/lobby/data";     // 订阅：服务器 → ESP
const char* MQTT_TOPIC_PUB = "hotel/lobby/online";   // 发布：ESP 上线心跳
const char* MQTT_TOPIC_SENSOR = "hotel/lobby/sensor"; // v1.5: STM32 传感器数据上行

// ==================== UART 配置 ====================
#define BAUD_RATE 115200

// ==================== 全局变量 ====================
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastReconnectAttempt = 0;
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 30000;  // 心跳 30s 一次

// ==================== 函数声明 ====================
void setupWiFi();
void reconnectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void forwardToSTM32(int nowCount, int* predArr, int predLen, int threshold, const char* timeStr);
void sendErrorFrame();
bool parseJsonAndForward(const char* payload, unsigned int length);

// ==================== setup ====================
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.setTimeout(100);
  delay(100);

  Serial.println();
  Serial.println("====================================");
  Serial.println("  ESP01S Hotel MQTT Bridge v1.6");
  Serial.println("====================================");

  // v1.3: 先连 WiFi（最小化内存占用），连上后再配 MQTT
  setupWiFi();

  // WiFi 连上后再配置 MQTT
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
  client.setKeepAlive(60);
  client.setBufferSize(512);
}

// ==================== loop ====================
void loop() {
  // ---- 1. WiFi 检查 ----
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WiFi] disconnected, retrying...");
    setupWiFi();
    delay(2000);
    return;
  }

  // ---- 2. MQTT 连接检查 ----
  if (!client.connected()) {
    reconnectMQTT();
    // v1.3: 关键！MQTT 未连时必须 delay+yield，否则空转饿死 WiFi 触发看门狗
    delay(2000);
    yield();
    return;
  }

  // ---- 3. MQTT 循环 ----
  client.loop();

  // ---- 3.5. STM32 传感器上行 (v1.5) ----
  while (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.startsWith("$") && line.indexOf('#') > 0) {
      client.publish(MQTT_TOPIC_SENSOR, line.c_str());
    }
  }

  // ---- 4. 心跳 (30s) ----
  unsigned long now = millis();
  if (now - lastHeartbeat >= HEARTBEAT_INTERVAL) {
    lastHeartbeat = now;
    char statusMsg[80];
    snprintf(statusMsg, sizeof(statusMsg),
             "{\"device\":\"esp01s\",\"online\":true,\"rssi\":%d}",
             WiFi.RSSI());
    client.publish(MQTT_TOPIC_PUB, statusMsg);
    Serial.print("[HEARTBEAT] published to ");
    Serial.println(MQTT_TOPIC_PUB);
    // v1.6: 给 STM32 发心跳帧, 用于维持绿灯
    Serial.println("$HEARTBEAT#");
  }

  delay(10);
  yield();
}

// ==================== WiFi 连接 ====================
void setupWiFi() {
  delay(10);
  Serial.print("[WiFi] Connecting to ");
  Serial.print(WIFI_SSID);
  Serial.println(" ...");

  // v1.3: 简化流程（最小测试证明简单代码更稳定）
  WiFi.mode(WIFI_STA);

  // v1.3: 降低 WiFi 发射功率（默认20.5dBm→10dBm），减少电流，防止电源崩溃
  WiFi.setOutputPower(10);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
    yield();
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("[WiFi] OK! IP: ");
    Serial.print(WiFi.localIP());
    Serial.print("  RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println();
    Serial.println("[WiFi] Connect FAILED");
  }
}

// ==================== MQTT 重连 ====================
void reconnectMQTT() {
  unsigned long now = millis();
  if (now - lastReconnectAttempt < 5000) return;
  lastReconnectAttempt = now;

  Serial.print("[MQTT] Connecting ");
  Serial.print(MQTT_SERVER);
  Serial.print(":");
  Serial.print(MQTT_PORT);
  Serial.println(" ...");

  bool connected;
  if (strlen(MQTT_USER) > 0) {
    connected = client.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS);
  } else {
    connected = client.connect(MQTT_CLIENT_ID);
  }

  if (connected) {
    Serial.println("[MQTT] Connected!");
    client.subscribe(MQTT_TOPIC_SUB);
    Serial.print("[MQTT] Subscribed: ");
    Serial.println(MQTT_TOPIC_SUB);

    char hello[80];
    snprintf(hello, sizeof(hello),
             "{\"device\":\"esp01s\",\"event\":\"online\",\"rssi\":%d}",
             WiFi.RSSI());
    client.publish(MQTT_TOPIC_PUB, hello);

    sendErrorFrame();
  } else {
    Serial.print("[MQTT] FAILED, rc=");
    Serial.println(client.state());
    Serial.println("  -4=timeout -3=lost -2=fail -1=busy");
    Serial.println("   1=proto 2=id 3=unavail 4=cred 5=auth");
  }
}

// ==================== MQTT 消息回调 ====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Msg [");
  Serial.print(topic);
  Serial.print("] len=");
  Serial.println(length);

  char* payloadStr = (char*)malloc(length + 1);
  if (payloadStr == NULL) {
    Serial.println("[MQTT] malloc failed");
    return;
  }
  memcpy(payloadStr, payload, length);
  payloadStr[length] = '\0';

  Serial.print("[MQTT] Data: ");
  Serial.println(payloadStr);

  parseJsonAndForward(payloadStr, length);

  free(payloadStr);
}

// ==================== JSON 解析 + 转发 STM32 ====================
bool parseJsonAndForward(const char* payload, unsigned int length) {
  StaticJsonDocument<512> doc;

  DeserializationError err = deserializeJson(doc, payload, length);
  if (err) {
    Serial.print("[JSON] Parse fail: ");
    Serial.println(err.c_str());
    sendErrorFrame();
    return false;
  }

  if (!doc.containsKey("now_count")) {
    Serial.println("[JSON] missing now_count");
    sendErrorFrame();
    return false;
  }
  int nowCount = doc["now_count"].as<int>();

  int predArr[5] = {0};
  int predLen = 0;
  if (doc.containsKey("predict_count")) {
    JsonArray arr = doc["predict_count"].as<JsonArray>();
    int sz = arr.size();
    predLen = (sz < 5) ? sz : 5;
    for (int i = 0; i < predLen; i++) {
      predArr[i] = arr[i].as<int>();
    }
  }

  int threshold = 150;
  if (doc.containsKey("alert_threshold")) {
    threshold = doc["alert_threshold"].as<int>();
  }

  // v1.4: 时间格式改为 "YYYY-MM-DD HH:MM:SS"(含日期), 时区 +8 修正(原 localtime 默认 UTC, 慢 8 小时)
  char timeStr[20] = "----/--/-- --:--:--";
  if (doc.containsKey("timestamp")) {
    time_t ts = doc["timestamp"].as<time_t>() + 8 * 3600;   // ★ +8 时区(北京时间)
    struct tm* t = localtime(&ts);
    snprintf(timeStr, sizeof(timeStr), "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
  }

  forwardToSTM32(nowCount, predArr, predLen, threshold, timeStr);
  return true;
}

// ==================== 发送协议帧给 STM32 ====================
void forwardToSTM32(int nowCount, int* predArr, int predLen, int threshold, const char* timeStr) {
  // v1.4: 帧缓冲加大到 160(TIME 字段从 8 字节扩展到 19 字节)
  char frame[160];
  int n = snprintf(frame, sizeof(frame),
    "$COUNT:%d|PREDICT:%d,%d,%d,%d,%d|THRESH:%d|TIME:%s#",
    nowCount,
    predArr[0], predArr[1], predArr[2], predArr[3], predArr[4],
    threshold, timeStr);

  Serial.print("[UART->STM32] ");
  Serial.println(frame);
  // frame 同时通过 TX 发给 STM32（ESP01S 只有一个 UART，Serial.println 即 TX 输出）
}

// ==================== 错误帧 ====================
void sendErrorFrame() {
  Serial.println("[ERR] send ERR#");
  Serial.println("ERR#");
}
