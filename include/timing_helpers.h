/**
    Header que contiene funcionalidades relacionadas al timing.
    @file timing_helpers.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    runEvery() utiliza una variable estática unsigned long para determinar
    si pasaron determinada cantidad de milisegundos desde la última vez que ingresó.
    Además, tiene un slot que permite escalar esta tarea.
    Por ejemplo:
        currentMillis = 32678
        interval = 1000
        slot = 1
    Devuelve:
        false (mientras currentMillis esté en el rango 32678 - 33677)
        true (cuando currentMillis sea 33678 o más)
    Antes de devolver true, se encarga de actualizar previousMillis del slot correspondiente.
    La condición booleana debería utilizarse dentro de un condicional para que esta función tenga sentido.
    Por ejemplo, el siguiente código:
        if(runEvery(sec2ms(2), 1)) {
            Serial.println("Hola");
        }
    Imprime por pantalla "Hola" cada 2 segundos.
    @param interval Intervalo a esperar entre tarea y tarea.
    @param slot Slot de timing seleccionado (permite múltiples "timers").
    @return true o false, dependiendo de si ya es momento de repetir la tarea.
*/
boolean runEvery(unsigned long interval, int slot) {
    static unsigned long previousMillis[TIMING_SLOTS] = {0};
    unsigned long currentMillis = millis();

    if (slot <= TIMING_SLOTS) {
        if (currentMillis - previousMillis[slot-1] >= interval) {
            previousMillis[slot-1] = currentMillis;
            return true;
        } else {
            return false;
        }
    } else {
        #if DEBUG_LEVEL >= 1
            Serial.println("TIMING_SLOTS mal configurado!");
            Serial.print("Slot ingresado: ");
            Serial.println(slot);
            Serial.print("Slots reservados: ");
            Serial.println(TIMING_SLOTS + 1);
        #endif
        return false;
    }
}

/**
    sec2ms() se encarga de convertir segundos a milisegundos.
    @param seconds Segundos a convertir.
    @return Milisegundos casteados a unsigned long.
*/
unsigned long sec2ms(int seconds) {
    return ((unsigned long)seconds) * 1000;
}

/*
    scanTime() se encarga de medir el tiempo de scan, es decir,
    el tiempo que le toma recorrer un loop() entero, y de imprimirlo
    por pantalla en caso que se "bata" un nuevo récord.
*/
void scanTime() {
    static unsigned long previousMillis = 0;
    unsigned long currentMillis = millis();
    static unsigned long longestInterval = 0;

    if (currentMillis - previousMillis > longestInterval /*&& previousMillis != 0*/) {
        longestInterval = currentMillis - previousMillis;
        Serial.print("** New scan time: ");
        Serial.print(longestInterval);
        Serial.println(" ms **");
    }

    previousMillis = currentMillis;
}