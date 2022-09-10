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
            digitalWrite(RELE_PIN, RELE_ACTIVO);
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
    Si es de día, saltea el resto de la función (porque el relé estará siempre en modo activo).
    Si es de noche y la puerta está abierta, desactiva el relé. Si no, lo activa.
*/
void lightsObserver() {
    if (dayTime) {
        digitalWrite(RELE_PIN, RELE_ACTIVO);
    } else {
        if (doorOpen) {
            digitalWrite(RELE_PIN, RELE_INACTIVO);
        } else {
            digitalWrite(RELE_PIN, RELE_ACTIVO);
        }
    }
}

/**
    statusObserver() se encarga de parsear solo el caracter útil de la string de entrada
    del USB, y almacenar en statusOutcoming el estado actual de la cabina reportada
    por el proyecto SIGEFA.
*/
void statusObserver() {
    if (incomingUSBComplete) {
        // incomingUSB típico:
        // 'USB: status=S\n'
        equalsPosition = incomingUSB.indexOf(equalSign);
        statusOutcoming = incomingUSB.substring(equalsPosition + 1, equalsPosition + 2);
        incomingUSBComplete = false;
    }
}
