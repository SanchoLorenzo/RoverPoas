#define pinSensorCorriente A0

const int periodoMuestreo = 1000;
unsigned long tiempoPasado = 0;
float corrienteSuma = 0.0;
unsigned long conteoMuestreo = 0;
float corrientePromedio = 0.0;

const int PinIN1 = 10;
const int PinIN2 = 9;
const int PinIN3 = 5;
const int PinIN4 = 6;

void setup() {
  // inicializar la comunicación serial a 9600 bits por segundo:
  Serial.begin(9600);
  // configuramos los pines como salida
  pinMode(PinIN1, OUTPUT);
  pinMode(PinIN2, OUTPUT);
  pinMode(PinIN3, OUTPUT);
  pinMode(PinIN4, OUTPUT);
}

void loop() {
  Adelante();
  if (millis() - tiempoPasado <= periodoMuestreo) {
    conteoMuestreo++;
    unsigned int corrienteMedida = analogRead(pinSensorCorriente);
    float tensionCorriente = (float)corrienteMedida/1024.0 * 5.0;
    //if(tensionCorriente >= 0) {
      float corriente = (2.5 - tensionCorriente)/0.185;
      //Serial.println(corriente);
      corrienteSuma += corriente;
      delay(10);
    //}
  }
  else {
    corrientePromedio = (float)corrienteSuma/(float)conteoMuestreo;
    Serial.print("Corriente promedio: "); Serial.print(corrientePromedio); Serial.println(" A");
    if(corrientePromedio >= 0.3) {
      MotorStop();
      delay(5000);
    }
    corrienteSuma = 0.0;
    conteoMuestreo = 0;
    corrientePromedio = 0.0;
    tiempoPasado = millis();
  }
}
  

//función para girar el motor en sentido horario
void Adelante()
{
  digitalWrite (PinIN1, HIGH);
  digitalWrite (PinIN2, LOW);
  digitalWrite (PinIN3, HIGH);
  digitalWrite (PinIN4, LOW);
}
//función para girar el motor en sentido antihorario
void Atras()
{
  digitalWrite (PinIN1, LOW);
  digitalWrite (PinIN2, HIGH);
  digitalWrite (PinIN3, LOW);
  digitalWrite (PinIN4, HIGH);
}

//función para apagar el motor
void MotorStop()
{
  digitalWrite (PinIN1, LOW);
  digitalWrite (PinIN2, LOW);
  digitalWrite (PinIN3, LOW);
  digitalWrite (PinIN4, LOW);
}
