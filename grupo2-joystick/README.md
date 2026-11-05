# Grupo 2 · Joystick SPI

Receptor del Joystick: recibe las teclas que presiona el jugador y se las entrega al procesador.

## Integrantes

- Miguel Pardo
- Crsitian Silva
- Sebastian Ochoa
- Arturo Tello

## Qué hace este grupo

Nuestro grupo implementa el periférico de joystick analógico de la consola de videojuegos. Su función es leer la posición del joystick (ejes X e Y) y el estado de su pulsador, y dejar esa información disponible para el procesador FemtoRV, que la usa para controlar el juego (mover al personaje, navegar menús, etc.).

El joystick entrega dos voltajes analógicos, uno por eje, entre 0 y 3.3 V. Como la FPGA Tang Nano solo trabaja con señales digitales, usamos un conversor analógico-digital MCP3008 (10 bits, resolución de ≈ 3.22 mV por paso). El ADC se comunica con la FPGA por SPI en modo 0, donde la FPGA actúa como maestro y el MCP3008 como esclavo.

- Dispositivo(s): Joystick conectado a MCP3008
- Protocolo: SPI
- Dirección base en el mapa de memoria: `0x43_0000`

## Tareas

- [ ] Estudiar el protocolo SPI y la trama del MCP3008
- [ ] Definir los registros del receptor
- [ ] Diseñar el receptor en Verilog
- [ ] Definir la asignación de cada jugador
- [ ] Escribir el testbench
- [ ] Probar en la placa

## SPI y como funciona

En esta sección explicamos cómo conversan la FPGA y el MCP3008: cuándo se mueve cada señal, qué bits viajan en cada pulso y cómo vamos a convertirlo en un circuito. Los términos que usamos están definidos en [1. Terminología básica](01-terminologia.md).

## 2.1 La idea general

SPI es una conversación entre dos chips: **un maestro** que manda y **un esclavo** que obedece. Nos ayuda imaginarlo como un walkie-talkie con metrónomo:

- El maestro (la FPGA) **marca el ritmo** con un reloj (SCLK).
- En cada tick, **un bit sale del maestro hacia el esclavo** y **un bit sale del esclavo hacia el maestro**, al mismo tiempo.
- Cada chip tiene un **registro de desplazamiento**: una fila de casillas donde, en cada tick, todos los bits se corren un lugar, entra uno nuevo por un extremo y sale otro por el otro.

Después de 17 ticks, la FPGA habrá enviado su comando (solo los primeros 5 bits importan) y habrá recibido el número de 10 bits.

> **Regla de oro:** el esclavo nunca habla por su cuenta. Solo responde mientras el maestro tenga CS en bajo y mande pulsos de SCLK.

## 2.2 Las 4 líneas y cómo se conectan

La FPGA y el MCP3008 se unen con cuatro líneas:

| Línea | Dirección | Pin del MCP3008 | Conexión |
|---|---|---|---|
| **SCLK** | FPGA → ADC | 13 (CLK) | Reloj generado por la FPGA |
| **MOSI** | FPGA → ADC | 11 (DIN) | Comando que envía la FPGA |
| **MISO** | ADC → FPGA | 12 (DOUT) | Resultado que envía el ADC |
| **CS** | FPGA → ADC | 10 (CS/SHDN) | Selección del chip, activa en bajo |

Además, el chip necesita alimentación y las entradas del joystick:

| Pin | Nombre | Conectar a |
|---|---|---|
| 16 | VDD | 3.3 V |
| 15 | VREF | 3.3 V (define que 3.3 V = 1023) |
| 14 | AGND | GND |
| 9 | DGND | GND |
| 1 a 4 | CH0 a CH3 | Ejes X e Y del joystick (3.3 V arriba, GND abajo, el eje al canal) |

Todos los GND deben ir unidos. Sin tierra común, SPI no funciona.

## 2.3 Reglas del modo 0: quién cambia y quién lee, y cuándo

En modo 0, SCLK **descansa en 0**. En cada pulso hay dos momentos:

| Momento | Qué pasa |
|---|---|
| **Flanco de subida** (0 → 1) | Se **leen** los bits: el ADC lee MOSI y la FPGA lee MISO. |
| **Flanco de bajada** (1 → 0) | Se **cambian** los bits: la FPGA pone el siguiente bit en MOSI y el ADC pone el siguiente bit en MISO. |

Con esta división nadie lee una línea mientras está cambiando. Cada bit se prepara en la bajada y se lee estable en la subida siguiente. El primer bit (start) tiene que estar ya puesto en MOSI antes del primer flanco de subida, por eso lo colocamos justo al bajar CS.

## 2.4 Límites de tiempo que tenemos que respetar

Estos valores salen del datasheet de Microchip MCP3004/3008:

- Velocidad de SCLK: 3.6 MHz como máximo a 5 V y 1.35 MHz a 2.7 V. Como trabajamos a 3.3 V, usamos **≤ 1.35 MHz**, que es el valor seguro.
- SCLK en alto y en bajo: al menos 125 ns cada uno.
- De bajar CS al primer flanco de subida (tSUCS): al menos 100 ns.
- CS en alto entre conversiones (tCSH): al menos 270 ns.
- Con SCLK ≈ 0.85 MHz el semiperiodo es ~590 ns, lo que cumple con holgura todos los mínimos.

## Diseño

[Ver diagramas](./docs/Diagramas.md)
_(Diagrama de bloques, diagrama de flujo, cálculos. Guardar los archivos en `docs/`.)_

## Registros

| Desplazamiento | Nombre | Acceso | Descripción |
| --- | --- | --- | --- |
| 0x00 | | | |

## Pines

| Señal | Pin FPGA | Conector | Nota |
| --- | --- | --- | --- |
| | | | |

## Simulación

_(Cómo correr el testbench y qué se verificó. Archivos en `sim/`.)_

## Avances

| Fecha | Avance |
| --- | --- |
| | |

## Pendientes y problemas

-
