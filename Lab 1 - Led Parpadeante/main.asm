; Programa: Blink LED en RB0 con retardo de 2 bucles - PIC16F877A (PIC-AS)
; Cristal: 4 MHz

PROCESSOR 16F877A
#include <xc.inc>

; ---------------------------
; Fusibles (CONFIG)
; ---------------------------
CONFIG CP = OFF
CONFIG WDTE = OFF
CONFIG BOREN = OFF
CONFIG PWRTE = ON
CONFIG FOSC = HS
CONFIG WRT = OFF
CONFIG LVP = OFF
CONFIG CPD = OFF

; ---------------------------
; Variables (RAM, banco 0)
; ---------------------------
PSECT udata_bank0
CONT_INT: DS 1     ; Contador interno (rápido)
CONT_EXT: DS 1     ; Contador externo (lento)

; ---------------------------
; Vector de reset
; ---------------------------
PSECT resetVec, class=CODE, delta=2
resetVec:
        PAGESEL INICIO
        GOTO INICIO

; ---------------------------
; Código principal
; ---------------------------
PSECT code, class=CODE, delta=2

INICIO:
        ; Configuración de puertos
        BANKSEL TRISB
        MOVLW   0b11111110    ; RB0 como salida, resto entradas
        MOVWF   TRISB

        BANKSEL PORTB
        CLRF    PORTB         ; LED apagado al inicio

BUCLE_PRINCIPAL:
        ; Encender LED
        BSF     PORTB, 0
        CALL    RETARDO

        ; Apagar LED
        BCF     PORTB, 0
        CALL    RETARDO

        GOTO    BUCLE_PRINCIPAL

; ---------------------------
; Subrutina de retardo con 2 bucles
; ---------------------------
RETARDO:
        MOVLW   200          ; Valor para el contador externo
        MOVWF   CONT_EXT

RETARDO_EXTERNO:
        MOVLW   250          ; Valor para el contador interno
        MOVWF   CONT_INT

RETARDO_INTERNO:
        DECFSZ  CONT_INT, F   ; Decrementa contador interno
        GOTO    RETARDO_INTERNO

        DECFSZ  CONT_EXT, F   ; Cuando el interno llega a 0, decrementa externo
        GOTO    RETARDO_EXTERNO

        RETURN
END