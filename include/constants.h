/**
    Header que contiene constantes relevantes para el accionar del programa principal.
    @file constants.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/// Comunicación serial.
#define DEBUG_LEVEL 0            // Nivel de debug (0 para comunicarse sólo con SIGEFA).
#define SERIAL_BPS 9600          // Bitrate de las comunicaciones por puerto serial.
#define SERIAL_REPORT_TIMEOUT 10 // Intervalo de tiempo entre cada reporte por puerto serial.

/// LoRa.
#define LORA_FREQ 433175000                                                         // Frecuencia de la transmisión LoRa (en Hz).
#define DEVICE_ID 10009                                                             // Identificador de este nodo.
#define EXTERIOR_ID (DEVICE_ID + 10000)                                             // Identificador del nodo exterior.
#define BROADCAST_ID (DEVICE_ID - DEVICE_ID % 10000 + 9999)                         // ID broadcast para este tipo de nodo.
#define DEVICE_ID_MAX_SIZE 6                                                        // Tamaño máximo que se espera para cada DEVICE_ID entrante.
#define INCOMING_PAYLOAD_MAX_SIZE 100                                               // Tamaño máximo esperado del payload LoRa entrante.
#define INCOMING_FULL_MAX_SIZE (INCOMING_PAYLOAD_MAX_SIZE + DEVICE_ID_MAX_SIZE + 2) // Tamaño máximo esperado del mensaje entrante.
#define MAX_SIZE_OUTCOMING_LORA_REPORT 200                                          // Tamaño máximo esperado del payload LoRa saliente.
#define KNOWN_COMMANDS_SIZE 3                                                       // Cantidad de comandos LoRa conocidos.
#define LORA_TIMEOUT 20                                                             // Tiempo entre cada mensaje LoRa.
#define LORA_SYNC_WORD 0x34                                                         // Palabra de sincronización LoRa.

/// Watchdog.
#define USE_WATCHDOG_TMR TRUE
#define WATCHDOG_TMR 8

/// Arrays.
#define SENSORS_QTY 2          // Cantidad de sensores conectados.
#define TIMEOUT_READ_SENSORS 2 // Tiempo entre mediciones.
#define ARRAY_SIZE (LORA_TIMEOUT / TIMEOUT_READ_SENSORS + 3)
#define TIMING_SLOTS 4 // Cantidad de slots necesarios de timing (ver timing_helpers.h)

// Sensor de tensión.
#define EMON_CROSSINGS 20 // Cantidad de semi-ondas muestreadas para medir tensión y/o corriente.
#define EMON_TIMEOUT 1000 // Timeout de la rutina calcVI (en ms).

// Sensor de puerta abierta.
#define PUERTA_ABIERTA HIGH // Señal entrante cuando la puerta está abierta.
#define PUERTA_CERRADA LOW  // Señal entrante cuando la puerta está cerrada.

// Sensor de botón antipánico.
#define ANTIPANICO_ACTIVO LOW    // Señal entrante cuando el botón está presionado.
#define ANTIPANICO_INACTIVO HIGH // Señal entrante cuando el botón no está presionado.

// Actuador relé.
#define RELE_ACTIVO LOW           // definido según jumper de módulo
#define RELE_MODO NORMALLY_CLOSED // definido según conexión al relé
#if (RELE_ACTIVO == LOW && RELE_MODO == NORMALLY_CLOSED) || (RELE_ACTIVO == HIGH && RELE_MODO == NORMALLY_OPEN)
    #define LUZ_ENCENDIDA HIGH
    #define LUZ_APAGADA LOW
#else
    #define LUZ_ENCENDIDA LOW
    #define LUZ_APAGADA HIGH
#endif

// Actuador buzzer.
#define BUZZER_ACTIVO HIGH
#define BUZZER_INACTIVO LOW

/// Valores mock.
// #define TENSION_MOCK 223.11      // Tensión de prueba.
// #define TEMPERATURA_MOCK 23.11   // Temperatura de prueba.
// #define ANTIPANICO_MOCK 0        // Antipánico de prueba.
// #define PUERTA_MOCK 0            // Puerta de prueba.
