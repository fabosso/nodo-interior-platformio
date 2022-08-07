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
            voltages[index] = newVoltage;
        #else
            newVoltage = TENSION_MOCK + random(300) / 100.0;
            voltages[index] = newVoltage;
        #endif
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
            temperatures[index] = newTemperature;
        #else
            temperatures[index] = TEMPERATURA_MOCK + random(300) / 100.0;
        #endif
    }
    #if DEBUG_LEVEL >= 3
        Serial.print("Nueva temperatura: ");
        Serial.println(newTemperature);
    #endif
    refreshRequested[1] = false;
}

/**
    callbackPresencia() se encarga de pollear el estado del pin del sensor de presencia,
    en base al estado del flag presenciaLast.
    Cuando detecta un cambio de estado del sensor de presencia, pone en true el flag presenciaChanged.
*/
void callbackPresencia() {
    float dist = 0.0;
    dist = sonar.ping_median(PING_SAMPLES);
    dist = sonar.convert_cm(dist);
    if (dist < MAX_DISTANCE) {
        presenciaDetected = true;
    }
}