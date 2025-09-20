;==============================================================================
; CÓDIGO PARA EFECTOS DE LEDs CON PIC16F877A
;==============================================================================
; Compilador: PIC-AS (MPLAB X)
; Autor: Asistente v0 (modificado)
; Descripción: Control de 12 LEDs con dos modos de operación:
;              - MODO 1: Barrido secuencial de LEDs
;              - MODO 2: Parpadeo alternante (impares/pares)
;==============================================================================

    #include <xc.inc>          ; Incluir archivo de definiciones del compilador

;------------------------------------------------------------------------------
; CONFIGURACIÓN DE FUSIBLES
;------------------------------------------------------------------------------
    CONFIG FOSC = HS        ; Oscilador de alta velocidad (Cristal)
    CONFIG WDTE = OFF       ; Watchdog Timer deshabilitado
    CONFIG PWRTE = ON       ; Power-up Timer habilitado (delay de 72ms al inicio)
    CONFIG BOREN = ON       ; Brown-out Reset habilitado (protección por bajo voltaje)
    CONFIG LVP = OFF        ; Low Voltage Programming deshabilitado
    CONFIG CPD = OFF        ; Data EEPROM Memory Code Protection deshabilitado
    CONFIG WRT = OFF        ; Flash Program Memory Write Enable deshabilitado
    CONFIG CP = OFF         ; Flash Program Memory Code Protection deshabilitado

;------------------------------------------------------------------------------
; VARIABLES EN MEMORIA RAM (BANCO 0)
;------------------------------------------------------------------------------
PSECT udata_bank0
delay_counter1:     DS 1    ; Contador principal para delays (loop externo)
delay_counter2:     DS 1    ; Contador secundario para delays (loop interno)
current_led_pos:    DS 1    ; Posición actual del LED en el barrido (0-11)
scan_direction:     DS 1    ; Dirección del barrido (0: derecha, 1: izquierda)
blink_pattern:      DS 1    ; Patrón actual de parpadeo (0: impares, 1: pares)

;------------------------------------------------------------------------------
; VECTOR DE RESET - Punto de entrada al programa
;------------------------------------------------------------------------------
PSECT resetVec, class=CODE, delta=2
resetVec:
    goto    main            ; Saltar al programa principal

;------------------------------------------------------------------------------
; CÓDIGO PRINCIPAL
;------------------------------------------------------------------------------
PSECT code, class=CODE, delta=2

;==============================================================================
; PROGRAMA PRINCIPAL - Configuración inicial
;==============================================================================
main:
    ;------------------------------
    ; CONFIGURACIÓN DE PUERTOS I/O
    ;------------------------------
    BANKSEL TRISB           ; Seleccionar banco de TRISB
    clrf    TRISB           ; Configurar PORTB completo como salidas (RB0-RB7)

    BANKSEL TRISD           ; Seleccionar banco de TRISD
    movlw   0x0F            ; Configurar RD0-RD3 como entradas, RD4-RD7 como salidas
    movwf   TRISD

    ;------------------------------
    ; CONFIGURACIÓN DE PERIFÉRICOS
    ;------------------------------
    BANKSEL ADCON1          ; Seleccionar banco de ADCON1
    movlw   0x07            ; Configurar todos los pines AN como digitales
    movwf   ADCON1

    BANKSEL CMCON           ; Seleccionar banco de CMCON
    movlw   0x07            ; Deshabilitar comparadores analógicos
    movwf   CMCON

    ; Configurar resistencias pull-up internas
    BANKSEL OPTION_REG
    bcf     OPTION_REG, 7   ; RBPU = 0 -> habilitar pull-ups en PORTB

    ;------------------------------
    ; INICIALIZACIÓN DE PUERTOS
    ;------------------------------
    BANKSEL PORTB           ; Seleccionar banco de PORTB
    clrf    PORTB           ; Apagar todos los LEDs de PORTB
    BANKSEL PORTD           ; Seleccionar banco de PORTD
    clrf    PORTD           ; Apagar todos los LEDs de PORTD

    ;------------------------------
    ; INICIALIZACIÓN DE VARIABLES
    ;------------------------------
    clrf    current_led_pos ; Iniciar posición de LED en 0
    clrf    scan_direction  ; Iniciar dirección de barrido hacia derecha
    clrf    blink_pattern   ; Iniciar patrón de parpadeo en 0 (impares)

;==============================================================================
; BUCLE PRINCIPAL DEL PROGRAMA
;==============================================================================
main_loop:
    ; Leer estado del pulsador en RD0 
    BANKSEL PORTD
    btfss   PORTD, 0        ; Si RD0 = 0 (pulsador liberado) -> modo PARPADEO
    goto    blink_mode      ; Saltar al modo parpadeo alternante
    ; Si RD0 = 1 (pulsador presionado) -> modo BARRIDO
    goto    scan_mode       ; Saltar al modo barrido secuencial

;==============================================================================
; MODO AUTO FANTÁSTICO
; Efecto: Un LED individual se enciende y se mueve secuencialmente a través
;         de los 12 LEDs, cambiando de dirección al llegar a los extremos.
;==============================================================================
scan_mode:
    ; Apagar todos los LEDs antes de encender uno nuevo
    BANKSEL PORTB
    clrf    PORTB           ; Apagar todos los LEDs de PORTB (RB0-RB7)
    BANKSEL PORTD
    movlw   0x0F            ; Máscara para preservar RD0-RD3 (entradas)
    andwf   PORTD, f        ; Limpiar RD4-RD7 (LEDs 9-12) manteniendo entradas

    ; Seleccionar y encender el LED correspondiente a la posición actual
    movf    current_led_pos, w  ; Cargar posición actual en W
    btfsc   STATUS, 2       ; Verificar si W == 0 (primer LED)
    goto    led_0
    addlw   -1              ; Decrementar W y verificar siguiente posición
    btfsc   STATUS, 2
    goto    led_1
    addlw   -1
    btfsc   STATUS, 2
    goto    led_2
    addlw   -1
    btfsc   STATUS, 2
    goto    led_3
    addlw   -1
    btfsc   STATUS, 2
    goto    led_4
    addlw   -1
    btfsc   STATUS, 2
    goto    led_5
    addlw   -1
    btfsc   STATUS, 2
    goto    led_6
    addlw   -1
    btfsc   STATUS, 2
    goto    led_7
    addlw   -1
    btfsc   STATUS, 2
    goto    led_8
    addlw   -1
    btfsc   STATUS, 2
    goto    led_9
    addlw   -1
    btfsc   STATUS, 2
    goto    led_10
    addlw   -1
    btfsc   STATUS, 2
    goto    led_11
    goto    scan_delay      ; Seguridad: si posición inválida, ir a delay

;------------------------------------------------------------------------------
; RUTINAS PARA ENCENDER LEDs ESPECÍFICOS
; LEDs 1-8 en PORTB (RB0-RB7), LEDs 9-12 en PORTD (RD7-RD4)
;------------------------------------------------------------------------------
led_0:
    BANKSEL PORTB
    movlw   0x01            ; Encender LED 1 (RB0)
    movwf   PORTB
    goto    scan_delay

led_1:
    BANKSEL PORTB
    movlw   0x02            ; Encender LED 2 (RB1)
    movwf   PORTB
    goto    scan_delay

led_2:
    BANKSEL PORTB
    movlw   0x04            ; Encender LED 3 (RB2)
    movwf   PORTB
    goto    scan_delay

led_3:
    BANKSEL PORTB
    movlw   0x08            ; Encender LED 4 (RB3)
    movwf   PORTB
    goto    scan_delay

led_4:
    BANKSEL PORTB
    movlw   0x10            ; Encender LED 5 (RB4)
    movwf   PORTB
    goto    scan_delay

led_5:
    BANKSEL PORTB
    movlw   0x20            ; Encender LED 6 (RB5)
    movwf   PORTB
    goto    scan_delay

led_6:
    BANKSEL PORTB
    movlw   0x40            ; Encender LED 7 (RB6)
    movwf   PORTB
    goto    scan_delay

led_7:
    BANKSEL PORTB
    movlw   0x80            ; Encender LED 8 (RB7)
    movwf   PORTB
    goto    scan_delay

led_8:
    BANKSEL PORTD
    movlw   0x80            ; Encender LED 9 (RD7)
    iorwf   PORTD, f
    goto    scan_delay

led_9:
    BANKSEL PORTD
    movlw   0x40            ; Encender LED 10 (RD6)
    iorwf   PORTD, f
    goto    scan_delay

led_10:
    BANKSEL PORTD
    movlw   0x20            ; Encender LED 11 (RD5)
    iorwf   PORTD, f
    goto    scan_delay

led_11:
    BANKSEL PORTD
    movlw   0x10            ; Encender LED 12 (RD4)
    iorwf   PORTD, f

;------------------------------------------------------------------------------
; DELAY PARA MODO AUTO FANTÁSTICO (Controla la velocidad del barrido)
;------------------------------------------------------------------------------
scan_delay:
    movlw   160             ; Cargar valor para loop externo
    movwf   delay_counter1
scan_delay_loop1:
    movlw   255             ; Cargar valor para loop interno
    movwf   delay_counter2
scan_delay_loop2:
    decfsz  delay_counter2, f   ; Decrementar y saltar si cero
    goto    scan_delay_loop2    ; Continuar loop interno
    decfsz  delay_counter1, f   ; Decrementar y saltar si cero
    goto    scan_delay_loop1    ; Continuar loop externo

    ;--------------------------------------------------------------------------
    ; ACTUALIZAR POSICIÓN DEL LED Y DIRECCIÓN DEL BARRIDO
    ;--------------------------------------------------------------------------
    btfsc   scan_direction, 0   ; Verificar dirección actual (0=derecha, 1=izquierda)
    goto    scan_backward       ; Si dirección = 1, ir hacia atrás

scan_forward:
    ; Movimiento hacia adelante (derecha)
    incf    current_led_pos, f  ; Incrementar posición del LED
    movf    current_led_pos, w  ; Cargar posición actual
    sublw   11                  ; Verificar si posición > 11 (límite superior)
    btfsc   STATUS, 0           ; Si carry=1, posición <= 11 (válida)
    goto    main_loop           ; Continuar con posición válida
    
    ; Si posición excedió el límite (12+)
    movlw   11                  ; Fijar posición en último LED (11)
    movwf   current_led_pos
    bsf     scan_direction, 0   ; Cambiar dirección a izquierda
    goto    main_loop

scan_backward:
    ; Movimiento hacia atrás (izquierda)
    movf    current_led_pos, w  ; Cargar posición actual
    btfsc   STATUS, 2           ; Verificar si posición == 0 (límite inferior)
    goto    scan_change_direction ; Si en límite, cambiar dirección
    decf    current_led_pos, f  ; Decrementar posición
    goto    main_loop

scan_change_direction:
    clrf    current_led_pos     ; Reiniciar posición a 0
    bcf     scan_direction, 0   ; Cambiar dirección a derecha
    goto    main_loop

;==============================================================================
; MODO PARPADEO ALTERNANTE
; Efecto: Alterna entre dos patrones:
;         - Estado 0: LEDs impares encendidos (1,3,5,7,9,11)
;         - Estado 1: LEDs pares encendidos (2,4,6,8,10,12)
;==============================================================================
blink_mode:
    ; Determinar patrón actual de parpadeo
    movf    blink_pattern, w
    btfsc   STATUS, 2       ; Verificar si blink_pattern == 0
    goto    blink_odds      ; Saltar a patrón de LEDs impares
    goto    blink_evens     ; Saltar a patrón de LEDs pares

blink_odds:
    ; PATRÓN 0: LEDs IMPARES ENCENDIDOS
    BANKSEL PORTB
    movlw   0x55            ; Encender RB0, RB2, RB4, RB6 (LEDs 1,3,5,7)
    movwf   PORTB

    BANKSEL PORTD
    movlw   0x0F            ; Máscara para preservar RD0-RD3 (entradas)
    andwf   PORTD, f        ; Limpiar RD4-RD7 manteniendo entradas
    movlw   0xA0            ; Encender RD7 y RD5 (LEDs 9,11) - binario: 10100000
    iorwf   PORTD, f

    ; Preparar siguiente estado (pares)
    movlw   1
    movwf   blink_pattern
    goto    blink_delay

blink_evens:
    ; PATRÓN 1: LEDs PARES ENCENDIDOS
    BANKSEL PORTB
    movlw   0xAA            ; Encender RB1, RB3, RB5, RB7 (LEDs 2,4,6,8)
    movwf   PORTB

    BANKSEL PORTD
    movlw   0x0F            ; Máscara para preservar RD0-RD3 (entradas)
    andwf   PORTD, f        ; Limpiar RD4-RD7 manteniendo entradas
    movlw   0x50            ; Encender RD6 y RD4 (LEDs 10,12) - binario: 01010000
    iorwf   PORTD, f

    ; Preparar siguiente estado (impares)
    clrf    blink_pattern

;------------------------------------------------------------------------------
; DELAY PARA MODO PARPADEO (Controla la velocidad del parpadeo)
;------------------------------------------------------------------------------
blink_delay:
    movlw   190             ; Cargar valor para loop externo
    movwf   delay_counter1
blink_delay_loop1:
    movlw   255             ; Cargar valor para loop interno
    movwf   delay_counter2
blink_delay_loop2:
    decfsz  delay_counter2, f   ; Decrementar y saltar si cero
    goto    blink_delay_loop2   ; Continuar loop interno
    decfsz  delay_counter1, f   ; Decrementar y saltar si cero
    goto    blink_delay_loop1   ; Continuar loop externo

    ; Volver al bucle principal
    goto    main_loop

;==============================================================================
; FIN DEL PROGRAMA
;==============================================================================
    END