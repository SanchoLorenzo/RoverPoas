import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import time
import csv
from matplotlib.animation import FuncAnimation

import time
import board
import adafruit_dht

import RPi.GPIO as GPIO
import time

'''
Sensor Humedad y Temperatura DHT :

https://learn.adafruit.com/dht-humidity-sensing-on-raspberry-pi-with-gdocs-logging/python-setup

'''

# Initial the dht device, with data pin connected to:
dhtDevice = adafruit_dht.DHT22(board.D17)

def get_temp_humid():

    var = True
    while var:
        try:
            # Print the values to the serial port
            var = 0
            return dhtDevice.temperature, dhtDevice.humidity
            

        except RuntimeError as error:
            # Errors happen fairly often, DHT's are hard to read, just keep going
            var = 0
            print(error.args[0])
            return 0,0
            continue
            
            
            
        except Exception as error:
            var = 0
            dhtDevice.exit()
            raise error
            return 0,0
    
        
'''
Sensor Ultrasónico HC-SR04:

https://pimylifeup.com/raspberry-pi-distance-sensor/

'''
# Configurar los pines GPIO
GPIO.setmode(GPIO.BCM)

TRIG = 23
ECHO = 24

# Configurar los pines de entrada y salida
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

def get_distance():
    # Enviar un pulso de 10 microsegundos a TRIG
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    # Marcar el tiempo cuando ECHO pasa de bajo a alto
    while GPIO.input(ECHO) == 0:
        pulse_start = time.time()

    while GPIO.input(ECHO) == 1:
        pulse_end = time.time()

    # Calcular la diferencia de tiempo
    pulse_duration = pulse_end - pulse_start

    # Multiplicar por la velocidad del sonido (34300 cm/s)
    # y dividir por 2 porque el sonido va y viene
    distance = pulse_duration * 34300 / 2

    return distance



'''
Ploteo de sensores: 
'''
# Tiempo de inicio del programa
start_time = time.time()

# Creamos el archivo CSV para almacenar los datos de los sensores
filename = 'sensor_data.csv'
with open(filename, 'w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(["Timestamp", "Sensor1", "Sensor2", "Sensor3"])

# Función para simular los valores de los sensores
def simulate_sensor_values():
    
    timestamp = time.time() - start_time  # Tiempo transcurrido desde el inicio del programa
    sensor1 = get_distance()   # Simulando voltajes entre 0 y 5V
    sensor2, sensor3 = get_temp_humid()
    
    return timestamp, sensor1, sensor2, sensor3

# Función para agregar datos al archivo CSV
def append_to_csv(data):
    with open(filename, 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(data)

# Preparar el plot con 3 subplots
fig, axs = plt.subplots(3, 1, figsize=(10, 8))  # 3 filas, 1 columna

# Colores para cada sensor
colors = ['blue', 'green', 'red']
labelsx = ['Distancia (cm)', 'Temperatura (°C)', 'Humedad (%)']
ylims = [(0,100), (20,50), (25,75)]

# Función para actualizar el plot
def animate(i):
    data = simulate_sensor_values()
    append_to_csv(data)

    df = pd.read_csv(filename)

    # Limitar a mostrar solo los últimos 100 segundos de datos
    max_time = df['Timestamp'].max()
    min_time_for_plot = max(0, max_time - 25)
    df = df[df['Timestamp'] > min_time_for_plot]

    # Actualizar cada subplot con los datos correspondientes
    for ax, sensor, color, labelx, ylim in zip(axs, ['Sensor1', 'Sensor2', 'Sensor3' ], colors, labelsx, ylims):
        ax.clear()  # Limpia el plot anterior
        ax.plot(df['Timestamp'], df[sensor], label=sensor, color=color)
        
        ax.set_ylim(ylim)
        
        ax.set_xlabel('Tiempo (s)')
        ax.set_ylabel(labelx)
        ax.legend(loc='upper left')

    plt.tight_layout()

# Animación que actualiza el plot cada 500 ms
ani = FuncAnimation(fig, animate, interval=200)

plt.show()