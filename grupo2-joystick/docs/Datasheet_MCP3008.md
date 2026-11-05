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

## Características eléctricas relevantes

| Parámetro | Valor |
|---|---|
| Tensión de operación (VDD) | 2.7 V a 5.5 V |
| Corriente de operación | Típica de 425 µA a 5 V y 225 µA a 2.7 V; máxima de 550 µA a 5 V (con DOUT sin carga) |
| Resolución | 10 bits sin códigos perdidos |
| INL (no linealidad integral) | ±0.5 LSB típica y ±1 LSB máxima |
| DNL (no linealidad diferencial) | ±0.25 LSB típica y ±1 LSB máxima |
| Error de offset | ±1.5 LSB máximo |
| Error de ganancia | ±1 LSB máximo |

## Tiempos de la interfaz SPI

| Parámetro | Símbolo | Valor |
|---|---|---|
| Frecuencia de reloj máxima | fCLK | 3.6 MHz a 5 V y 1.35 MHz a 2.7 V |
| Reloj en alto, mínimo | tHI | 125 ns |
| Reloj en bajo, mínimo | tLO | 125 ns |
| CS bajo hasta el primer flanco de subida | tSUCS | 100 ns mínimo |
| CS en alto entre conversiones | tCSH | 270 ns mínimo |
| Flanco de bajada hasta salida habilitada | tEN | 125 ns máximo a 5 V y 200 ns máximo a 2.7 V |
| CS en alto hasta salida deshabilitada | tDIS | 100 ns máximo |
| Tiempo de muestreo de la entrada | tSAMPLE | 1.5 ciclos de reloj |
| Tiempo de conversión | tCONV | 10 ciclos de reloj |

El datasheet advierte que con relojes efectivos por debajo de 10 kHz puede degradarse la linealidad, porque el capacitor de muestreo pierde carga. A 3.3 V usamos fCLK ≤ 1.35 MHz, que es el límite del rango de baja tensión y por lo tanto el valor seguro.

## Conversión del voltaje a número

El código digital de salida depende del voltaje de entrada y de la referencia:


código = 1024 × VIN / VREF


El tamaño de un paso (1 LSB) es VREF / 1024. Con VREF = 3.3 V resulta ≈ 3.22 mV, que es la resolución que usamos en el proyecto.

| VIN | Código aproximado |
|---|---|
| 0 V | 0 |
| 1.65 V | 512 |
| 3.3 V | 1023 |

## Tabla de configuración de canales

Después del bit de start, el primer bit enviado es SGL/DIFF y luego D2, D1 y D0. Con SGL/DIFF = 1 la entrada es single-ended (contra AGND) y con SGL/DIFF = 0 es pseudo-diferencial.

*Modo single-ended (el que usamos):*

| SGL/DIFF | D2 | D1 | D0 | Canal |
|---|---|---|---|---|
| 1 | 0 | 0 | 0 | CH0 |
| 1 | 0 | 0 | 1 | CH1 |
| 1 | 0 | 1 | 0 | CH2 |
| 1 | 0 | 1 | 1 | CH3 |
| 1 | 1 | 0 | 0 | CH4 |
| 1 | 1 | 0 | 1 | CH5 |
| 1 | 1 | 1 | 0 | CH6 |
| 1 | 1 | 1 | 1 | CH7 |

*Modo pseudo-diferencial (no lo usamos):*

| SGL/DIFF | D2 | D1 | D0 | IN+ | IN- |
|---|---|---|---|---|---|
| 0 | 0 | 0 | 0 | CH0 | CH1 |
| 0 | 0 | 0 | 1 | CH1 | CH0 |
| 0 | 0 | 1 | 0 | CH2 | CH3 |
| 0 | 0 | 1 | 1 | CH3 | CH2 |
| 0 | 1 | 0 | 0 | CH4 | CH5 |
| 0 | 1 | 0 | 1 | CH5 | CH4 |
| 0 | 1 | 1 | 0 | CH6 | CH7 |
| 0 | 1 | 1 | 1 | CH7 | CH6 |

## Secuencia de comunicación

1. La FPGA baja CS para iniciar la comunicación.
2. El primer pulso de reloj con DIN en 1 se interpreta como el bit de start.
3. La FPGA envía SGL/DIFF y los tres bits D2, D1, D0 para elegir el canal.
4. El ADC toma la muestra de la entrada y empieza la conversión.
5. El ADC responde con un bit nulo (siempre 0) seguido de los 10 bits del resultado, de B9 a B0.
6. La FPGA sube CS para terminar. Si lo sube antes de tiempo, la conversión se interrumpe.

En nuestra implementación usamos la trama de 17 pulsos descrita en el documento de uso del protocolo SPI. La posición exacta del bit nulo respecto de los pulsos de reloj debe contrastarse con la figura de la trama en el datasheet original antes de codificar.

## Qué tomamos del datasheet para el diseño

| Decisión de diseño | Valor | Origen |
|---|---|---|
| Modo SPI | 0 (CPOL = 0, CPHA = 0) | Interfaz SPI modos 0,0 y 1,1 |
| Frecuencia de SCLK | ≤ 1.35 MHz | fCLK a 2.7 V |
| Espera tras bajar CS | ≥ 100 ns | tSUCS |
| CS en alto entre transacciones | ≥ 270 ns | tCSH |
| Pulsos por transacción | 17 | Trama de start, configuración, muestreo, bit nulo y 10 bits |
| Canales usados | CH0 a CH3, single-ended | Tabla de configuración |
| VREF | 3.3 V | Define el rango completo 0 a 1023 |
