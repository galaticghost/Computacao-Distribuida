#include <SoftwareSerial.h>
#include <Wire.h>
#include "DHT.h"

SoftwareSerial ESP_Serial(11, 12); // RX, TX

String rede = ;
String senha = ;

const int pino_dht = 9;   // pino onde o sensor DHT está conectado
float temperatura;        // variável para armazenar o valor de temperatura
float umidade;            // variável para armazenar o valor de umidade
DHT dht(pino_dht, DHT11); // define o pino e o tipo de DHT

void setup()
{
    Serial.begin(9600);
    ESP_Serial.begin(9600);
    dht.begin(); // inicializa o sensor DHT

    Serial.println("================================");
    Serial.println("Teste ESP8266");
    Serial.println("================================");

    executarComando("AT", 2000);

    executarComando("AT+CWMODE=1", 2000);

    Serial.println("\nProcurando redes WiFi...");
    executarComando("AT+CWLAP", 15000);

    Serial.print("Senha: [");
    Serial.print(senha);
    Serial.println("]");
    Serial.println("\nTentando conectar...");
    String cmd = "AT+CWJAP=\"";
    cmd += rede;
    cmd += "\",\"";
    cmd += senha;
    cmd += "\"";

    Serial.print("Comando enviado: ");
    Serial.println(cmd);

    executarComando(cmd, 20000);

    Serial.println("\nConsultando IP...");
    executarComando("AT+CIFSR", 5000);
}

void loop()
{
    delay(2000);

    float temperatura = dht.readTemperature();
    float umidade = dht.readHumidity();

    if (isnan(temperatura) || isnan(umidade))
    {
        Serial.println("Falha na leitura do DHT!");
        return;
    }

    // SERIAL
    Serial.print("Temp: ");
    Serial.print(temperatura);
    Serial.print(" C | Umidade: ");
    Serial.print(umidade);
    Serial.println(" %");

    enviarDados(temperatura, umidade);
}

void enviarDados(float temperatura, float umidade)
{
    String host = ;
    String json = "{";
    json += "\"temperatura\":";
    json += String(temperatura, 1);
    json += ",";
    json += "\"umidade\":";
    json += String(umidade, 1);
    json += "}";

    String request = "";
    request += "POST /telemetria HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "Content-Type: application/json\r\n";
    request += "Connection: close\r\n";
    request += "Content-Length: " + String(json.length()) + "\r\n\r\n";
    request += json;

    Serial.println("\nAbrindo conexao TCP...");

    executarComando("AT+CIPSTART=\"TCP\",\"COLOCA O IP AQUI\",8000", 10000);

    ESP_Serial.print("AT+CIPSEND=");
    ESP_Serial.println(request.length());

    delay(2000);

    while (ESP_Serial.available())
    {
        Serial.write(ESP_Serial.read());
    }

    ESP_Serial.print(request);

    Serial.println("\nPOST enviado!");

    delay(3000);

    while (ESP_Serial.available())
    {
        Serial.write(ESP_Serial.read());
    }

    executarComando("AT+CIPCLOSE", 3000);
}

void executarComando(String comando, unsigned long timeout)
{
    Serial.println("\n>>> " + comando);

    ESP_Serial.println(comando);

    unsigned long inicio = millis();

    while (millis() - inicio < timeout)
    {
        while (ESP_Serial.available())
        {
            char c = ESP_Serial.read();
            Serial.write(c); // mostra exatamente a resposta do ESP
        }
    }

    Serial.println("\n--------------------");
}