/**
    Header que contiene funciones relevantes para el accionar de los arrays.
    @file constants.h
    @author Franco Abosso
    @author Julio Donadello
    @version 1.0 29/03/2021
*/

/**
    compressArray() obtiene el promedio de los valores no nulos de un array de floats.
    Por ejemplo:
        int size = 2;
        float array[size] = {10.00, 11.00};
        getAverage(array, size);
    Devuelve: 10.50.
    @param array Arreglo de números que se quiere promediar.
    @param size Cantidad de elementos del array.
    @return Promedio de los valores no nulos del array.
*/
float compressArray(float array[], int size) {
    float average = 0;
    int zerosFound = 0;
    for (int i = 0; i < size; i++) {
        average += array[i];
        if (array[i] == 0.0) {
            zerosFound++;
        }
        #if DEBUG_LEVEL >= 5
            Serial.print(array[i]);
            Serial.print(" ");
        #endif
    }
    int nonZeroValues = size - zerosFound;
    average /= nonZeroValues;
    average = round2decimals(average);
    #if DEBUG_LEVEL >= 5
        Serial.print("Average of array: ");
        Serial.println(average);
    #endif

    return average;
}

/**
    cleanupArray() resetea todos los elementos de un array.
    @param array Arreglo de números que se quiere limpiar.
    @param size Cantidad de elementos del array.
*/
void cleanupArray(float array[], int size) {
    for (int i = 0; i < size; i++) {
        array[i] = 0.0;
    }
}
