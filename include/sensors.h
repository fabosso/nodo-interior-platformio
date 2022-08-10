/**
    Header que contiene funcionalidades referidas a los sensores conectados.
    @file sensors.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    refreshAllSensors() se encarga de pedir el refresco de todos los sensores,
    poniendo cada uno de los flags del vector refreshRequested en true.
    Depende directamente del valor definido SENSORS_QTY (ver constants.h).
*/
void refreshAllSensors() {
    for (int i = 0; i < SENSORS_QTY; i++) {
        refreshRequested[i] = true;
    }
    #if DEBUG_LEVEL >= 2
        Serial.println("Refrescando sensores!");
    #endif
}

/**
    stopRefreshingAllSensors() se encarga de parar el refresco de todos los sensores,
    poniendo cada uno de los flags del vector refreshRequested en false.
    Depende directamente del valor definido SENSORS_QTY (ver constants.h).
*/
void stopRefreshingAllSensors() {
    for (int i = 0; i < SENSORS_QTY; i++) {
        refreshRequested[i] = false;
    }
    #if DEBUG_LEVEL >= 2
        Serial.println("Abandonando refrescos!");
    #endif
}

/**
    getNewVoltage() se encarga de agregar un nuevo valor en el array de medición de tensión.
    Luego de hacerlo, baja el flag correspondiente en refreshRequested.
*/

void getNewVoltage() {
    float newVoltage = 0.0;
    if (index < ARRAY_SIZE) {
        #ifndef TENSION_MOCK
            eMon.calcVI(EMON_CROSSINGS, EMON_TIMEOUT);
            newVoltage = eMon.Vrms;
        #else
            newVoltage = TENSION_MOCK + random(300) / 100.0;
        #endif
        voltages[index] = newVoltage;
        #if DEBUG_LEVEL >= 3
            Serial.print("Nueva tension: ");
            Serial.println(newVoltage);
        #endif
    }
    refreshRequested[0] = false;
}

/**
    getNewTemperature() se encarga de agregar un nuevo valor en el array de medición de temperatura.
    Luego de hacerlo, baja el flag correspondiente en refreshRequested.
*/
void getNewTemperature() {
    float newTemperature = 0.0;
    if (index < ARRAY_SIZE) {
        #ifndef TEMPERATURA_MOCK
            sensorDS18B20.requestTemperatures();
            newTemperature = sensorDS18B20.getTempCByIndex(0);
        #else
            newTemperature = TEMPERATURA_MOCK + random(300) / 100.0;
        #endif
        temperatures[index] = newTemperature;
    }
    #if DEBUG_LEVEL >= 3
        Serial.print("Nueva temperatura: ");
        Serial.println(newTemperature);
    #endif
    refreshRequested[1] = false;
}

/**
    callbackPuerta() se encarga de pollear el estado del pin del sensor de puerta,
    almacenando el valor en doorOpen.
*/
void callbackPuerta() {
    #ifndef PUERTA_MOCK
        #if PUERTA_ACTIVA == HIGH
            doorOpen = digitalRead(PUERTA_PIN);
        #else
            doorOpen = !digitalRead(PUERTA_PIN);
        #endif
    #else
        doorOpen = PUERTA_MOCK;
    #endif
}