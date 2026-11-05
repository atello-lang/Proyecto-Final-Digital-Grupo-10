# Resumen del datasheet del MCP3008

Este documento resume en nuestras palabras la información del datasheet del MCP3008 que usamos en el proyecto. No es una copia del datasheet: para ver las figuras de temporización, las curvas y las tablas completas del fabricante hay que consultar el documento original.

**Documento de referencia:** Microchip, *MCP3004/3008: 2.7V 4-Channel/8-Channel 10-Bit A/D Converters with SPI Serial Interface* (código de documento DS21295). Se descarga desde la página del producto en el sitio de Microchip.

Los términos están explicados en [Terminología básica](01-terminologia.md) y el uso del protocolo en [Cómo usar el protocolo SPI](02-uso-spi.md).

## Descripción general

El MCP3008 es un conversor analógico-digital (ADC) de aproximaciones sucesivas de 10 bits, con circuito de muestreo y retención integrado. Se comunica con un microcontrolador o FPGA por una interfaz serial compatible con SPI. El MCP3004 es la versión de 4 canales y el MCP3008 la de 8 canales.

## Características principales

| Característica | Valor |
|---|---|
| Resolución | 10 bits |
| Canales de entrada | 8 (MCP3008) |
| Tipo de entradas | Single-ended o pseudo-diferenciales (programable) |
| Interfaz | SPI, modos 0,0 y 1,1 |
| Alimentación | 2.7 V a 5.5 V |
| Tasa de muestreo máxima | 200 ksps a VDD = 5 V y 75 ksps a VDD = 2.7 V |
| Error INL / DNL | ±1 LSB máximo cada uno |
| Corriente en reposo (standby) | 5 nA típica y 2 µA máxima |
| Rango de temperatura | -40 °C a +85 °C |

En nuestro proyecto alimentamos el chip con 3.3 V, así que nos aplican los valores del rango de baja tensión.

## Pines (encapsulado de 16 pines, PDIP o SOIC)

| Pin | Nombre | Función | Nuestra conexión |
|---|---|---|---|
| 1 | CH0 | Entrada analógica 0 | Eje X del jugador 1 |
| 2 | CH1 | Entrada analógica 1 | Eje Y del jugador 1 |
| 3 | CH2 | Entrada analógica 2 | Eje X del jugador 2 |
| 4 | CH3 | Entrada analógica 3 | Eje Y del jugador 2 |
| 5 a 8 | CH4 a CH7 | Entradas analógicas 4 a 7 | Sin usar |
| 9 | DGND | Tierra digital | GND |
| 10 | CS/SHDN | Selección de chip (activo en bajo) y apagado | Salida CS de la FPGA |
| 11 | DIN | Entrada de datos serial | MOSI de la FPGA |
| 12 | DOUT | Salida de datos serial | MISO de la FPGA |
| 13 | CLK | Reloj serial | SCLK de la FPGA |
| 14 | AGND | Tierra analógica | GND |
| 15 | VREF | Voltaje de referencia | 3.3 V |
| 16 | VDD | Alimentación | 3.3 V |

Los canales que no usamos conviene dejarlos conectados a GND para evitar entradas flotantes. Si usamos dos jugadores, la asignación de canales de la tabla es la que planteamos; se confirma en la tarea de asignación de jugadores.
