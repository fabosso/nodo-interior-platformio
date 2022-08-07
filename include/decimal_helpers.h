/**
    Header que contiene funciones relevantes para el redondeo decimal.
    @file decimal_helpers.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    round2decimals() redondea variables de tipo float a 2 decimales.
    Por ejemplo:
        float var = 213.27123;
        var = round2decimals(var);
    Devuelve 213.27.
    @param var Número a redondear.
    @return Número rendondeado a 2 posiciones decimales.
*/
float round2decimals(float var) {
    float value = (int)(var * 100 + 0.5);
    return (float)(value / 100);
}
