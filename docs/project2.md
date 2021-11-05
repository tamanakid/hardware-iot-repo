# Entregable 2 - Desarrollo con ESP8266

Realizado por Nicolás Villanueva.

## 1. Revisión de Funcionalidad

El prototipo se ha realizado conforme a las especificaciones resaltadas en el enunciado del entregable, donde algunas decisiones de diseño que no se establecán explícitamente en el documento han sido tomadas por el alumno y resaltadas **en negrita**

* El programa intentará conectarse a la red WiFi especificada en el fichero `ssid.c` mientras el LED interno conmuta cada 0.5s.
  * De fallar el intento de conexión, **El LED deja de conmutar**, 
  * Al conectarse, el LED se mantendrá encendido.
  * Si se pierde la conexión, **Se intentará conectarse nuevamente a la misma red** (Se puede probar apagando y rápidamente encendiendo la red)
* Al conectarse a la red, el programa activa la mayor parte de la funcionalidad:
  * El zumbador conmutará tres veces.
  * Realiza una llamada a un servidor NTP y ejecuta un "reloj" interno que mantiene sincronizada la hora.
  * Realiza medidas de luminosidad cada 5 segundos y las guarda en la memoria flash (junto a su timestamp y si está sobreiluminado)
* Se realiza una interfaz de usuario para interactuar con el programa
  * Muestra la última medida de luminosidad realizada
  * Muestra el umbral de iluminación y permite su edición.
  * Si la úlitma medida supera el umbral, muestra un botón que permite activar un LED de alarma (Dicho botón no se muestra si esta alarma ya se encuentra activada)
    * Este LED **se apaga apenas el programa detecte una medida inferior al umbral**
  * Se muestra un "LED" en la interfaz que replicará el comportamiento del led de alarma


&nbsp;
## 2. Revisión de la Implementación

La implementación en código del firmware del wearable consta de dos características fundamentales:
* Una estructura modular que permite dividir el software en "tareas" o _tasks_ y aislar la ejecución de las mismas. Esto se ha logrado mediante la creación de una librería `scheduler.h`, la cual permite instanciar dichas tareas por separado y activarlas o desactivarlas según sea necesario, lo que facilita el control de flujo en la aplicación.
* La centralización y gestión del estado global/compartido de la aplicación mediante un patrón "_flux_", el cual consiste en la restricción parcial o total de la modificación de dicho estado por código. Se ha implementado una versión "laxa" del patrón _flux_, fundamentalmente para gestionar las transiciones de las tareas de la librería `scheduler.h`.
-**Aunque no se ha podido realizar la restricción de acceso (Debido al uso de estructuras en C en lugar de clases de C++), de igual forma**-

### `scheduler.h` - Librería planificadora de ejecución de tareas

Como se ha mencionado, la librería se usa para instanciar y gestionar distintas tareas a ser ejecutadas por el wearable.

Similar a cómo se sugerió en clase, se instancia una lista donde se guardan las tareas a ejecutarse (`callback`) y cada cuántos ticks deben transcurrir para ejecutarlas (`schedule_ticks`), además teniendo un registro que cuenta cuántos ticks restan para la proxima ejecución (`remaining_ticks`), el cual se decrementa con el transcurso de cada tick.

```C
typedef struct {
  int schedule_ticks;
  taskCallback callback;
  bool is_active;
  int remaining_ticks;
} schedulerTask;
```

La librería tiene una interfaz bastante sencilla, teniendo los siguientes métodos:
* **`scheduler_add(int ticks, void (*taskCallback)(void) callback, bool is_active)`** crea una instancia de una tarea y la añade a la lista de tareas a ejecutar (Debe llamarse dentro el `setup()`)
	* `ticks` define el número de ticks que deben transcurrir para que se ejecute la tarea; cada cuánto se ejecutará
	* `callback` será la función que define la tarea a ejecutar, y se llamará cuando transcurran los `ticks` establecidos
	* `is_active` define el estado inicial de la tarea: activada o desactivada.
* **`scheduler_run()`** activa la ejecución secuencial de las tareas. Se llama al inicio de `loop()` y debe ser seguido por un `delay(ms)`, donde `ms` será detectado por `scheduler.h` como la duración de un tick.
* **`scheduler_activate(schedulerTask task)`** y **`scheduler_deactivate()`** activan y desactivan una tarea, respectivamente. Se podría modificar directamente la estructura que retorna `scheduler_add()`, sin embargo, se han añadido estos métodos para facilitar la legibilidad del código.

Pequeño ejemplo de uso a continuación: 

```C
void setup() {
  pins_setup();

  task_leds = scheduler_add(1, &taskLeds, true);
  task_buzzer = scheduler_add(4, &taskBuzzer, false);
}

void loop() {
  scheduler_run();
  delay(100);
}
```

### Cetralización del estado global mediante implementación laxa del patrón _flux_

Como se ha mencionado, el patrón _flux_ consiste en el flujo "unidireccional" de los datos; esto es, no podría utilizarse la misma interfaz para la lectura y escritura de una variable de estado. Es comúnmente utilizado en frameworks de aplicaciones web de front-end como React o Vue (ejecutados en el cliente -navegadores- y basados en JavaScript).

Sin embargo, tanto por motivos prácticos y restricciones del lenguaje C, se ha implementado una versión laxa del patrón, fundamentalmente en los dos siguientes puntos:
* C no provee restricción de acceso/escritura a los elementos de una estructura, a diferencia de las clases de C++. Por ende no puede imponerse el patrón de forma estricta, sino que queda en manos del desarrollador.
* El patrón viene implementado en librerías con estructuras internas complejas y enfocadas principalmente a la optimización del renderizado en navegadores. Sin embargo, en este caso sólo se ha utilizado para "centralizar" el código que provoca conmutaciones en la ejecución de las tareas (activación y desactivación de _tasks_)

La conmutación del estado se ha implementado en el fichero del `sketch_project.ino`, junto a las funciones `setup()` y `loop()`, en una función llamada `setSchedulerState(t_state_action action)`, donde `t_state_action` es un tipo enumerable que describe las distintas formas que hay de conmutar el estado:

```C
void setSchedulerState (t_state_action action) {

  switch (action) {
    case ACTION_WRIST_ACTIVATE:
      Serial.println(">> State Change: Wrist Activated");
      state.is_wrist_active = true;
      scheduler_activate(task_light_sensor);
      break;

    case ACTION_WRIST_DEACTIVATE:
      Serial.println(">> State Change: Wrist Deactivated");
      state.is_wrist_active = false;
      scheduler_deactivate(task_light_sensor);
      onResetLightSensor();
      break;
      
    case ACTION_GREEN_LED_ON:
      Serial.println(">> State Change: Alarm on Green LED blinking");
      scheduler_activate(task_green_led_blink);
      break;
    
    ...
}
```

Como se observa, el estado sólo se utiliza para manejar el estado de las _tasks_ que se ejecutan mediante `scheduler.h`, llamando a `scheduler_activate(task)` o `scheduler_deactivate(task)`, y se llaman a funciones de "reset" relacionadas a cada tarea para reiniciar el hardware a su estado inicial (i.e. apagando los LEDs). También se conmutan ciertas variables de `state` que afecten la ejecución de dichas tareas.

Desde cada tarea, se puede conmutar el estado fácilmente. Nótese la implementación del patrón _flux_: la lectura de la variable de estado (`state.alarm_state`) se realiza como de costumbre, sin embargo sólo se debe de cambiar mediante una llamada a `setSchedulerState()`.

```C
if (state.alarm_state != ALARM_SLEEP) {
  setSchedulerState(RDX_WRIST_ACTIVATE);
}
```

### Medida de exposición luminosa y establecimiento del umbral

Para establecer el encendido/apagado de la barra de LEDs según el nivel de exposición luminosa, se realiza la siguiente ecuación:

```C
for (int i = 0; i < LIGHT_SENSOR_STEPS; i++) {
  is_step_active = (light > LIGHT_SENSOR_RANGE * (light_steps[i].step_size / LIGHT_SENSOR_STEPS) + LIGHT_SENSOR_NOISE);
  ...
}
```

Donde las constantes son:
* `LIGHT_SENSOR_NOISE = 20` es el valor mínimo que alcanza la lectura del sensor, al reducir completamente la exposición lumínica, por lo que se asume que se trata de ruido eléctrico que inserta el dispositivo.
* `LIGHT_SENSOR_RANGE = 1020 - LIGHT_SENSOR_NOISE` es la magnitud del rango que puede tener la exposición lumínica. Debido a que el sensor alcanza la lectura de 1020 en exposición máxima, el rango será la diferencia entre este máximo y el mínimo, para calcular el tamaño de cada "step" o nivel.
* `LIGHT_SENSOR_STEPS = 6` es la cantidad de "steps" o niveles que corresponde a los LEDs de la barra que han de iluminarse. Se utiliza en la ecuación para saber la "fracción" del rango a la que corresponde cada nivel. Dicha fracción es `light_steps[i].step_size / LIGHT_SENSOR_STEPS` y, como se observa en el siguiente mapping, se realiza una cuantificación por redondeo [ref]

```C
t_light_step light_steps[LIGHT_SENSOR_STEPS] = {
  {0.5, PIN_INT_LED_BAR_0},
  {1.5, PIN_INT_LED_BAR_1},
  {2.5, PIN_INT_LED_BAR_2},
  {3.5, PIN_INT_LED_BAR_3},
  {4.5, PIN_INT_LED_BAR_4},
  {5.5, PIN_INT_LED_BAR_5}
};
```


### Detección del movimiento de muñeca

Para detectar el movimiento de muñeca y cambiar el estado de funcionamiento del wearable en la segunda parte del entregable, se utiliza el acelerómetro ADXL-335. Debido a la forma en la que se ha cosido el acelerómetro en la muñequera, se han de detectar los movimientos en el eje Y del acelerómetro. La tarea (_task_) que realiza las lecturas se ejecuta cada 200ms (2 ticks).

Sin embargo se ha de resaltar la estrategia que se ha utilizado: se ha creado un registro "histórico" o de medidas pasadas de longitud 3, de tal forma que se compare la medida actual con las tres últimas (200ms, 400ms y 600ms atrás) y, de haber una diferencia considerable en la aceleración en cualquiera de los tres casos, se procede a dicho cambio de estado. Esto ayuda a detectar tanto movimientos rápidos rápidamente (en máximo 200ms) como movimientos más lentos (que puedan "acumular" esta "diferencia considerable" en un plazo de hasta 600ms).


### Estructura de ficheros

Ambos proyectos se componen de los siguientes elementos:

* **`sketch_project.ino`**: Es el eje del proyecto: contiene las definiciones de las funciones `setup()` y `loop()`, al igual que la lógica de centralización del estado.
* **`task_<task-name>.ino`**: Cada "tarea" que se instancia para el scheduler se ha separado en un fichero. Puede estar compuesto de 3 llamadas:
  * `void setup<TaskName>(void)`: (opcional) se llama desde `setup()` y simplemente inicializa el hardware a utilizarse (i.e. `pinMode()`)
  * `void task<TaskName>(void)`: La ejecución de la tarea en sí que ejecuta `scheduler.h`
  * `void reset<TaskName>(void)`: (opcional) la restauración al estado inicial/inactivo de la tarea (i.e. apagado de LEDs), que se suele llamar en conjunto con el `scheduler_deactivate(task)` de la misma.
* **`scheduler`**: Con sus respectivos ficheros `.c` y `.h`, contiene el código de la librería realizada.
* **`sketch.h`**: Contiene la declaración de los tipos utilizados para el estado (y su centralización) así como la declaración de las funciones de cada task, con el fin de evitar un fichero `.h` para cada task.
* **`hardware.h`**: Contiene las constantes que se mapean a la interfaz hardware del wearable (i.e. pines). _Nota: Puede tener algunos pines incorrectos que aún no se hayan utilizado_


&nbsp;
## Posibles mejoras a la implementación

* Como se ha mencionado, `scheduler.h` está realizada en estructuras C, que no pueden hacer cumplir restricciones de acceso a propiedades, como podrían las clases de C++. Esto permitiría construir una interfaz del scheduler mucho más robusta y menos dependiente de la correcta implementación por parte del desarrollador que la consuma, además de facilitar un uso que provea una mejor legibilidad.
* Igualmente, `scheduler.h` guarda las _tasks_ en un array de longitud fija, lo cual es menos flexible que utilizar, por ejemplo, listas linkadas o inclusive "dynamic allocation" bajo demanda.
* Por otra parte, la implementación del estado y sus conmutaciónes se pueden ver beneficiadas del mismo control de acceso que ofrecen las clases de C++. Se puede observar también que, debido a que C/C++ son lenguajes de tipado estricto (a diferencia de JavaScript), resulta difícil implementar el patrón _flux_ de forma flexible, ya que la conmutación de estado no puede realizarse de forma dinámica (proveer parámetros) mediante un sólo metodo; teniendo que definir diversos métodos de conmutación según el atributo del estado a conmutar.



## Referencias

* **[Repositorio](https://github.com/tamanakid/hardware-iot-repo)**
* **[Manejo del Buzzer: Documentación de Lilypad](https://learn.sparkfun.com/tutorials/lilypad-protosnap-plus-activity-guide/5-play-a-tune)**
* **[Datasheet: Acelerómetro ADXL-335](https://www.analog.com/media/en/technical-documentation/data-sheets/ADXL335.pdf)**
* **[Estudio e implementación de un ADC/DAC Sigma-Delta](https://core.ac.uk/download/pdf/46111717.pdf#page=25)** - Conversión A/D: Cuantificación por truncamiento o redondeo (p.10, sección 2.2.2.1 - Error de cuantificación)

