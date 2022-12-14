/**
    Header que contiene funcionalidades referidas al módulo SX1278.
    @file LoRa_helpers.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.2 29/03/2021
*/

/*
    onRecieve() es la función por interrupción que se llama cuando
    existen datos en el buffer LoRa.
*/
void onReceive(int packetSize) {
    // Si el tamaño del paquete entrante es nulo,
    // o si es superior al tamaño reservado para la string incomingFull
    // salir de la subrutina.
    if (packetSize == 0 || packetSize > INCOMING_FULL_MAX_SIZE) {
        return;
    }

    // No se puede utilizar readString() en un callback.
    // Se añaden los bytes uno por uno.
    while (LoRa.available()) {
        incomingFull += (char)LoRa.read();
    }

    // Se levanta un flag de finalización de lectura LoRa.
    incomingFullComplete = true;
}

void downlinkObserver() {
    if (incomingFullComplete) {
        // Extraer el delimitador ">" para diferenciar el ID del payload.
        int delimiter = incomingFull.indexOf(greaterSign);

        // Obtener el ID de receptor.
        receiverStr = incomingFull.substring(1, delimiter);
        int receiverID = receiverStr.toInt();
        #if DEBUG_LEVEL >= 2
            Serial.print("Receiver: ");
            Serial.println(receiverID);
        #endif

        // Si el ID del receptor coincide con nuestro ID o si es un broadcast:
        if (receiverID == DEVICE_ID || receiverID == BROADCAST_ID) {
            // Obtiene el payload entrante.
            // incomingFull típico del uplink LoRa:
            // <10009>daytime 
            // incomingPayload pasaría a ser:
            // daytime
            incomingPayload = incomingFull.substring(delimiter + 1);
            #if DEBUG_LEVEL >= 2
                Serial.println("ID coincide!");
            #endif
        } else if (receiverID == EXTERIOR_ID) {
            // incomingFull típico del nodo exterior:
            // <20009>current=0.65&raindrops=1&gas=123.51/150&lat=-34.57475&lng=58.43552&alt=15
            equalsPosition = incomingFull.indexOf(equalSign);
            ampersandPosition = incomingFull.indexOf(ampersandSign);
            if (equalsPosition != -1 && ampersandPosition != -1) {
                currentStr = incomingFull.substring(equalsPosition + 1, ampersandPosition);
                currentBuffer = currentStr.toFloat();
            }
            ampersandPosition = incomingFull.indexOf(ampersandSign, ampersandPosition + 1);
            equalsPosition = incomingFull.indexOf(equalSign, ampersandPosition);
            slashPosition = incomingFull.indexOf(slashSign, equalsPosition);
            if (equalsPosition != -1 && slashPosition != -1) {
                gasStr = incomingFull.substring(equalsPosition + 1, slashPosition);
                gasBuffer = gasStr.toFloat();
            }
            #if DEBUG_LEVEL >= 2
                Serial.println("Nodo exterior!");
            #endif
        } else {
            #if DEBUG_LEVEL >= 2
                Serial.println("Descartado por ID!");
            #endif
        }

        // Limpiar variables.
        incomingFullComplete = false;
        incomingFull = "";
        receiverStr = "";
    }
}

/**
    LoRaInitialize() inicializa el módulo SX1278 con:
        - la frecuencia y la palabra de sincronización indicados en constants.h
        - los pines indicados en pinout.h,
    Si por algún motivo fallara, "cuelga" al programa.
*/
void LoRaInitialize() {
    LoRa.setPins(NSS_PIN, RESET_PIN, DIO0_PIN);

    if (!LoRa.begin(LORA_FREQ)) {
        Serial.println("Starting LoRa failed!");
        blockingAlert(2000, 10);
        while (1);
    }
    LoRa.setSyncWord(LORA_SYNC_WORD);
    LoRa.onReceive(onReceive);
    LoRa.receive();

    #if DEBUG_LEVEL >= 1
        Serial.println("LoRa initialized OK.");
    #endif
}

/**
    composeLoRaPayload() se encarga de crear la string de carga útil de LoRa,
    a partir de los estados actuales de los sensores.
    Por ejemplo, si:
        DEVICE_ID = 10009
        volts = {220.00, 230.00}
        temps = {24.00, 25.00}
        status = "S"
    Entonces, esta función sobreescribe la String a retornar con:
        "<10009>voltage=225.00&temperature=24.50&status=S"
    @param volts Array con los valores de medición de tensión.
    @param temps Array con los valores de medición de temperatura.
    @param status Estado de la cabina.
    @param &rtn Dirección de memoria de la String a componer.
*/
void composeLoRaPayload(float volts[], float temps[], bool emergency, String status, String& rtn) {
    // Payload LoRA = vector de bytes transmitidos en forma FIFO.
    // | Dev ID | Tensión | Temperatura | Status |
    rtn = "<";
    #ifdef DEVICE_ID
        rtn += ((int)DEVICE_ID);
    #else
        rtn += "***";
    #endif
    rtn += ">";

    rtn += "voltage";
    rtn += "=";
    rtn += compressArray(volts, ARRAY_SIZE);

    rtn += "&";
    rtn += "temperature";
    rtn += "=";
    rtn += compressArray(temps, ARRAY_SIZE);

    rtn += "&";
    rtn += "status";
    rtn += "=";
    if (emergency) {
        rtn += "F";
    } else {
        rtn += status;
    }
}

void composeUSBPayload(float volts[], float temps[], bool emergency, float current, float gas, String& rtn) {
    // Payload USB = vector de bytes transmitidos en forma FIFO.
    // | Tensión | Temperatura | Emergencia | Corriente | Combustible |
    rtn = "USB: ";

    rtn += "voltage=";
    rtn += compressArray(volts, ARRAY_SIZE);
    rtn += ", ";

    rtn += "temperature=";
    rtn += compressArray(temps, ARRAY_SIZE);
    rtn += ", ";

    rtn += "emergency=";
    rtn += emergency ? "1" : "0";
    rtn += ", ";

    rtn += "current=";
    rtn += current;
    rtn += ", ";

    rtn += "gas=";
    rtn += gas;
}