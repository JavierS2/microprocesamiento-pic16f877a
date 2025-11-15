#ifndef _LCD_H
#define _LCD_H

#include <xc.h>
#define _XTAL_FREQ 4000000UL

// Pines de conexión LCD
#define RS RB0
#define EN RB1
#define D4 RB2
#define D5 RB3
#define D6 RB4
#define D7 RB5

// ================= FUNCIONES =================
void Lcd_Port(char a) {
    D4 = (a & 1) ? 1 : 0;
    D5 = (a & 2) ? 1 : 0;
    D6 = (a & 4) ? 1 : 0;
    D7 = (a & 8) ? 1 : 0;
}

void Lcd_Cmd(char a) {
    RS = 0;
    Lcd_Port(a);
    EN = 1;
    __delay_ms(4);
    EN = 0;
}

void Lcd_Clear() {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char fila, char columna) {
    char pos;
    if (fila == 1)
        pos = 0x80 + columna - 1;
    else if (fila == 2)
        pos = 0xC0 + columna - 1;

    Lcd_Cmd(pos >> 4);
    Lcd_Cmd(pos & 0x0F);
}

void Lcd_Init() {
    // Configurar pines como salida
    TRISB0 = 0;
    TRISB1 = 0;
    TRISB2 = 0;
    TRISB3 = 0;
    TRISB4 = 0;
    TRISB5 = 0;

    Lcd_Port(0x00);
    __delay_ms(20);

    Lcd_Cmd(0x03);
    __delay_ms(5);
    Lcd_Cmd(0x03);
    __delay_ms(11);
    Lcd_Cmd(0x03);

    Lcd_Cmd(0x02);

    // Config LCD
    Lcd_Cmd(0x02);
    Lcd_Cmd(0x08);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x0C);
    Lcd_Cmd(0x00);
    Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char data) {
    char temp, y;

    // nibble alto
    temp = data & 0xF0;
    y = temp >> 4;
    RS = 1;
    Lcd_Port(y);
    EN = 1;
    __delay_us(40);
    EN = 0;

    // nibble bajo
    temp = data & 0x0F;
    Lcd_Port(temp);
    EN = 1;
    __delay_us(40);
    EN = 0;
}

void Lcd_Write_String(const char *a) {
    while (*a)
        Lcd_Write_Char(*a++);
}

#endif
