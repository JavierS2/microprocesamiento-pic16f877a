#include <xc.h>
#include <stdint.h>
#include "lcd.h"      // Usa Lcd_Init, Lcd_Clear, Lcd_Set_Cursor, Lcd_Write_Char, Lcd_Write_String

// ================== CONFIG BITS ==================
#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = ON
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// ================== DHT11 EN RA0 (PIN 2) ==================
#define DHT_PIN      RA0
#define DHT_TRIS     TRISA0

// ================== LEDS Y BUZZER ==================
#define LED_ALTO       RC0    // LED rojo: por encima del rango
#define LED_BAJO       RC1    // LED azul: por debajo del rango
#define BUZZER         RC2    // Buzzer: cualquier fuera de rango

#define LED_ALTO_TRIS  TRISC0
#define LED_BAJO_TRIS  TRISC1
#define BUZZER_TRIS    TRISC2

// ================== CONSTANTES DE RANGO ==================
#define MIN_TEMP  15
#define MAX_TEMP  25
#define MIN_HUM   40
#define MAX_HUM   80

// ================== PROTOTIPOS DHT11 ==================
uint8_t DHT_ReadByte(void);
uint8_t DHT_ReadData(uint8_t *temp, uint8_t *hum);

// ================== MAIN ==================
void main(void) {

    uint8_t temp = 0, hum = 0;

    // Desactivar entradas analógicas (incluye AN0 / RA0)
    ADCON1 = 0x07;

    // RA0 como entrada digital para DHT11
    TRISA0 = 1;

    // LCD en PORTB
    TRISB = 0x00;

    // LEDs y buzzer como salida
    LED_ALTO_TRIS = 0;
    LED_BAJO_TRIS = 0;
    BUZZER_TRIS   = 0;

    // Apagar alarmas al inicio
    LED_ALTO = 0;
    LED_BAJO = 0;
    BUZZER   = 0;

    // Inicializar LCD
    Lcd_Init();
    Lcd_Clear();

    Lcd_Set_Cursor(1,1);
    Lcd_Write_String((char *)"Iniciando...");
    __delay_ms(1200);
    Lcd_Clear();

    while (1) {
        __delay_ms(1000);

        if (DHT_ReadData(&temp, &hum)) {
            // ---- Mostrar valores en LCD ----
            Lcd_Clear();

            // Temperatura
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String((char *)"Temp:");
            Lcd_Write_Char((temp/10)+'0');
            Lcd_Write_Char((temp%10)+'0');
            Lcd_Write_String((char *)"C");

            // Humedad
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String((char *)"Humedad:");
            Lcd_Write_Char((hum/10)+'0');
            Lcd_Write_Char((hum%10)+'0');
            Lcd_Write_String((char *)"%");

            // ---- Lógica de alarmas ----
            // Apagar todo por defecto
            LED_ALTO = 0;
            LED_BAJO = 0;
            BUZZER   = 0;

            // Temperatura: MIN_TEMP?MAX_TEMP
            if (temp > MAX_TEMP) {
                LED_ALTO = 1;      // rojo: por encima
                BUZZER   = 1;
            } else if (temp < MIN_TEMP) {
                LED_BAJO = 1;      // azul: por debajo
                BUZZER   = 1;
            }

            // Humedad: MIN_HUM?MAX_HUM
            if (hum > MAX_HUM) {
                LED_ALTO = 1;      // también fuera por arriba
                BUZZER   = 1;
            } else if (hum < MIN_HUM) {
                LED_BAJO = 1;      // también fuera por abajo
                BUZZER   = 1;
            }

        } else {
            // Error/no respuesta del DHT11
            Lcd_Clear();
            Lcd_Set_Cursor(1,1);
            Lcd_Write_String((char *)"No response");
            Lcd_Set_Cursor(2,1);
            Lcd_Write_String((char *)"from DHT11");

            // Apagar alarmas en caso de error de lectura
            LED_ALTO = 0;
            LED_BAJO = 0;
            BUZZER   = 0;
        }
    }
}

uint8_t DHT_ReadByte(void) {
    uint8_t i, byte = 0;
    uint16_t timeout;

    for (i = 0; i < 8; i++) {

        timeout = 0;
        while (!DHT_PIN) {             // esperar flanco de subida
            if (++timeout > 200) return 0;
            __delay_us(1);
        }

        __delay_us(30);                // muestreo del bit

        if (DHT_PIN)
            byte |= (1 << (7 - i));

        timeout = 0;
        while (DHT_PIN) {              // esperar que termine el pulso alto
            if (++timeout > 200) break;
            __delay_us(1);
        }
    }
    return byte;
}

uint8_t DHT_ReadData(uint8_t *temp, uint8_t *hum) {
    uint8_t hum_int, hum_dec, temp_int, temp_dec, sum;
    uint16_t timeout;

    // Pulso de inicio al DHT11
    DHT_TRIS = 0;        // salida
    DHT_PIN  = 0;
    __delay_ms(20);      // mínimo 18 ms
    DHT_PIN  = 1;
    __delay_us(30);
    DHT_TRIS = 1;        // entrada (liberar línea)

    // Respuesta del sensor
    timeout = 0;
    while (DHT_PIN) {    // espera a que baje
        if (++timeout > 200) return 0;
        __delay_us(1);
    }
    timeout = 0;
    while (!DHT_PIN) {   // bajo ~80us
        if (++timeout > 200) return 0;
        __delay_us(1);
    }
    timeout = 0;
    while (DHT_PIN) {    // alto ~80us
        if (++timeout > 200) return 0;
        __delay_us(1);
    }

    // Lectura de los 5 bytes
    hum_int  = DHT_ReadByte();
    hum_dec  = DHT_ReadByte();
    temp_int = DHT_ReadByte();
    temp_dec = DHT_ReadByte();
    sum      = DHT_ReadByte();

    if ((uint8_t)(hum_int + hum_dec + temp_int + temp_dec) != sum)
        return 0;        // error de checksum

    *hum  = hum_int;     // DHT11 usa solo parte entera
    *temp = temp_int;

    return 1;
}
