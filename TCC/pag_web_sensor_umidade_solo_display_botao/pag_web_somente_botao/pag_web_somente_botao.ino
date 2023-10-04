//CANAL INTERNET E COISAS https://www.youtube.com/watch?v=wmxptpd6aWc
//https://randomnerdtutorials.com/esp8266-nodemcu-date-time-ntp-client-server-arduino/
//udp e ip fixo: https://labdegaragem.com/forum/topics/problema-para-acessar-hor-rio-da-internet?id=6223006%3ATopic%3A688591&page=1#comments

// Bibliotecas ------------------------------------------
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

// Variáveis globais ------------------------------------
time_t        nextNTPSync         = 0;
int ano, mes, dia, hora, minuto, segundo, dia_semana;
unsigned long ultimo_tempo = 0,
              ultimo_tempo2 = 0,
              tempo_anterior = 0;
int led2 = 2,   //Led Build_in D4 GPIO-2
    led16 = 16; //D0
int buzzer = D2, rele = D3;
int botao = D6;
int estado_botao;
int soar; //ativar ou desativar buzzer

// Funções auxiliares -----------------------------------
void raiz() {
  server.send(200, "text/html", paginaWeb);
}

void tratarBotao() {
  int botaoDaVez = (server.arg("nBotao")).toInt();

  if (botaoDaVez == 1) {
    toque();z
  }
  else if (botaoDaVez == 2) {
    digitalWrite(led16, !digitalRead(led16));
  }

  server.send(200, "text/html", paginaWeb);
}

String diaextenso(int dia) {
  String x;
  switch (dia) {
    case 0: x = "Domingo"; break;
    case 1: x = "Segunda-feira"; break;
    case 2: x = "Terça-feira"; break;
    case 3: x = "Quarta-feira"; break;
    case 4: x = "Quinta-feira"; break;
    case 5: x = "Sexta-feira"; break;
    case 6: x = "Sábado"; break;
  }
  return x;
}

void atualizarHorario() {
  String y = diaextenso(dia_semana);
  String x = y + "<br>" + horario() + "<br>" + data();
  server.send(200, "text/plane", String(x));
}

String data() { // data no formato dd/mm/aaaa
  String teste;

  if (dia <= 9) {
    teste += "0";
  }
  teste += dia;
  teste += "/";
  if (mes <= 9) {
    teste += "0";
  }
  teste += mes;
  teste += "/";
  teste += ano;

  return teste;
}

String horario() { //horário no formato hh:mm:ss
  String teste;

  if (hora <= 9) {
    teste += "0";
  }
  teste += hora;
  teste += ":";
  if (minuto <= 9) {
    teste += "0";
  }
  teste += minuto;
  teste += ":";
  if (segundo <= 9) {
    teste += "0";
  }
  teste += segundo;

  return teste;
}

String dateTimeStr(time_t t, int8_t tz = -3) {
  // Formata time_t como "aaaa-mm-dd hh:mm:ss"
  if (t == 0) {
    return "N/D";
  }
  else {
    t += tz * 3600;   // Ajusta fuso horário
    struct tm *ptm;
    ptm = gmtime(&t);
    String s;

    ano = (ptm->tm_year) + 1900;
    s = s + ano;
    s += "-";
    if (ptm->tm_mon < 9) {
      s += "0";
    }
    s += ptm->tm_mon + 1;
    mes = (ptm->tm_mon) + 1;
    s += "-";
    if (ptm->tm_mday < 10) {
      s += "0";
    }
    s += ptm->tm_mday;
    dia = (ptm->tm_mday);
    s += " ";
    if (ptm->tm_hour < 10) {
      s += "0";
    }
    hora = (ptm->tm_hour);
    s += ptm->tm_hour;
    s += ":";

    if (ptm->tm_min < 10) {
      s += "0";
    }
    minuto = (ptm->tm_min);
    s += ptm->tm_min;
    s += ":";
    if (ptm->tm_sec < 10) {
      s += "0";
    }
    segundo = (ptm->tm_sec);
    s += ptm->tm_sec;

    dia_semana = (ptm -> tm_wday);

    return s;
  }
}

String timeStatus() {// Obtém o status da sinronização
  if (nextNTPSync == 0) {
    return "não definida";
  } else if (time(NULL) < nextNTPSync) {
    return "atualizada";
  } else {
    return "atualização pendente";
  }
}

// Callback de sincronização
void ntpSync_cb() {
  time_t t;
  t = time(NULL);
  // Data/Hora da próxima atualização
  nextNTPSync = t + (SNTP_UPDATE_DELAY / 1000) + 60; //ver video youtube

  Serial.println("Sincronizou com NTP em " + dateTimeStr(t));
  Serial.println("Limite para próxima sincronização é " +
                 dateTimeStr(nextNTPSync));
}

// Setup ------------------------------------------------
void setup() {

  pinMode(led2, OUTPUT);
  digitalWrite(led2, 1);//desliga o led2
  pinMode(led16, OUTPUT);
  digitalWrite(led16, 1);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, 0);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, 0);
  pinMode(botao, INPUT_PULLUP);

  // Inicializa Serial e define callback
  Serial.begin(115200);
  // Conecta WiFi
  Serial.print("\nConectando WiFi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.config(ip, gateway, subnet, DNS1, DNS2); //PASSA OS PARÂMETROS PARA A FUNÇÃO QUE VAI SETAR O IP FIXO NO NODEMCU
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led16, !digitalRead(led16));
    Serial.print(".");
    delay(200);//aguarda conexão
  }
  Serial.print("\nConnected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Função para inicializar o cliente NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  settimeofday_cb(ntpSync_cb);

  // Intervalo de sincronização - definido pela bibioteca lwIP
  Serial.printf("\n\nNTP sincroniza a cada %d segundos\n",
                SNTP_UPDATE_DELAY / 1000);

  //tratamento das requisições
  server.on("/", raiz);
  server.on("/requisicaoBotao", tratarBotao);
  server.on("/requisicaoHorario", atualizarHorario);

  server.begin(); //INICIA O SERVIDOR PARA RECEBER DADOS NA PORTA DEFINIDA EM "WiFiServer server(porta);"
}

// Loop -------------------------------------------------
void loop() {
  server.handleClient(); // função que permite ouvir as solicitações externas.

  if (millis() - ultimo_tempo > 1000) {
    alarmar();
    Serial.print(diaextenso(dia_semana) + "\t");
    Serial.println(dateTimeStr(time(NULL)) + "\t   Status: " + timeStatus());
    ultimo_tempo = millis();
  }

  if ((millis() - ultimo_tempo2 > 5000) && soar == 1) {
    digitalWrite(buzzer, 0);
    digitalWrite(led2, 1);
    soar = 0;
  }

  estado_botao = !digitalRead(botao);
  if (estado_botao == 1) {
    toque();
  }

}

void toque() {
  tone(buzzer, 987) ; //LA note ...
  digitalWrite(led2, 0);
  soar = 1;
  ultimo_tempo2 = millis();
}

void alarmar() {
  if (dia_semana > 0 && dia_semana < 6 ) {
    if (((hora == 7 && minuto == 00) ||
         (hora == 7 && minuto == 50) ||
         (hora == 8 && minuto == 40) ||
         (hora == 9 && minuto == 30) ||
         (hora == 9 && minuto == 50) ||
         (hora == 10 && minuto == 40) ||
         (hora == 11 && minuto == 30) ||
         (hora == 12 && minuto == 20) ||

         (hora == 22 && minuto == 34) ||

         (hora == 13 && minuto == 00) ||
         (hora == 14 && minuto == 40) ||
         (hora == 15 && minuto == 00) ||
         (hora == 16 && minuto == 40))

        && (segundo < 10 )) {

      if (segundo < 4) { // para acionar rele
        digitalWrite(rele, 1);
      }
      else {
        digitalWrite(rele, 0);
      }

      if (segundo < 5) { // acionar buzzer
        som_alarme();
      }
    }
    else {
      if (soar == 0) {
        analogWrite(buzzer, 0);
        digitalWrite(rele, 0);
        tempo_anterior = 0;
      }
    }
  }
}

void som_alarme() {
  if (millis() - tempo_anterior >= 1000) {

    for (int i = 0; i < 2; i++) {
      tone(buzzer, 1870);
      delay(100);
      tone(buzzer, 2870);
      delay(100);
      noTone(buzzer);
    }
    tempo_anterior = millis();
  }
}
