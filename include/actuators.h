/**
    Header que contiene funcionalidades referidas a los actuadores conectados.
    @file actuators.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    alertObserver() se encarga de observar el estado de la variable resetAlert:
    si existe un pedido de iniciar la alerta, actualiza pitidosRestantes
    en base a totalPitidos (configurado por startAlert()) y baja el flag de pedido.
    Luego, si existen pedidos restantes, los realiza en base a tiempoPitido (configurado
    por startAlert()).
*/
void alertObserver() {
    if (resetAlert && pitidosRestantes == 0) {
        pitidosRestantes = totalPitidos;
        resetAlert = false;
    }
    if (pitidosRestantes > 0) {
        if(runEvery(tiempoPitido, 3)) {
            digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
            if (digitalRead(BUZZER_PIN) == BUZZER_INACTIVO) {
                pitidosRestantes--;
            }
        }
    }
}

/**
    LoRaCmdObserver() se encarga de observar el estado de la variable incomingPayload.
    Si la variable está vacía, sale de la función.
    Si el comando existe dentro del array de comandos conocidos, ejecuta cierta acción.
    Incluso si no existiera, limpia incomingPayload.
*/
void LoRaCmdObserver() {
    if (incomingPayload == "") {
        return;
    } else {
        #if DEBUG_LEVEL >= 1
            Serial.print("Quiero hacer esto >> ");
            Serial.println(incomingPayload);
        #endif
        if (incomingPayload == knownCommands[0]) {          // knownCommands[0]: startAlert
            startAlert(750, 10);
        } else if (incomingPayload == knownCommands[1]) {   // knownCommands[1]: dayttime
            dayTime = true;
        } else if (incomingPayload == knownCommands[2]) {   // knownCommands[2]: nighttime
            dayTime = false;
        } else {
            #if DEBUG_LEVEL >= 1
                Serial.println("Descartado por payload incorrecto!");
            #endif
        }
        incomingPayload = "";
    }
}

/**
    lightsObserver() se encarga de observar el estado de dos flags: dayTime y doorOpen.
    Si es de día, enciende la luz.
    Si es de noche y la puerta está abierta, apaga la luz. Si no, la enciende.
*/
void lightsObserver() {
    if (dayTime) {
        digitalWrite(RELE_PIN, LUZ_ENCENDIDA);
    } else {
        if (doorOpen) {
            digitalWrite(RELE_PIN, LUZ_APAGADA);
        } else {
            digitalWrite(RELE_PIN, LUZ_ENCENDIDA);
        }
    }
}

/**
    usbObserver() se encarga, primero, de diferenciar el tipo de dato entrante por USB
    (sea un reporte de sensores o un mensaje militar).
        - si es un reporte de sensores, actualiza la variable statusOutcoming a 'S', 'L' o 'F'
        - si es un mensaje militar, levanta un flag que indica que la siguiente transmisión que se
        haga por LoRa sea de tipo mensaje militar, y prepara la string para que se envíe por LoRa.
*/
void usbObserver() {
    if (incomingUSBComplete) {
        equalsPosition = incomingUSB.indexOf(equalSign);
        incomingUSBType = incomingUSB.substring(6, equalsPosition);
        if (incomingUSBType == "status") {
            // incomingUSB típico:
            // 'USB: status=S\n'
            // en este caso, equalsPosition sería 12
            statusOutcoming = incomingUSB.substring(equalsPosition + 1, equalsPosition + 2);
        } else {
            // incomingUSB típico:
            // 'USB: nro=13&o=2&d=3&cl=1&p=1&ci=0&e=1&m=xxx'
            outcomingFull = "<";
            #ifdef DEVICE_ID
                outcomingFull += ((int)DEVICE_ID);
            #else
                outcomingFull += "***";
            #endif
            outcomingFull += ">";
            outcomingFull += incomingUSB.substring(6);
            outcomingMM = true;
        }
        incomingUSBComplete = false;
        incomingUSB = "";
    }
}
