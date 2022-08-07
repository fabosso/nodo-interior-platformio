/**
    Header que contiene rutinas relevantes para el accionar de las alertas.
    @file constants.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

int totalPitidos = 3;           // Total de pitidos por alarma.
int tiempoPitido = 133;         // Tiempo de cada pitido del buzzer [ms].
bool resetAlert = false;        // Flag responsable de resetear pitidosRestantes.
int pitidosRestantes = 0;       // Variable que contiene los pitidos restantes.

/**
    blockingAlert() se encarga de realizar una subrutina bloqueante de alerta,
    en base a una cantidad de tiempo y de pitidos dados como parámetros.
    Esta función se utiliza cuando el programa presenta algún tipo de fallo.
    Realiza una serie de 3 pitidos en donde cada pitido (y no-pitido) del buzzer dura 133 ms.
    @param tiempo Tiempo que dura cada pitido.
    @param pitidos Cantidad de pitidos.
*/
void blockingAlert(int tiempo, int pitidos) {
    for (int i = 0; i < pitidos * 2 + 1; i++) {
        digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN));
        delay(tiempo);
    }
}

/**
    startAlert() se encarga de inicializar la subrutina no bloqueante
    de alerta, en base a una cantidad de tiempo y de pitidos dados como parámetros.
    Por ejemplo:
        startAlert(133, 3);
    Realiza una serie de 3 pitidos en donde cada pitido (y no-pitido) del buzzer dura 133 ms.
    @param tiempo Tiempo que dura cada pitido.
    @param pitidos Cantidad de pitidos.
*/
void startAlert(int tiempo, int pitidos) {
    if (resetAlert && pitidosRestantes == 0) {
        return;
    } else {
        tiempoPitido = tiempo;
        totalPitidos = pitidos;
        resetAlert = true;
    }
}
