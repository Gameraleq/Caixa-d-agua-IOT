#include <ESP8266WiFi.h>
#include <PubSubClient.h>  // Para MQTT
#include "informacoes_sensiveis.h"  // SSID e senha do Wi-Fi

// --- Configurações do HC-SR04 ---
#define TRIG_PIN D1  // Pino Trigger (D1 no NodeMCU)
#define ECHO_PIN D2  // Pino Echo (D2 no NodeMCU)

// --- Configurações MQTT ---
const char* MQTT_BROKER = "test.mosquitto.org";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "Teste";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// --------------------------------------------
// Função para medir a distância em centímetros (HC-SR04)
// --------------------------------------------
float medirDistanciaCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);  // Pulso de 10μs para iniciar a medição
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH);  // Mede o tempo de retorno do echo
  float distancia = duracao * 0.034 / 2;   // Convertendo para cm (velocidade do som = 0.034 cm/μs)

  return distancia;
}

// --------------------------------------------
// Função para conectar ao Wi-Fi
// --------------------------------------------
void conectarWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_KEY);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// --------------------------------------------
// Função para conectar ao Broker MQTT
// --------------------------------------------
void conectarMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);

  Serial.print("Conectando ao broker MQTT...");
  
  while (!mqttClient.connected()) {
    if (mqttClient.connect("NodeMCUClient")) {
      Serial.println("Conectado!");
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print("Falha. Código: ");
      Serial.print(mqttClient.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// --------------------------------------------
// Função para publicar a distância no MQTT
// --------------------------------------------
void publicarDistancia() {
  if (mqttClient.connected()) {
    float distancia = medirDistanciaCM();
    char mensagem[50];
    snprintf(mensagem, 50, "Distância: %.2f cm", distancia);  // Formata a mensagem

    mqttClient.publish(MQTT_TOPIC, mensagem);
    Serial.println("Publicado: " + String(mensagem));
  } else {
    Serial.println("Conexão MQTT perdida. Reconectando...");
    conectarMQTT();
  }
}

// --------------------------------------------
// Setup e Loop
// --------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  conectarWiFi();
  conectarMQTT();
}

void loop() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();

  // Publica a distância a cada 2 segundos
  static unsigned long ultimoTempo = 0;
  if (millis() - ultimoTempo > 2000) {
    publicarDistancia();
    ultimoTempo = millis();
  }
}