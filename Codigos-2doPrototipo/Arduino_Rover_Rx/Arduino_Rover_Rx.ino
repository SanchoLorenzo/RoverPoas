// Arduino situado en el Rover
// Rover TECSpace


#include <SPI.h>
#include <RH_RF95.h>
#include <DHT.h>


#define RFM95_CS 35
#define RFM95_RST 37
#define RFM95_INT 3
#define DHTPIN 2      // Pin digital donde está conectado el DHT22
#define DHTTYPE DHT22 // Definir el tipo de sensor DHT



// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 434.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 8

DHT dht(DHTPIN, DHTTYPE);

#define TRIG_PIN 6    // Pin de salida para el TRIG del HC-SR04
#define ECHO_PIN 7   // Pin de entrada para el ECHO del HC-SR04
  
// Función para inicializar los sensores
void initSensors() {
    dht.begin();
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

// Función para leer temperatura y humedad
void getTempHumid(float &temperature, float &humidity) {
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
        temperature = 3;
        humidity = 3;
    }
}

// Función para leer distancia con el sensor ultrasónico
float getDistance() {
    long duration;
    float distance;

    // Enviar un pulso de 10 microsegundos a TRIG
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Leer la duración del pulso ECHO
    duration = pulseIn(ECHO_PIN, HIGH);

    // Calcular la distancia
    distance = (duration * 0.0343) / 2;

    return distance;

}


void setup() 
{
  pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  while (!Serial);
  Serial.begin(9600);
  delay(100);

  Serial.println("Arduino LoRa RX Test!");
  
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");

  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

  rf95.setTxPower(23, false);

  initSensors();
}



void loop()
{ 

  float distancia = getDistance();
  float temperatura, humedad;
  getTempHumid(temperatura, humedad);

  
  char mensajeDatos[40];
  char distBuffer[10], tempBuffer[10], humBuffer[10];

  dtostrf(distancia, 6, 2, distBuffer);
  dtostrf(temperatura, 6, 2, tempBuffer);
  dtostrf(humedad, 6, 2, humBuffer);
  snprintf(mensajeDatos, sizeof(mensajeDatos), "Dist: %s, Temp: %s, Hum: %s", distBuffer, tempBuffer, humBuffer);


  rf95.send(mensajeDatos, sizeof(mensajeDatos));
  rf95.waitPacketSent();


  if (rf95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    
    if (rf95.recv(buf, &len))
    {
      Serial.print("Got: ");
      Serial.println(buf[0], BIN);
    }
    else
    {
      Serial.println("Receive failed");
    }

    bool bit0 =  buf[0]       & 0x01;  // Guardar el Bit 0 (M1A)
    bool bit1 = (buf[0] >> 1) & 0x01;  // Guardar el Bit 1 (M1A)
    bool bit2 = (buf[0] >> 2) & 0x01;  // Guardar el Bit 2 (M1A)
    bool bit3 = (buf[0] >> 3) & 0x01;  // Guardar el Bit 3 (M1A)

    digitalWrite(12, bit0 ? HIGH : LOW);
    digitalWrite(11, bit1 ? HIGH : LOW);
    digitalWrite(10, bit2 ? HIGH : LOW);
    digitalWrite(9,  bit3 ? HIGH : LOW);


  }

  delay(100);
}