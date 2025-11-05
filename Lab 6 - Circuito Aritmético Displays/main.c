#include <xc.h>
#include <stdint.h>

#pragma config FOSC = XT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 4000000UL

// Matriz de Cátodo Común
unsigned char display[10] = {
    0x3F,0x06,0x5B,0x4F,0x66,
    0x6D,0x7D,0x07,0x7F,0x67
};

// Definiciones para los selects de displays (usando RC4-RC7)
#define DISPLAY_MILLAR  PORTCbits.RC7
#define DISPLAY_CENTENA PORTCbits.RC6
#define DISPLAY_DECENA  PORTCbits.RC5
#define DISPLAY_UNIDAD  PORTCbits.RC4

#define digito PORTB

// --- Prototipos ---
uint8_t bcd_a_decimal(uint8_t decenas, uint8_t unidades);
void decimal_a_bcd(uint16_t valor, uint8_t *miles, uint8_t *centenas, uint8_t *decenas, uint8_t *unidades);

// --- Función principal ---
void main(void) {
    // Configuración de puertos
    ADCON1 = 0x07; // Todos digitales
    CMCON  = 0x07; // Desactiva comparadores

    // TRIS
    TRISA = 0x2F;  // Entradas para B
    TRISB = 0x00;  // Salida para segmentos
    TRISC = 0x0F;  // RC0-RC3 entradas, RC4-RC7 salidas
    TRISD = 0x0F;  // RD0-RD3 entradas (A decenas)
    TRISE = 0x07;  // RE0-RE2 entradas (B)

    // Inicializar puertos
    PORTA = 0; PORTB = 0; PORTC = 0; PORTD = 0; PORTE = 0;

    uint8_t A = 0, B = 0;
    uint16_t X = 0;
    uint8_t millares = 0, centenas = 0, decenas = 0, unidades = 0, error = 0;

    while(1) {
        // --- Lectura de A ---
        uint8_t uniA = PORTC & 0x0F;    // Unidades A
        uint8_t decA = PORTD & 0x0F;    // Decenas A
        A = bcd_a_decimal(decA, uniA);

        // --- Lectura de B ---
        uint8_t uniB = PORTA & 0x0F;    
        uint8_t decB = ((PORTA & 0x20) >> 2) | (PORTE & 0x07);
        B = bcd_a_decimal(decB, uniB);

        // --- Comprobación de error ---
        error = 0;
        if (A > 99 || B > 99) error = 1;

        if (!error) {
            if (A > B) {
                X = B * B;
            }
            else if (A < B) {
                if (A == 0) {
                    X = 0;
                } else {
                    X = (2 * B) / A;  // X = B / (A/2)
                }
            }
            else { // A == B
                X = A + (2 * B);
            }

            if (X > 9999) error = 1;
        }

        // --- Mostrar o apagar displays ---
        if (error) {
            PORTB = 0x00;
            DISPLAY_MILLAR = 0; 
            DISPLAY_CENTENA = 0; 
            DISPLAY_DECENA = 0;
            DISPLAY_UNIDAD = 0;
        } else {
            // Convertir X a BCD
            decimal_a_bcd(X, &millares, &centenas, &decenas, &unidades);

            // Mostrar solo los dígitos necesarios
            if (X >= 1000) {
                DISPLAY_MILLAR = 1; digito = display[millares];
                __delay_ms(1); DISPLAY_MILLAR = 0;
            }

            if (X >= 100) {
                DISPLAY_CENTENA = 1; digito = display[centenas];
                __delay_ms(1); DISPLAY_CENTENA = 0;
            }

            if (X >= 10) {
                DISPLAY_DECENA = 1; digito = display[decenas];
                __delay_ms(1); DISPLAY_DECENA = 0;
            }

            // Unidades siempre se muestran
            DISPLAY_UNIDAD = 1; digito = display[unidades];
            __delay_ms(1); DISPLAY_UNIDAD = 0;
        }
    }
    return;
}

// --- Funciones auxiliares ---

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
