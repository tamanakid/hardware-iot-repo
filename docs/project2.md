# Entregable 2 - Desarrollo con ESP8266

*Hardware para IoT - DTE UPM.*

Realizado por Nicolás Villanueva.

## 1. Revisión de Funcionalidad

El prototipo se ha realizado conforme a las especificaciones resaltadas en el enunciado del entregable, donde algunas decisiones de diseño que no se establecán explícitamente en el documento han sido tomadas por el alumno y resaltadas **en negrita**

* El programa intentará conectarse a la red WiFi especificada en el fichero `ssid.c` mientras el LED interno conmuta cada 0.5s.
  * De fallar el intento de conexión, **El LED deja de conmutar**, 
  * Al conectarse, el LED se mantendrá encendido.
  * Si se pierde la conexión, **Se intentará conectarse nuevamente a la misma red** (Se puede probar apagando y rápidamente encendiendo la red)
* Al conectarse a la red, el programa activa la mayor parte de la funcionalidad:
  * El zumbador conmutará tres veces.
  * Realiza una llamada a un servidor NTP para obtener la hora actual, y ejecuta una tarea de "reloj" interno que mantiene sincronizada la hora.
  * Realiza medidas de luminosidad cada 5 segundos
  * Calcula la media de las medidas durante el último minuto y la guarda en la memoria flash (junto a su timestamp y si está sobreiluminado o no)
* Se realiza una interfaz de usuario para interactuar con el programa
  * Muestra la última medida de luminosidad realizada
  * Muestra el umbral de iluminación y permite su edición.
  * Si la úlitma medida supera el umbral, muestra un botón que permite activar un LED de alarma (Dicho botón no se muestra si esta alarma ya se encuentra activada)
    * Este LED **se apaga apenas el programa detecte una medida inferior al umbral**
  * Se muestra un "LED" en la interfaz que replicará el comportamiento del LED real de alarma


&nbsp;
## 2. Revisión de la Implementación

La modularización de las tareas y la centralización del estado se siguen majenando como en el primer prototipo, donde las tareas son las siguientes:

* **`task_wifi_connect`**: Realiza la conexión con la red WiFi (Y hacer conmutar el LED interno)
* **`task_buzzer`**: Conmuta el zumbador cuando se accede a dicha red WiFi.
* **`task_light_sensor`**: Realiza las lecturas de luminosidad y guarda la media calculada en la memoria flash
* **`task_web_server`**: Sirve las peticiones HTTP realizadas por los clientes
* **`task_clock`**: Obtiene la hora actual del servidor NTP y mantiene el transcurso del tiempo.

Por otra parte, la conmutación global del estado ocurre según el programa se conecte/desconecte de la red WiFi, ya que la activación de las demás tareas depende de dicha conexión, y el encendido/apagado de la tarea del zumbador.


### Funcion de transferencia de la medida de exposición luminosa y guardado en memoria flash.

La medición del sensor de luminosidad consta del siguiente procedimiento:

1. Cada 5 segundos, se activa la tarea encargada de realizar la medición
2. Se obtiene el valor analógico mediante la llamada a `analogRead(port)`
3. Se realiza la conversión del valor "digital" (Entre 0 y 1024) y el equivalente en mV, según la función de transferencia del sensor.

Para hallar dicha **función de transferencia** se realizó el siguiente procedimiento:

* Se realizaron dos medidas del sensor en valores extremos: oscuridad máxima y luminosidad máxima.
* Se conectó un multímetro en la salida del sensor para obtener la tensión resultante
* Se mostró por consola de depuración el valor retornado por la llamada a `analogRead(port)`

Se obtuvo entonces los resultados presentados en la tabla. Nótese que, aunque el FSR del sensor llega hasta los 3.18V, la ADC del μP provee su salida máxima a partir de una entrada de 3.08V, reduciendo el FSR efectivo del programa:

| Medida | Codigo Digital (CD) | V [mV] |
|--------|---------------------|--------|
| Mínima | 6                   | 0.01   |
| Maxima | 1021                | 3.07   |
| Maxima | 1024                | 3.08   |
| Maxima | 1024                | 3.18   |

Por ende, la función de transferencia resulta:

***V = (3.0157 * CD) - 8.094 [mV]***

Lo que se ve reflejado en el código de la tarea `task_light_sensor`

```
int analog_read = analogRead(MINID1_PIN_A0);
state.light = (analog_read * 3.0157) - 8.094;
```

Luego se procede a guardar, en la memoria flash, el registro de la media calculada durante el minuto con la siguiente información:

* **timestamp**: presente en el estado global y gestionado por la tarea `task_clock`.
* **luminosidad media (mV)**: Se guarda la media de las 12 medidas durante el minuto.
* **subiluminado (underlit) o sobreiluminado (overlit)**: según la media supere o no el umbral establecido.

```
task:LightSensor> Mean written to measures file:
>>> 16:58:33 - 2136 - overlit!

task:LightSensor> Mean written to measures file:
>>> 16:59:33 - 0407 - underlit
```


### Estructura de ficheros

El proyecto tiene una estructura idéntica al proyecto anterior. Se reiteran los siguientes elementos:

* **`sketch_project.ino`**: Es el eje del proyecto: contiene las definiciones de las funciones `setup()` y `loop()`, al igual que la lógica de centralización del estado.
* **`task_<task-name>.ino`**: Cada "tarea" que se instancia para el scheduler se ha separado en un fichero. Puede estar compuesto de 3 llamadas:
  * `void setup<TaskName>(void)`: (opcional) se llama desde `setup()` y simplemente inicializa el hardware a utilizarse (i.e. `pinMode()`)
  * `void task<TaskName>(void)`: La ejecución de la tarea en sí que ejecuta `scheduler.h`
  * `void reset<TaskName>(void)`: (opcional) la restauración al estado inicial/inactivo de la tarea (i.e. apagado de LEDs), que se suele llamar en conjunto con el `scheduler_deactivate(task)` de la misma.
* **`scheduler`**: Con sus respectivos ficheros `.c` y `.h`, contiene el código de la librería realizada.
* **`sketch.h`**: Contiene la declaración de los tipos utilizados para el estado (y su centralización) así como la declaración de las funciones de cada task, con el fin de evitar un fichero `.h` para cada task.
* **`hardware_miniD1.h`**: Contiene las constantes que se mapean a la interfaz hardware del wearable (i.e. pines). _Nota: Puede tener algunos pines incorrectos que aún no se hayan utilizado_

Se ha añadido además el fichero `ssid.h` el cual contiene la información de la red WiFi a la que conectarse. Es importante notar que este fichero no se añade al control de versiones, ya que esto expondría dichas redes.


&nbsp;
## Posibles mejoras a la implementación

* Como un añadido, podría incorporarse una breve funcionalidad a la tarea `task_wifi_connect` que permita declarar varias redes WiFi, tal que de no poder conectarse a alguna (por error de autenticación o no disponibilidad de dicha red) intente conectarse a las siguientes, de forma iterativa. Esta lógica no debería tener demasiada complicación



## Referencias

* **[Repositorio](https://github.com/tamanakid/hardware-iot-repo)**
* **[Manejo del Buzzer: Documentación de Lilypad](https://learn.sparkfun.com/tutorials/lilypad-protosnap-plus-activity-guide/5-play-a-tune)**
* **[Arduino Wifi Library Reference](https://www.arduino.cc/en/Reference/WiFi)**
* **[ESP8266 Web Server Library Reference](https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer)**
* **[Arduino ESP8266 filesystem uploader reference](https://github.com/esp8266/arduino-esp8266fs-plugin)**
* **[Blog Post: Getting Date & Time From NTP Server With ESP8266 NodeMCU](https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/)**
* **[FS library API (header file)](https://github.com/esp8266/Arduino/blob/master/cores/esp8266/FS.h)**

