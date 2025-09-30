# Códigos en Ensamblador y C para PIC16F877A  

Este repositorio contiene ejemplos y prácticas de programación para el **microcontrolador PIC16F877A**, desarrollados en **MPLAB X IDE** utilizando tanto lenguaje ensamblador como lenguaje C.  

Cada proyecto incluye su respectiva simulación en **Proteus 8**, lo que permite probar y verificar el funcionamiento del código antes de implementarlo en hardware real.  

---

## 🛠 Herramientas utilizadas  

- **Microcontrolador:** PIC16F877A  
- **IDE:** [MPLAB X IDE](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)  
- **Compiladores:**  
  - MPASM (para proyectos en ensamblador)  
  - XC8 (para proyectos en C)  
- **Simulador:** [Proteus 8](https://www.labcenter.com/)
  
---

## 🚀 Pasos para ejecutar los proyectos  

### 2. Abrir el proyecto en MPLAB X IDE  
- Abre **MPLAB X IDE**.  
- Selecciona `File > Open Project`.  
- Busca la carpeta del proyecto deseado y selecciónala.  
- Verifica que el compilador esté configurado:  
  - **PIC-AS** si el código es en ensamblador.  
  - **XC8** si el código es en C.  

### 3. Compilar el código y generar el archivo `.hex`  
- Haz clic derecho sobre el proyecto en el panel lateral.  
- Selecciona **Set as Main Project**.  
- Luego selecciona **Clean and Build**.  
- Una vez compilado, el archivo `.hex` se generará en la carpeta: /dist/default/production/

  Ejemplo: `Proyecto.X.production.hex`  

### 4. Abrir la simulación en Proteus 8  
- Abre **Proteus 8**.  
- Selecciona `File > Open Project`.  
- Carga el archivo `.pdsprj` correspondiente en la carpeta `/simulaciones/`.  

### 6. Ejecutar la simulación  
- Haz doble clic sobre el **PIC16F877A** en el esquema de Proteus.  
- En el campo **Program File**, carga el archivo `.hex` generado en MPLAB X.  
- Da clic en **OK** para asignar el programa.  
- Finalmente, presiona **Run** (botón play) en Proteus para iniciar la simulación.  

---

## 📊 Checklist de pasos  

- [x] Abrir el proyecto en **MPLAB X IDE**  
- [x] Compilar con **Clean and Build** y generar el `.hex`  
- [x] Abrir la simulación en **Proteus 8**  
- [x] Ejecutar la simulación cargando el `.hex` en el PIC16F877A  

---

## 📌 Ejemplos incluidos  

- Secuencia de LEDs  
- Control con pulsadores  
- Temporizadores y contadores  
- Comunicación serial (USART)  
- Interrupciones  

*(Se irán agregando más proyectos con el tiempo).*  

---

## 🤝 Contribuciones  

Si deseas aportar nuevos códigos o mejorar los existentes:  

1. Haz un **fork** del repositorio.  
2. Crea una nueva rama con tu mejora:  
 ```bash
 git checkout -b feature-nueva

