// BIBLIOTECAS NECESSÁRIAS
#include <EmonLib.h>		// Emon Libraara os sensores.
#include <SPI.h>				// SPI Library : necessária para o Shield Ethernet.
#include <Ethernet.h>		// Ethernet Library : para o comunicação com a nuvem.

// VARIÁVEIS GLOBAIS
EnergyMonitor emon1;		// Uma instância de um monitor de energial da Emon Library.
const int CT_PIN = 1;		// Pino onde está conectado o sinal do sensor de corrente.

byte ip[]   = {192,168,  1,101};                      // ip address (if dhcp not used)
byte gw[]   = {192,168,  1,  1};                      // gateway address
byte ns[]   = {  8,  8,  8,  8};                      // name server address
byte sub[]  = {255,255,255,  0};                      // network mask
byte mac[]  = { 0x90, 0xA2, 0xDA, 0x0E, 0x04, 0x21 }; // Endereço MAC do Shield Ethernet (olhe atrás dele).
EthernetClient client;		// Uma instância de um cliente TCP (usaremos eles para enviar os dados).

//Emoncms configurations
//char server[] = "emoncms.org";      // name address for emoncms.org
// OR 
IPAddress server(80, 243, 190, 58);  // numeric IP for emoncms.org (no DNS)

  String apikey = "0fc3340c4434a7d42986e83fe2745e01";  //api key  emoncms.org
  int node = 1; //if 0, not used

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 5*1000;  // delay between updates, in milliseconds

void setup()
{ 
  delay(10000);
	// Prepara a porta serial para ser utilizada para depuração. 
  Serial.begin(9600);
  Serial.println("Emoncms Cliente Inicializando");

  // Inicializa o monitor de energia.
  emon1.current(CT_PIN, 64);		// Calibração do sensor (deveria ser 111.1 para o SCT-013-100).

 	// Inicializa o Shield Ethernet utilizando DHCP.
	Serial.print("Inicializando Ethernet com DHCP...");
	if (Ethernet.begin(mac) == 0) { 
		Serial.println("falhou.");
    Serial.print("Inicializando com IP fixo...");
    Ethernet.begin(mac,ip,ns,gw,sub);
    Serial.println("feito!");
		//Serial.println("Aplicação interrompida.");
		//while(1);		// Fica em loop indefinidamente (aplicação interrompida).
	} else {
		Serial.println("feito!");
	}
	delay(1000);
	Serial.print("Endereço IP: ");
	Serial.println(Ethernet.localIP()); // Imprime o endereço IP conseguido.

	Serial.println("Iniciando medições: \n");
}

// this method makes a HTTP connection to the server:
void sendData() {
         Serial.println("toti");
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("Connecting...");
    // send the HTTP GET request:
    client.print("GET /emoncms/input/post?apikey=");
    client.print(apikey);
    if (node > 0) {
      client.print("&node=");
      client.print(node);
    }
    client.print("&json={");
    client.print("light:");
    client.print(lightValue);
    client.println("} HTTP/1.1");
    client.println("Host:emoncms.org");
    client.println("User-Agent: Arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("Connection failed");
    Serial.println("Disconnecting...");
    client.stop();
  }
  
void loop()
{
	// Mede a corrente usando a biblioteca EmonLib e calcula potência.
	// Imprime dados na serial para depuração.
  double Irms = emon1.calcIrms(1480);  // Mede a corrente RMS.
  double Potencia = Irms * 127.0;		// Calcula a potência aparente (supondo que a rede elétrica esteja em 127 V). isto ocasiona em uma margem de erro de até 20% na leitura
  Serial.print("Irms: ");
  Serial.print(Irms);		// Imprime a corrente na serial.
  Serial.print(" A \t");
  Serial.print("Potencia: ");
  Serial.print(Potencia);	// Imprime a potência na serial.
  Serial.print(" W");
  Serial.println();

  

  // Aguarde 10 segundos e siga em frente.
  delay(2000);

}
