#include <WiFi.h>
#include <PubSubClient.h>

// Configurações de Wi-Fi
const char* ssid = "INTELBRAS - JOAO";
const char* password = "jps510123";

// Configurações do broker HiveMQ
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// --- TÓPICOS MQTT ---
const char* topic_sub  = "hover/led";   // LED (On/Off)
const char* topic_dt   = "hover/dt";    // Pino DT (Digital)
const char* topic_et   = "hover/et";    // Pino ET (Digital)
const char* topic_pwm1 = "hover/pwm1";  // PWM 1 (0-255)
const char* topic_pwm2 = "hover/pwm2";  // PWM 2 (0-255)

// --- DEFINIÇÃO DOS PINOS ---
const int pinDT = 4;
const int pinET = 5;
const int pinPWM1 = 18; // PWM Esquerdo (exemplo)
const int pinPWM2 = 19; // PWM Direito (exemplo)

// --- CONFIGURAÇÃO PWM ---
const int freq = 5000;      // Frequência de 5 KHz
const int resolution = 8;   // Resolução de 8 bits (0-255)

// Objetos WiFi e MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Função chamada quando chega mensagem MQTT
void callback(char* topic, byte* message, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)message[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(msg);

  String topicoStr = String(topic);

  // --- CONTROLE LED EXISTENTE ---
  if (topicoStr == topic_sub && msg == "TEST") {
    Serial.println("Piscar LED 5 vezes");
    for (int i = 0; i < 5; i++) {
      digitalWrite(2, HIGH); delay(100);
      digitalWrite(2, LOW); delay(100);
    }
  }

  // --- CONTROLE DT (Digital) ---
  if (topicoStr == topic_dt) {
    if (msg == "on") digitalWrite(pinDT, HIGH);
    else if (msg == "off") digitalWrite(pinDT, LOW);
  }

  // --- CONTROLE ET (Digital) ---
  if (topicoStr == topic_et) {
    if (msg == "on") digitalWrite(pinET, HIGH);
    else if (msg == "off") digitalWrite(pinET, LOW);
  }

  // --- CONTROLE PWM 1 ---
  if (topicoStr == topic_pwm1) {
    int pwmValue = msg.toInt(); 
    
    // Trava os valores entre 0 e 255
    if (pwmValue < 0) pwmValue = 0;
    if (pwmValue > 255) pwmValue = 255;

    // ATENÇÃO: Na versão 3.0+, escrevemos direto no PINO
    ledcWrite(pinPWM1, pwmValue);
    
    Serial.print("PWM1 (Pino ");
    Serial.print(pinPWM1);
    Serial.print(") ajustado para: ");
    Serial.println(pwmValue);
  }

  // --- CONTROLE PWM 2 ---
  if (topicoStr == topic_pwm2) {
    int pwmValue = msg.toInt();
    
    if (pwmValue < 0) pwmValue = 0;
    if (pwmValue > 255) pwmValue = 255;

    // ATENÇÃO: Na versão 3.0+, escrevemos direto no PINO
    ledcWrite(pinPWM2, pwmValue);
    
    Serial.print("PWM2 (Pino ");
    Serial.print(pinPWM2);
    Serial.print(") ajustado para: ");
    Serial.println(pwmValue);
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado - IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    String clientId = "hover-2025-" + String(random(1000, 9999));
    
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe(topic_sub);
      client.subscribe(topic_dt);
      client.subscribe(topic_et);
      client.subscribe(topic_pwm1);
      client.subscribe(topic_pwm2);
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" Tentando em 5s");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(2, OUTPUT);       
  pinMode(pinDT, OUTPUT);   
  pinMode(pinET, OUTPUT);   
  
  // --- CONFIGURAÇÃO DO PWM (VERSÃO ESP32 3.0+) ---
  // A função ledcAttach agora faz tudo (configura e anexa)
  // Sintaxe: ledcAttach(pino, frequencia, resolucao)
  ledcAttach(pinPWM1, freq, resolution);
  ledcAttach(pinPWM2, freq, resolution);

  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setKeepAlive(60);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
