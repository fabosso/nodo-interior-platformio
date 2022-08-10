/**
    Header que contiene funcionalidades referidas a los actuadores conectados.
    @file actuators.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    callbackAlert() se encarga de consultar el estado de la variable resetAlert:
    si existe un pedido de iniciar la alerta, actualiza pitidosRestantes
    en base a totalPitidos (configurado por startAlert()) y baja el flag de pedido.
    Luego, si existen pedidos restantes, los realiza en base a tiempoPitido (configurado
    por startAlert()).
*/
void callbackAlert() {
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
    callbackLoRaCommand() se encarga de consultar el estado de la variable incomingPayload.
    Si la variable está vacía, sale de la función.
    Si el comando existe dentro del array de comandos conocidos, ejecuta cierta acción.
    Incluso si no existiera, limpia incomingPayload.
*/
void callbackLoRaCommand() {
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
    callbackLights() se encarga de chequear el estado de dos flags: dayTime y doorOpen.
    Si es de día, saltea el resto de la función (porque el relé estará siempre en modo activo).
    Si es de noche y la puerta está abierta, desactiva el relé. Si no, lo activa.
*/
void callbackLights() {
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
