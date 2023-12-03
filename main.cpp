#include <Arduino.h>
#define encoderA 2
#define encoderB 3
#define pwm 9
volatile int currentCounts = 0;
volatile float dataReception;
unsigned long starTime, currenTime = 0, pastCounts = 0;
int counts = 0;
int pulse;
float velocity(float x);
void revolutions();
float reception();

typedef union{
    float datoFloat;      // dato flotante de 4 bytes
    uint8_t datoBytes[4]; // dato entero de 4 bytes
} dato;
dato velocidad;
dato receptionBytes;

void setup(){
    pinMode(encoderA, INPUT_PULLUP); // entradas en las cuales se reflejan los pulsos de los encoder
    pinMode(encoderB, INPUT_PULLUP);
    pinMode(pwm, OUTPUT); //salida de voltaje relacionada con velocidad
    Serial.begin(9600);
    attachInterrupt(digitalPinToInterrupt(2), revolutions, CHANGE); // interrupcion configurada cada que haya un cambio en el pulsp de los encoder
    attachInterrupt(digitalPinToInterrupt(3), revolutions, CHANGE);
}
void loop(){

    if (Serial.available() > 0){//se recive un impulso unitario por el puerto serial
        dataReception = reception();
    }

    pulse = map(dataReception, 0, 5, 0, 255); // se mapea el dato recivido
    analogWrite(pwm, pulse);
    starTime = millis();

    if ((starTime - currenTime) >= 50){      //periodo de muestreo 50ms
        currenTime = starTime;               // se actualiza el tiempo transcurrido
        counts = currentCounts - pastCounts; // se resta las cuentas antiguas a las actuales para identificar cuantas cuentas hay ahora
        velocidad.datoFloat = velocity(counts * 20.0); //se convierten cuentas/ms a rad/seg
        Serial.write("start"); // se inicia el envio del dato por comnunicación serial 

        for (int i = 0; i < 4; i++){
            Serial.write(velocidad.datoBytes[i]); //se envia bit a bit el valor de la velocidad
        }

        Serial.write('\n');// se termina de enviar el dato
        pastCounts = currentCounts; //se actualizan las cuentas 
    }
}

void revolutions(){
    currentCounts++; // se cuenta cada pulso del encoder desde que empieza a funcionar el motor
}

float velocity(float x){
    return (x / 6400) * (2 * PI); //se convienten cuentas en velocidad
}
float reception(){ // se convierte el valor recivido por el puerto serial en un dato flotante
    for (int i = 0; i < 4; i++){
        receptionBytes.datoBytes[i] = Serial.read();
    }

    return receptionBytes.datoFloat;
}