#include <xc.h>
#include <stdint.h>

// CONFIGURACIÓN DE FUSES PARA PIC16F877A
#pragma config FOSC = XT        // Oscilador XT (cristal de 4 MHz)
#pragma config WDTE = OFF       // Watchdog Timer deshabilitado
#pragma config PWRTE = ON       // Power-up Timer habilitado
#pragma config BOREN = ON       // Reset por bajo voltaje habilitado
#pragma config LVP = OFF        // Programación en bajo voltaje deshabilitada
#pragma config CPD = OFF        // Protección de datos deshabilitada
#pragma config WRT = OFF        // Protección de escritura deshabilitada
#pragma config CP = OFF         // Protección de código deshabilitada

#define _XTAL_FREQ 4000000UL    // Frecuencia de reloj (4 MHz)

// ---------------- Patrones para la secuencia normal (auto fantástico con 1 LED) ----------------
// PORTB: RB7–RB0 (un solo LED moviéndose en orden inverso)
const uint8_t patrones_portb[] = {
    0x80, 0x40, 0x20, 0x10,  // RB7, RB6, RB5, RB4
    0x08, 0x04, 0x02, 0x01,  // RB3, RB2, RB1, RB0
    0x00, 0x00, 0x00, 0x00   // Transición suave a PORTD
};

// PORTD: RD7–RD4 (un solo LED moviéndose en orden inverso)
const uint8_t patrones_portd[] = {
    0x00, 0x00, 0x00, 0x00,   // PORTB activo
    0x00, 0x00, 0x00, 0x00,   // PORTB activo
    0x80, 0x40, 0x20, 0x10    // RD7, RD6, RD5, RD4
};

// ---------------- Patrones para la secuencia personalizada (pares e impares) ----------------
// PORTB: LEDs pares e impares
const uint8_t secuencia_personalizada_portb[] = {
    0x55, 0xAA, 0x55, 0xAA,  // Pares (01010101), Impares (10101010)
    0x55, 0xAA, 0x55, 0xAA,
    0x55, 0xAA, 0x00, 0x00
};

// PORTD: LEDs pares e impares
const uint8_t secuencia_personalizada_portd[] = {
    0x50, 0xA0, 0x50, 0xA0,  // Pares (RD4+RD6), Impares (RD5+RD7)
    0x50, 0xA0, 0x50, 0xA0,
    0x50, 0xA0, 0x50, 0xA0
};

// ---------------- Prototipos ----------------
void ejecutar_secuencia_normal(void);
void ejecutar_secuencia_personalizada(void);

// ---------------- Programa principal ----------------
void main(void) {
    CMCON = 0x07;    // Deshabilitar comparadores (necesario en algunos PICs)
    ADCON1 = 0x06;   // Configurar pines como digitales
    TRISB = 0x00;    // PORTB como salida (RB0–RB7)
    TRISD = 0x01;    // RD7–RD4 salida, RD0 entrada (pulsador)
    PORTB = 0x00;
    PORTD = 0x00;

    while(1) {
        // Validación más rápida del pulsador - chequeo en cada paso
        if (PORTDbits.RD0 == 1) {
            ejecutar_secuencia_normal();
        } else {
            ejecutar_secuencia_personalizada();
        }
    }
}

// ---------------- Funciones de secuencias ----------------
void ejecutar_secuencia_normal(void) {
    // Dirección: RB7→RB0→RD7→RD4 (hacia la derecha en el sentido del circuito)
    for (uint8_t i = 0; i < 12; i++) {
        if (PORTDbits.RD0 == 0) return;  // Salir inmediatamente si cambia el pulsador
        PORTB = patrones_portb[i];
        PORTD = patrones_portd[i];
        __delay_ms(100);
    }
    
    // Dirección inversa: RD4→RD7→RB0→RB7 (hacia la izquierda en el sentido del circuito)
    for (uint8_t i = 11; i != 0xFF; i--) {
        if (PORTDbits.RD0 == 0) return;  // Salir inmediatamente si cambia el pulsador
        PORTB = patrones_portb[i];
        PORTD = patrones_portd[i];
        __delay_ms(100);
    }
}

void ejecutar_secuencia_personalizada(void) {
    // Secuencia de pares e impares (alternando)
    for (uint8_t i = 0; i < 8; i++) {
        if (PORTDbits.RD0 == 1) return;  // Salir inmediatamente si cambia el pulsador
        PORTB = secuencia_personalizada_portb[i];
        PORTD = secuencia_personalizada_portd[i];
        __delay_ms(150);
    }
}