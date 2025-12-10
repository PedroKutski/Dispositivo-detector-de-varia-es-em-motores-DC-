# 📊 IoT Motor Control & Vibration Analysis System

Este repositório contém o firmware para um sistema modular de instrumentação e controle remoto. O projeto integra análise de vibração em tempo real (FFT) com controle de atuadores via protocolo MQTT, ideal para monitoramento de bancadas de teste, motores DC ou sistemas robóticos.

## 📋 Arquitetura do Sistema

O sistema opera com dois módulos independentes:

### 1. Módulo de Análise Espectral (Arduino Nano)
Focado em telemetria e diagnóstico mecânico.
- **Análise de Vibração:** Executa FFT (Fast Fourier Transform) para identificar a frequência fundamental de vibração e converter para RPM.
- **Instrumentação:** Monitora temperatura e inclinação (Pitch/Roll) via MPU6050.
- **Interface Local:** Exibe dados em tempo real em um display IPS ST7789 com alertas visuais baseados em limites de amplitude configuráveis (calibração).
- **Comunicação Serial:** Envia pacote de dados estruturados para integração externa.

### 2. Módulo de Controle Remoto (ESP32)
Focado em conectividade e atuação de potência.
- **Controle PWM:** Gera sinais PWM de 8 bits a 5kHz para controle de velocidade de motores.
- **Acionamento Digital:** Controle de pinos para habilitação de drivers (Enable/Direction).
- **Conectividade IoT:** Cliente MQTT (PubSubClient) para recepção de comandos remotos via WiFi.

---

## ⚙️ Hardware

### Módulo de Análise
* **MCU:** Arduino Nano
* **Sensor IMU:** MPU6050
* **Display:** TFT ST7789 (170x320)
* **Interface:** Push Button (para rotina de autocalibração)

### Módulo de Controle
* **MCU:** ESP32 (DevKit V1 ou similar)
* **Atuadores:** Drivers de motor ou relés compatíveis com 3.3V
* **Conexão:** WiFi 2.4GHz

---

## 🔌 Mapeamento de Pinos (Pinout)

### Arduino (Sensor/Display)
| Pino | Função | Observação |
| :--- | :--- | :--- |
| **D10** | TFT CS | Chip Select |
| **D9** | TFT RST | Reset |
| **D8** | TFT DC | Data/Command |
| **D2** | Botão | Trigger de Calibração |
| **A4/A5**| I2C | MPU6050 (SDA/SCL) |

### ESP32 (Atuação)
| GPIO | Função | Descrição |
| :--- | :--- | :--- |
| **4** | Saída Digital 1 | Tópico `.../dt` |
| **5** | Saída Digital 2 | Tópico `.../et` |
| **18** | PWM Canal 1 | Tópico `.../pwm1` |
| **19** | PWM Canal 2 | Tópico `.../pwm2` |
| **2** | LED Status | Feedback visual MQTT |

---

## 📡 Protocolo MQTT

O dispositivo subscreve aos tópicos abaixo para controle remoto (Broker padrão: `broker.hivemq.com`).

> **Nota:** Os tópicos no código utilizam o prefixo `hover/`, mas podem ser alterados no firmware conforme a aplicação.

| Tópico | Payload | Ação |
| :--- | :--- | :--- |
| `hover/pwm1` | `0` - `255` | Define o Duty Cycle do Canal 1. |
| `hover/pwm2` | `0` - `255` | Define o Duty Cycle do Canal 2. |
| `hover/dt` | `"on"` / `"off"` | Ativa/Desativa saída digital no GPIO 4. |
| `hover/et` | `"on"` / `"off"` | Ativa/Desativa saída digital no GPIO 5. |
| `hover/led` | `"TEST"` | Executa rotina de teste do LED. |

---

## 🖥️ Funcionalidades de Interface

### Autocalibração Inteligente
O sistema possui uma rotina de duas etapas para definir o "zero" da vibração:
1. **Leitura Ambiental (20s):** Mede o ruído do sensor com a máquina parada.
2. **Leitura Operacional (20s):** Mede a vibração natural da máquina em funcionamento.
3. **Definição de Threshold:** Calcula automaticamente o limite para alertas visuais (Vermelho/Verde na interface).

### Protocolo Serial (Arduino -> Externo)
O Arduino envia dados na Serial a 115200 baud no formato:
`<Frequência, RPM, Amplitude(g), Temperatura, Pitch, Roll, Alerta>`

---

## 🛠️ Requisitos de Instalação

As seguintes bibliotecas são necessárias para compilar o projeto:

1. **Adafruit MPU6050**
2. **Adafruit ST7789** & **Adafruit GFX**
3. **arduinoFFT** (utilizando templates C++)
4. **PubSubClient** (para o ESP32)

---

## ⚠️ Notas Técnicas
* **Compatibilidade ESP32:** O código utiliza a API `ledcAttach` do **ESP32 Core 3.0+**. Para versões anteriores do core board manager, é necessário adaptar para `ledcSetup`.
* **Credenciais:** Configure as variáveis `ssid` e `password` no arquivo `.ino` antes de gravar.

---
**Status do Projeto:** Funcional / Em desenvolvimento.
