#include <ESP8266WiFi.h>
#include <coredecls.h>        // settimeofday_cb()
#include <ESP8266WebServer.h> // biblioteca para servidor WEB

#include "index_sirene.h" //arquivom com a página web

//DEFINIÇÃO DE IP FIXO PARA O NODEMCU
IPAddress ip(192, 168, 1, 50); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
IPAddress gateway(192, 168, 1, 1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 255, 0); //MASCARA DE REDE
IPAddress DNS1(8, 8, 8, 8); //dns do google
IPAddress DNS2(8, 8, 4, 4);

const char*   WIFI_SSID     = "oficinasifal";
const char*   WIFI_PASSWORD = "12345678";

// Wi-Fi ------------------------------------------------
ESP8266WebServer server(80); // objeto server atenderá na porta 80 (porta padrão) quando solicitado no navegador.
// Constantes -------------------------------------------
const char*   ntpServer           = "pool.ntp.org";
const long    gmtOffset_sec       = -3 * 60 * 60;   // -3h*60min*60s = -10800s
const int     daylightOffset_sec  = 0;              // Fuso em horário de verão

void raiz() {
  server.send(200, "text/html", paginaWeb);
}

void tratarBotao() {
  int botaoDaVez = (server.arg("nBotao")).toInt();

  if (botaoDaVez == 1) {
    toque();
  }
  else if (botaoDaVez == 2) {
    digitalWrite(led16, !digitalRead(led16));
  }

  server.send(200, "text/html", paginaWeb);
}


int buzzer = D4;
int botao = D5;
int estado_botao;

void setup() {
  // Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
}

void loop() {
  estado_botao = !digitalRead(botao);

  if(estado_botao == 1){
    for(int i = 0; i < 180; i++){
      float y = sin(i * (3.14 / 180));
      int som = 2000 + int(y * 1000);
      tone(buzzer, som);
      delay(2);
    } 
  }
  noTone(buzzer);;
}
