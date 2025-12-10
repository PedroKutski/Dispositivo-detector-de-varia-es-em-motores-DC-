# 🛠️ Hoverboard IoT Controller & Vibration Monitor

Este repositório contém o código-fonte para um sistema híbrido de controle e monitoramento de motores (focado em projetos tipo Hoverboard/Robótica). O sistema é dividido em dois módulos principais: um **Módulo de Monitoramento Local** (Arduino + MPU6050 + Display) e um **Módulo de Controle Remoto** (ESP32 + MQTT).

## 📋 Visão Geral do Sistema

O projeto visa oferecer controle preciso via WiFi e diagnósticos mecânicos em tempo real através de análise de vibração.

### 1. Módulo de Monitoramento (Arduino Nano)
Responsável pela análise física do sistema.
- **Análise de Vibração (FFT):** Utiliza transformada rápida de Fourier para detectar frequências de vibração e calcular RPM.
- **Display de Dados:** Interface gráfica em display ST7789 exibindo RPM, Temperatura, Inclinação (Pitch/Roll) e alertas visuais.
- **Calibração Automática:** Rotina inteligente que mede o ruído ambiente e a vibração do motor para definir limiares de alerta.

### 2. Módulo de Controle (ESP32)
Responsável pela conectividade e atuação.
- **Conectividade IoT:** Conexão via WiFi e protocolo MQTT (Broker HiveMQ).
- **Controle de Motores:** Saídas PWM de alta resolução (8 bits, 5kHz) para controle de velocidade.
- **Acionamento Digital:** Controle de pinos digitais (DT/ET) para habilitação de drivers.

---

## ⚙️ Hardware Necessário

### Módulo de Monitoramento
* **Microcontrolador:** Arduino Nano (ou compatível)
* **Sensor:** MPU6050 (Acelerômetro e Giroscópio)
* **Display:** TFT IPS ST7789 (240x320 ou 170x320)
* **Botão:** Push button (para calibração)

### Módulo de Controle
* **Microcontrolador:** ESP32
* **Conexão:** Drivers de Motor / Ponte H compatíveis com lógica 3.3V

---

## 🔌 Pinagem (Pinout)

### Arduino Nano (Monitoramento)
| Componente | Pino Arduino |
| :--- | :--- |
| **Display CS** | D10 |
| **Display RST** | D9 |
| **Display DC** | D8 |
| **Botão Calib.** | D2 |
| **MPU6050** | I2C (A4/SDA, A5/SCL) |

### ESP32 (Controle)
| Função | Pino ESP32 | Descrição |
| :--- | :--- | :--- |
| **DT** | GPIO 4 | Habilitação Digital |
| **ET** | GPIO 5 | Habilitação Digital |
| **PWM 1** | GPIO 18 | Controle Motor Esq. |
| **PWM 2** | GPIO 19 | Controle Motor Dir. |
| **LED Teste** | GPIO 2 | Feedback Visual |

---

## 📡 Tópicos MQTT

O ESP32 subscreve aos seguintes tópicos para receber comandos remotos. O broker configurado é o `broker.hivemq.com`.

| Tópico | Payload | Ação |
| :--- | :--- | :--- |
| `hover/led` | `"TEST"` | Pisca o LED interno 5 vezes. |
| `hover/dt` | `"on"` / `"off"` | Liga/Desliga pino DT (GPIO 4). |
| `hover/et` | `"on"` / `"off"` | Liga/Desliga pino ET (GPIO 5). |
| `hover/pwm1` | `0` a `255` | Define velocidade do Motor 1. |
| `hover/pwm2` | `0` a `255` | Define velocidade do Motor 2. |

---

## 🖥️ Interface e Calibração

O sistema de monitoramento possui uma interface gráfica avançada.

### Processo de Calibração
1. Pressione o botão conectado ao **Pino 2**.
2. **Etapa 1 (20s):** O sistema lê o ruído ambiente (motor desligado).
3. **Contagem Regressiva:** O sistema pedirá para ligar o motor.
4. **Etapa 2 (20s):** O sistema analisa a vibração operacional do motor.
5. **Conclusão:** O sistema define automaticamente o `limiteAmplitude` e retorna à tela principal.

### Saída Serial (Arduino)
O Arduino envia telemetria via Serial (Baud 115200) no formato:
```text
<frequencia, rpm, amplitude, temperatura, pitch, roll, alerta>
Exemplo: <45.2, 2712, 0.350, 42.1, 12.5, -3.2, 0>
