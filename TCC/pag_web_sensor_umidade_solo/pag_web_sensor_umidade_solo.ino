
// #include <WiFi.h> //lib para configuração do Wifi (para esp32)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#include "index.h"

const char* ssid = "IFAL - Rio Largo";
const char* password = "ifalriolargo";
int pinoSensorUmidade = A0;
int valor;
int porcentagem;
unsigned long int ultimo_tempo = 0;
//const char* ssid = "Espaço 4.0";
//const char* password = "Esp$@$2022";
int led_status = D0;

ESP8266WebServer server(80);

void raiz() {
  String s = pagina;
  server.send(200, "text/html", s);
}
void gerar_numero() {
  // int a = random(101);
  int a = porcentagem;
  String x = String(a);
  server.send(200, "text/plane", x);
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(pinoSensorUmidade, INPUT);
  pinMode(led_status, OUTPUT);
  
  Serial.println("");

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led_status, !digitalRead(led_status));
    delay(200);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  digitalWrite(led_status, 1);

  //metódos abaixo recebe valor tipo string no 2° parâmetro
  server.on("/", raiz);
  server.on("/lerNUM", gerar_numero);

  server.begin();
}

void loop() {
  server.handleClient();
  if (millis() - ultimo_tempo >= 500) {
    valor = analogRead(pinoSensorUmidade);
    porcentagem = map(valor, 484, 1023, 100, 0);
    Serial.println(porcentagem);
    ultimo_tempo = millis();
  }
}
