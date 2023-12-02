#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid = "Fikra Wifi";
const char* password = "fikra_pass";

const int pwmMotorA = D1;
const int pwmMotorB = D2;
const int dirMotorA = D3;
const int dirMotorB = D4;

WiFiUDP Udp;
unsigned int localUdpPort = 6000;  // local port to listen on
uint32_t controlAddr;

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Udp.setTimeout(200);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  controlAddr = ((uint32_t)(WiFi.localIP()) & 0x00FFFFFF) + 0x01000000;

  delay(500);

  char response = 0;
  do {
    Udp.beginPacket(controlAddr, localUdpPort);
    Udp.write("Hello");
    Udp.endPacket();

    delay(100);

    Udp.read(&response, 1);
    delay(100);
  } while (response != 'k');

  pinMode(pwmMotorA, OUTPUT);
  pinMode(pwmMotorB, OUTPUT);
  pinMode(dirMotorA, OUTPUT);
  pinMode(dirMotorB, OUTPUT);
}

char data;
int size;
void loop() {
  size = Udp.parsePacket();
  if(size > 0) {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", size, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(&data, 1);
    if (len > 0)
    {
      switch(data) {
        case '1': forward(); break;
        case '2': turnleft(); break;
        case '3': turnright(); break;
        case '4': backward(); break;
        default: stop(); break;
      }
    }

  } else {
    stop();
  }

  delay(100);
}

void stop() {
  Serial.println("stop");
  digitalWrite(pwmMotorA, LOW);
  digitalWrite(pwmMotorB, LOW);
}

void forward() {
  Serial.println("forward");
  digitalWrite(pwmMotorA, HIGH);
  digitalWrite(pwmMotorB, HIGH);
  digitalWrite(dirMotorA, HIGH);
  digitalWrite(dirMotorB, HIGH);
}

void backward() {
  Serial.println("backward");
  digitalWrite(pwmMotorA, HIGH);
  digitalWrite(pwmMotorB, HIGH);
  digitalWrite(dirMotorA, LOW);
  digitalWrite(dirMotorB, LOW);
}

void turnleft() {
  Serial.println("left");
  digitalWrite(pwmMotorA, HIGH);
  digitalWrite(pwmMotorB, HIGH);
  digitalWrite(dirMotorA, LOW);
  digitalWrite(dirMotorB, HIGH);
}

void turnright() {
  Serial.println("right");
  digitalWrite(pwmMotorA, HIGH);
  digitalWrite(pwmMotorB, HIGH);
  digitalWrite(dirMotorA, HIGH);
  digitalWrite(dirMotorB, LOW);
}
