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
    #if DEBUG_LEVEL >= 2
        Serial.println("Entering recieve mode");
    #endif

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

    // Extraer el delimitador ">" para diferenciar el ID del payload.
    int delimiter = incomingFull.indexOf(greaterThanStr);

    // Obtener el ID de receptor.
    receiverStr = incomingFull.substring(1, delimiter);
    int receiverID = receiverStr.toInt();
    #if DEBUG_LEVEL >= 1
        Serial.print("Receiver: ");
        Serial.println(receiverID);
    #endif

    // Si el ID del receptor coincide con nuestro ID o si es un broadcast:
    if (receiverID == DEVICE_ID || receiverID == BROADCAST_ID) {
        // Obtiene el payload entrante.
        incomingPayload = incomingFull.substring(delimiter + 1);
        #if DEBUG_LEVEL >= 1
            Serial.println("ID coincide!");
        #endif
    } else {
        #if DEBUG_LEVEL >= 2
            Serial.println("Descartado por ID!");
        #endif
    }

    // Limpiar strings.
    incomingFull = "";
    receiverStr = "";
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
    reserveMemory() reserva memoria para las Strings.
    En caso de quedarse sin memoria, alerta por puerto serial
    e inicia una alerta de falla
*/
void reserveMemory() {
    receiverStr.reserve(DEVICE_ID_MAX_SIZE);
    incomingPayload.reserve(INCOMING_PAYLOAD_MAX_SIZE);
    incomingFull.reserve(INCOMING_FULL_MAX_SIZE);

    if (!outcomingFull.reserve(MAX_SIZE_OUTCOMING_LORA_REPORT)) {
        #if DEBUG_LEVEL >= 1
            Serial.println("Strings out of memory!");
        #endif
        blockingAlert(133, 50);
        while (1);
    }
}

/**
    composeLoRaPayload() se encarga de crear la string de carga útil de LoRa,
    a partir de los estados actuales de los sensores.
    Por ejemplo, si:
        DEVICE_ID = 10009
        volts = {220.00, 230.00}
        temps = {24.00, 25.00}
    Entonces, esta función sobreescribe la String a retornar con:
        "<10009>voltage=225.00&temperature=24.50"
    @param volts Array con los valores de medición de tensión.
    @param temps Array con los valores de medición de temperatura.
    @param &rtn Dirección de memoria de la String a componer.
*/
void composeLoRaPayload(float volts[], float temps[], String& rtn) {
    // Payload LoRA = vector de bytes transmitidos en forma FIFO.
    // | Dev ID | Tensión | Temperatura |
    rtn = "<";
    rtn += DEVICE_ID;
    rtn += ">";

    rtn += "voltage";
    rtn += "=";
    rtn += compressArray(volts, ARRAY_SIZE);

    rtn += "&";
    rtn += "temperature";
    rtn += "=";
    rtn += compressArray(temps, ARRAY_SIZE);
}
