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

- [ ] Estudiar el protocolo SPI y los scan codes
- [ ] Definir los registros del receptor
- [ ] Diseñar el receptor en Verilog
- [ ] Definir las teclas de cada jugador
- [ ] Escribir el testbench
- [ ] Probar en la placa

## Diseño

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
