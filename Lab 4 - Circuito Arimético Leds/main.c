#include <xc.h>
#include <stdint.h>

// Configuración del PIC16F877A
#pragma config FOSC = XT      // Oscilador con cristal
#pragma config WDTE = OFF     // Watchdog desactivado
#pragma config PWRTE = OFF    // Power-up Timer desactivado
#pragma config BOREN = ON     // Brown-out Reset activado
#pragma config LVP = OFF      // Low Voltage Programming desactivado
#pragma config CPD = OFF      // Data Code Protection desactivado
#pragma config WRT = OFF      // Write Protection desactivado
#pragma config CP = OFF       // Code Protection desactivado

#define _XTAL_FREQ 4000000UL  // Frecuencia del cristal: 4MHz

// Prototipos de funciones
uint8_t bcd_a_decimal(uint8_t decenas, uint8_t unidades);
void decimal_a_bcd(uint16_t valor, uint8_t *miles, uint8_t *centenas, uint8_t *decenas, uint8_t *unidades);
void mostrar_bcd(uint8_t m, uint8_t c, uint8_t d, uint8_t u);

void main(void) {

    // Configurar puertos analógicos como digitales
    ADCON1 = 0x07; // Todos los pines de PORTA y PORTE como digitales
    
    // Desactivar comparadores analógicos
    CMCON  = 0x07;
    
    TRISA = 0x2F;  // RA0-RA3 y RA5 como entradas (para B decenas + bit RA5)
    TRISB = 0x00;  // PORTB completo como salida (para miles + centenas)
    TRISC = 0x0F;  // RC0-RC3 como entradas (A unidades), RC4-RC7 como salidas
    TRISD = 0x0F;  // RD0-RD3 como entradas (A decenas), RD4-RD7 como salidas
    TRISE = 0x07;  // RE0-RE2 como entradas (B unidades)

    // Inicialziación de puertos
    PORTA = 0; 
    PORTB = 0; 
    PORTC = 0; 
    PORTD = 0; 
    PORTE = 0;

    while (1) {
        // Leer decenas de A desde PORTD bits 3-0  y unidades de A desde PORTC bits 3-0.
        uint8_t decA = PORTD & 0x0F;  
        uint8_t uniA = PORTC & 0x0F;  
        
        // Convertir BCD a valor decimal
        uint8_t A = bcd_a_decimal(decA, uniA);

        // Leer unidades de B desde PORTA bits 3-0 .
        uint8_t uniB = PORTA & 0x0F;  
        
        // Leer bit RA5 (MSB de las decenas de B)
        uint8_t bitRA5 = (PORTA >> 5) & 0x01;
        
        // Construir decenas de B: RA5 + RE2 + RE1 + RE0
        uint8_t decB = ((bitRA5 & 1) << 3) |     // RA5 en bit 3
                      (((PORTE >> 0) & 1) << 2) | // RE0 en bit 2  
                      (((PORTE >> 1) & 1) << 1) | // RE1 en bit 1
                      ((PORTE >> 2) & 1);         // RE2 en bit 0
        
        // Convertir BCD a valor decimal
        uint8_t B = bcd_a_decimal(decB, uniB);
        
        uint16_t X;  // Variable para el resultado (16 bits por posibles valores grandes)
        if (A > B) {
            // Si A es mayor que B: X = B al cuadrado
            X = (uint16_t)B * (uint16_t)B;
        } else if (A == B) {
            // Si A es igual a B: X = A + 2*B
            X = (uint16_t)A + 2U * (uint16_t)B;
        } else {
            // Si A es menor que B: X = B / (A/2)
            // Evitar división por cero: si A/2 == 0 usar divisor 1
            uint8_t divisor = (A / 2) == 0 ? 1 : (A / 2);
            X = (uint16_t)B / divisor;
        }

        uint8_t m, c, d, u;  // Variables para miles, centenas, decenas, unidades
        decimal_a_bcd(X, &m, &c, &d, &u);

        // Resultado en leds
        mostrar_bcd(m, c, d, u);

        // Pequeña pausa antes de repetir el ciclo
        __delay_ms(200);
    }
}

uint8_t bcd_a_decimal(uint8_t decenas, uint8_t unidades) {
    return (decenas * 10U) + unidades;
}

void decimal_a_bcd(uint16_t valor, uint8_t *miles, uint8_t *centenas, uint8_t *decenas, uint8_t *unidades) {
    *unidades = valor % 10U;
    valor /= 10U; 
    *decenas = valor % 10U;
    valor /= 10U;  
    *centenas = valor % 10U;
    valor /= 10U;
    *miles = valor % 10U;
}

void mostrar_bcd(uint8_t m, uint8_t c, uint8_t d, uint8_t u) {
    // PORTB: miles en nibble alto (bits 7-4), centenas en nibble bajo (bits 3-0)
    PORTB = (m << 4) | (c);
    
    // PORTD: decenas en nibble alto (bits 7-4), parte alta de unidades en nibble bajo
    PORTD = (d << 4) | (u >> 4);
    
    // PORTC: parte baja de unidades en nibble alto (bits 7-4)
    PORTC = (u & 0xF) << 4;
}