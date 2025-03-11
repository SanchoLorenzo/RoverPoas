import serial
import time
import re

# Comunicación Serial
# Reemplazar 'COM12' por el que tenga el Arduino cuando se conecte
ser = serial.Serial('COM12', 9600, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

# Conteo para promedios
distance_sum = 0.0
temperature_sum = 0.0
humidity_sum = 0.0
count = 0

# Tamaño de muestreo
# Disminuir si se requiere más velocidad o menos recursos computacionales
batchSize = 10000

# Regex para extraer la información del mensaje
pattern = r"Dist: ([0-9.]+), Temp: ([0-9.]+), Hum: ([0-9.]+)"

try:
    while True:
        try:
            # Leer línea del serial
            line = ser.readline().decode('utf-8').strip()
            if line:
                #print(line)    # Desdocuemntar para debugging
                
                # Match con el patrón para extraer datos
                match = re.match(pattern, line)
                if match:
                    # Extraer y convertir en float
                    distancia = float(match.group(1))
                    temperatura = float(match.group(2))
                    humedad = float(match.group(3))
    
                    # Actualizar suma y contador
                    distance_sum += distancia
                    temperature_sum += temperatura
                    humidity_sum += humedad
                    count += 1
    
                    # Cuando se llegue al número de conteo se imprimen
                    # promedios y se reinicia el conteo
                    if count == batchSize:
                        # Calculate averages
                        avg_distance = distance_sum / count
                        avg_temperature = temperature_sum / count
                        avg_humidity = humidity_sum / count
                        
                        # Print the averages
                        print(f"\nAverage Distance: {avg_distance:.2f}\n"
                              f"Average Temperature: {avg_temperature:.2f}\n"
                              f"Average Humidity: {avg_humidity:.2f}")
                        
                        distance_sum = 0.0
                        temperature_sum = 0.0
                        humidity_sum = 0.0
                        count = 0
    
            # Si la computadora se realentiza se desdocumenta esta línea
            # y se aumenta el delay hasta lograr un balance
            #time.sleep(0.001)
    
        except UnicodeDecodeError:
            pass

except KeyboardInterrupt:
    print("Programa terminado por el usuario.")

finally:
    ser.close()
