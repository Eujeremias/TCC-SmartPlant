#include <ESP8266WiFi.h>
#include <coredecls.h>        // settimeofday_cb()
#include <ESP8266WebServer.h> // biblioteca para servidor WEB

#include "index.h" //arquivom com a página web

const char*   WIFI_SSID     = "IFAL - Rio Largo";
const char*   WIFI_PASSWORD = "ifalriolargo";

IPAddress ip(192, 168, 1, 50); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
IPAddress gateway(192, 168, 1, 1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 255, 0); //MASCARA DE REDE
IPAddress DNS1(8, 8, 8, 8); //dns do google
IPAddress DNS2(8, 8, 4, 4);

// Wi-Fi ------------------------------------------------
ESP8266WebServer server(80); // objeto server atenderá na porta 80 (porta padrão) quando solicitado no navegador.
// Constantes -------------------------------------------
const char*   ntpServer           = "pool.ntp.org";
const long    gmtOffset_sec       = -3 * 60 * 60;   // -3h*60min*60s = -10800s
const int     daylightOffset_sec  = 0;              // Fuso em horário de verão

time_t        nextNTPSync         = 0;
int ano, mes, dia, hora, minuto, segundo, dia_semana;
unsigned long ultimo_tempo = 0,
              ultimo_tempo2 = 0,
              tempo_anterior = 0;

int buzzer = D4;
int botao = D5;
int estado_botao;

void raiz() {
  server.send(200, "text/html", pagina);
}

void tratarBotao(){
  int botaoDaVez = (server.arg("nBotao")).toInt();
  if(botaoDaVez == 1){
      for (int i = 0; i < 180; i++) {
      float y = sin(i * (3.14 / 180));
      int som = 2000 + int(y * 1000);
      tone(buzzer, som);
      delay(2);
    }
  }

void setup() {
  Serial.begin(115200);
  Serial.print("\nConectando WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.config(ip, gateway, subnet, DNS1, DNS2); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU

  pinMode(buzzer, OUTPUT);
  pinMode(botao, INPUT_PULLUP);
  server.on("/", raiz);
  server.on("/requisicaoBotao", tratarBotao);
  server.begin(); //INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA DEFINIDA EM "WiFiServer server(porta);"

  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

}
  noTone(buzzer);
  server.send(200, "text/html", pagina);


}

void loop() {

}
