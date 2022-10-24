/**
    Rutina principal que obtiene los valores sensados y los modula
    y transmite sobre LoRa@433MHz en una string especialmente diseñada.
    @file nodo-sisicic.ino
    @author Franco Abosso
    @author Julio Donadello
    @version 1.1 29/03/2021
*/

/// Headers iniciales (preceden a la declaración de variables).

// Header que contiene constantes relevantes al accionar de este programa.
#include "constants.h"          // Biblioteca propia.

// Bibliotecas necesarias para manejar al SX1278.
#include <SPI.h>                // https://www.arduino.cc/en/reference/SPI
#include <LoRa.h>               // https://github.com/sandeepmistry/arduino-LoRa

// Bibliotecas necesarias para manejar al DS18B20.
#include <OneWire.h>            // https://www.pjrc.com/teensy/td_libs_OneWire.html
#include <DallasTemperature.h>  // https://www.milesburton.com/Dallas_Temperature_Control_Library

// Biblioteca necesaria para calcular los valores RMS de los sensores de tensión y corriente.
#include <EmonLib.h>            // https://learn.openenergymonitor.org/electricity-monitoring/ctac/how-to-build-an-arduino-energy-monitor

// Biblioteca necesaria para reservar espacios de memoria fijos para las Strings utilizadas.
#include <StringReserveCheck.h> // https://www.forward.com.au/pfod/ArduinoProgramming/ArduinoStrings/index.html

// Biblioteca necesaria para utilizar el watchdog timer. 
#include <avr/wdt.h>            // https://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html

/// Declaración de variables globales.

/**
    voltages es un array de floats que contiene los valores de tensión medidos entre cada
    transmisión LoRa. El tamaño del array depende del intervalo de tiempo entre cada transmisión
    LoRa (TIMEOUT_LORA) y el intervalo de tiempo entre cada medición (TIMEOUT_READ_SENSORS).
    Una vez realizada la transmisión, todos los valores de este array vuelven a ponerse en 0.
*/
float voltages[ARRAY_SIZE] = {0.0};

/**
    temperatures es un array de floats que contiene los valores de temperatura medidos entre cada
    transmisión LoRa. El tamaño del array depende del intervalo de tiempo entre cada transmisión
    LoRa y el intervalo de tiempo entre cada medición.
    Una vez realizada la transmisión, todos los valores de este array vuelven a ponerse en 0.
*/
float temperatures[ARRAY_SIZE] = {0.0};

/**
    currentBuffer es un float que contiene el último valor de corriente reportado por el
    nodo exterior emparejado a este nodo.
*/
float currentBuffer = 0.0;

/**
    gasBuffer es un float que contiene el último valor de nivel de combustible reportado por el
    nodo exterior emparejado a este nodo.
*/
float gasBuffer = 0.0;

/**
    index es una variable de tipo int utilizado para recorrer los arrays de medición.
    Una vez realizada la transmisión, vuelve a ponerse en 0.
*/
int index = 0;

/**
    dayTime es un flag que se pone en true o en false remotamente.
    Si dayTime es false, significa que hay luz ambiental exterior, por lo que no
    es necesario apagar la luz cuando se detecte la apertura de la puerta.
*/
bool dayTime = true;

/**
    doorOpen es un flag que se pone en true o en false en base al estado de la puerta.
    Si doorOpen es true, significa que la puerta está abierta.
*/
bool doorOpen = false;

/**
    emergency es un flag que se pone en true o en false en base al estado del 
    botón antipánico. Si emergency es true, significa que el botón ha sido presionado.
*/
bool emergency = false;

/**
    refreshRequested contiene SENSORS_QTY variables booleanas que representan la necesidad
    de refrescar los valores de los arrays de medición. Estos tienen un orden arbitrario:
    { Tensión, Temperatura }
    Una vez refrescado, cada uno de estos booleanos vuelve a ponerse en false.
*/
bool refreshRequested[SENSORS_QTY] = {false};

/**
    outcomingFull es una string que contiene el mensaje LoRa de salida preformateado especialmente
    para que, posteriormente, el concentrador LoRa pueda decodificarla.
*/
String outcomingFull;

/**
    incomingFull es una string que contiene el mensaje LoRa de entrada, incluyendo
    el identificador de nodo.
*/
String incomingFull;

/**
    incomingFullComplete es un flag que se pone en true luego de completarse la función de interrupción
    onRecieve de LoRa, provisto que la carga útil sea una string con entre 0 y INCOMING_FULL_MAX_SIZE 
    bytes.  
*/
bool incomingFullComplete = false;

/**
    receiverStr es una string que sólo contiene el identificador de nodo
    recibido en un mensaje LoRa entrante.
*/
String receiverStr;

/**
    incomingPayload es una string que contiene sólo la carga útil del mensaje LoRa de entrada,
    utilizada sólo cuando el identificador de nodo coincide con DEVICE_ID o con BROADCAST_ID.
*/
String incomingPayload;

/**
    knownCommands es un array de Strings que contiene los comandos LoRa que
    se pueden ejecutar.
*/
const String knownCommands[KNOWN_COMMANDS_SIZE] = {
    "startAlert",   // inicia una alerta con el siguiente llamado a función: startAlert(750, 10);
    "daytime",      // alerta a la cabina que es de día.
    "nighttime"    // alerta a la cabina que es de noche.
};

/**
    statusOutgoing es una string que puede tomar uno de los siguientes valores:
    - 'S': cabina en servicio.
    - 'L': cabina en modo limitado.
    - 'F': cabina fuera de servicio.
*/
String statusOutcoming = "F";

/**
    currentStr es una string que contiene el valor de corriente actual, antes de parsearlo a float.
*/
String currentStr = "";

/**
    gasStr es una string que contiene el valor de nivel de combustible actual, antes de parsearlo a float.
*/
String gasStr = "";

/**
    equalSign es el caracter "=" almacenado en una constante.
*/
const String equalSign = "=";

/**
    slashSign es el caracter "/" almacenado en una constante.
*/
const String slashSign = "/";

/**
    ampersandSign es el caracter "&" almacenado en una constante.
*/
const String ampersandSign = "&";

/**
    greaterSign es el caracter ">" almacenado en una constante.
*/
const String greaterSign = ">";

/**
    equalsPosition es un int que contiene la posición de la cadena '=' en una cadena de texto.
*/
int equalsPosition = 0;

/**
    slashPosition es un int que contiene la posición de la cadena '/' en una cadena de texto.
*/
int slashPosition = 0;

/**
    ampersandPosition es un int que contiene la posición de la cadena '&' en una cadena de texto.
*/
int ampersandPosition = 0;

/**
    outcomingUSB es una string que contiene el mensaje USB de salida hacia el proyecto SIGEFA.
*/
String outcomingUSB = "";

/**
    incomingUSB es una string que contiene el mensaje USB de entrada desde el proyecto SIGEFA.
*/
String incomingUSB = "";

/**
    incomingUSBType es una string que contiene el prefijo del primer campo enviado por USB al
    nodo, y puede ser "status" o "nro_mm".
*/
String incomingUSBType = "";

/**
    incomingUSBComplete es un flag que se pone en true cuando se recibe un newline en el 
    mensaje USB de entrada.
*/
bool incomingUSBComplete = false;

/**
    outcomingMM es un flag que se pone en true en caso de que el siguiente mensaje a ser
    transmitido por LoRa sea un mensaje militar.
 */
bool outcomingMM = false;


/// Headers finales (proceden a la declaración de variables).

#include "pinout.h"             // Biblioteca propia.
#include "alerts.h"             // Biblioteca propia.
#include "timing_helpers.h"     // Biblioteca propia.
#include "sensors.h"            // Biblioteca propia.
#include "actuators.h"          // Biblioteca propia.
#include "decimal_helpers.h"    // Biblioteca propia.
#include "array_helpers.h"      // Biblioteca propia.
#include "LoRa_helpers.h"       // Biblioteca propia.

/// Funciones principales.

/**
    reserveMemory() reserva memoria para las Strings.
    En caso de quedarse sin memoria, alerta por puerto serial
    e inicia una alerta de falla.
*/
void reserveMemory() {
    receiverStr.reserve(DEVICE_ID_MAX_SIZE);
    currentStr.reserve(8);
    gasStr.reserve(8);
    incomingUSB.reserve(20);
    incomingUSBType.reserve(10);
    incomingPayload.reserve(INCOMING_PAYLOAD_MAX_SIZE);
    incomingFull.reserve(INCOMING_FULL_MAX_SIZE);
    outcomingUSB.reserve(100);

    if (!outcomingFull.reserve(MAX_SIZE_OUTCOMING_LORA_REPORT)) {
        #if DEBUG_LEVEL >= 1
            Serial.println("Strings out of memory!");
        #endif
        blockingAlert(133, 50);
        while (1);
    }
}

/**
    setup() lleva a cabo las siguientes tareas:
        - setea el pinout,
        - inicializa el periférico serial,
        - reserva espacios de memoria para las Strings,
        - inicializa el módulo LoRa,
        - inicializa el watchdog timer en 8 segundos.
    Si después de realizar estas tareas no se "cuelga", da inicio
    a una alerta "exitosa".
*/
void setup() {
    setupPinout();
    Serial.begin(SERIAL_BPS);
    #if DEBUG_LEVEL >= 1
        Serial.println("Nodo interior")
        Serial.println("");
        Serial.println("Puerto serial inicializado en modo debug.");
        Serial.print("Nivel de debug = ");
        Serial.println(DEBUG_LEVEL);
        Serial.print("Fecha de última compilación: ");
        Serial.print(__DATE__);
        Serial.print(" ");
        Serial.println(__TIME__);
        Serial.println();
    #endif
    reserveMemory();
    LoRaInitialize();
    startAlert(133, 3);
    #if USE_WATCHDOG_TMR == TRUE
        #if WATCHDOG_TMR >= 8 
            wdt_enable(WDTO_8S);
        #elif WATCHDOG_TMR >= 4
            wdt_enable(WDTO_4S);
        #elif WATCHDOG_TMR >= 2
            wdt_enable(WDTO_2S);
        #else
            wdt_enable(WDTO_1S);
        #endif
    #endif
}

/**
    loop() determina las tareas que cumple el programa:
        - cada LORA_TIMEOUT segundos, envía un payload LoRa y un payload USB.
        - cada TIMEOUT_READ_SENSORS segundos, refresca el estado de todas las mediciones.
        - si corresponde, mide tensión y temperatura.
        - observa el estado actual de las variables de programa y, de ser necesario, actúa:
            - emite las alertas que sean necesarias,
            - ejecuta comandos entrantes de LoRa,
            - observa el estado de la puerta,
            - observa el estado del botón antipánico,
            - observa el estado del buffer USB.
    Al finalizar el loop, resetea el watchdog timer.
    Esta función se repite hasta que se le dé un reset al programa.
*/
void loop() {
    if (runEvery(sec2ms(LORA_TIMEOUT), 1)) {
        // Deja de refrescar TODOS los sensores.
        stopRefreshingAllSensors();

        // Compone la carga útil de LoRa (en caso de que se vaya a reportar el estado de los 
        // sensores, y no haya mensajes militares a emitir).
        if (!outcomingMM) {
            composeLoRaPayload(voltages, temperatures, emergency, statusOutcoming, outcomingFull);
        }

        #if DEBUG_LEVEL >= 1
            Serial.print("Payload LoRa encolado!: ");
            Serial.println(outcomingFull);
        #endif

        // Componer y enviar paquete.
        LoRa.beginPacket();
        LoRa.print(outcomingFull);
        LoRa.endPacket();

        // Pone al módulo LoRa en modo recepción.
        LoRa.receive();

        // Inicia la alerta preestablecida.
        startAlert(133, 3);

        // Compone la carga útil USB.
        composeUSBPayload(voltages, temperatures, emergency, currentBuffer, gasBuffer, outcomingUSB);
        #if DEBUG_LEVEL > 0
            Serial.print("Payload que saldría por USB: ");
        #endif
        #if DEBUG_LEVEL >= 0
            // Escribe la carga útil USB.
            Serial.println(outcomingUSB);
        #endif

        // Reestablece los arrays de medición.
        cleanupArray(voltages, ARRAY_SIZE);
        cleanupArray(temperatures, ARRAY_SIZE);

        // Reestablece el index de los arrays de medición.
        index = 0;

        // Baja el flag de mensaje militar.
        outcomingMM = false;
    }

    if(runEvery(sec2ms(TIMEOUT_READ_SENSORS), 3)) {
        // Refresca TODOS los sensores.
        refreshAllSensors();
        // Actualiza el estado de las luces.
        lightsObserver();
        // Avanza en 1 a los índices de los arrays de medición.
        index++;
    }

    if (!resetAlert && !pitidosRestantes) {
        if (refreshRequested[0]) {
            // Obtiene un nuevo valor de tensión.
            getNewVoltage();
        }
        if (refreshRequested[1]) {
            // Obtiene un nuevo valor de temperatura.
            getNewTemperature();
        }
    }

    alertObserver();
    downlinkObserver();
    LoRaCmdObserver();
    doorObserver();
    emergencyObserver();
    usbObserver();

    #if DEBUG_LEVEL >= 3
        scanTime();
    #endif

    #if USE_WATCHDOG_TMR == TRUE 
        wdt_reset();
    #endif
}

/*
    serialEvent() es la respuesta a la interrupción por puerto serie.
    Al recibir el caracter newline (\n), se levanta un flag, incomingUSBComplete.
    incomingUSB típico:
        'USB: status=S\n'
*/
void serialEvent() {
    while (Serial.available()) {
        // obtener el nuevo caracter
        char inChar = (char)Serial.read();
        if (inChar == '\n') {
            // si el caracter entrante es un newline, levantar un flag
            incomingUSBComplete = true;
        } else {
            // sino, appendearlo a incomingUSB
            incomingUSB += inChar;
        }
    }
}