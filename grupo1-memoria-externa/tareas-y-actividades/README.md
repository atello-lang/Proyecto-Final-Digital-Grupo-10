# Grupo 1 · Memoria externa — Tareas y actividades

[← Volver al README del grupo 1](../README.md)

Respuestas a las **actividades para todos los grupos** (arquitectura digital del periférico).

## 1. Clasificación del protocolo

| Protocolo | Se usa en | ¿Síncrono o asíncrono? | ¿Half o full-duplex? |
| --- | --- | --- | --- |
| **SPI** (modo 0) | Flash de la tarjeta (gráficos) | **Síncrono**: la FPGA (maestro) genera una línea de reloj dedicada, **SCK**. No hay baud rate. | **Full-duplex**: hay una línea para cada sentido, **MOSI** (FPGA → memoria) y **MISO** (memoria → FPGA), y ambas pueden moverse en el mismo pulso de reloj. |
| **I2C** | EEPROM 24LC256 (récords) | **Síncrono**: la FPGA genera el reloj **SCL**. | **Half-duplex**: una sola línea de datos, **SDA**, que se turnan maestro y memoria (por ejemplo, la memoria responde con ACK por la misma línea). |

## 2. Diagrama de funcionamiento (según las hojas de datos)

- [Diagrama de bloques, señales y tiempos](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FProyecto-Final-Digital-Grupo-10%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Diagrama_Funcionamiento.drawio) (3 páginas: bloques, tabla de señales con los tiempos de las hojas de datos y diagramas de tiempos SPI e I2C)
- [Diagrama de flujo de lo que hace la máquina](https://app.diagrams.net/#Uhttps%3A%2F%2Fraw.githubusercontent.com%2Fwwilchesc%2FProyecto-Final-Digital-Grupo-10%2Fmain%2Fgrupo1-memoria-externa%2Fdocs%2FGrupo1_Flujo_Maquina.drawio)

Resumen de las señales:

| Señal | Sentido | Qué lleva |
| --- | --- | --- |
| SCK | FPGA → Flash | Reloj SPI (12,5 MHz; la lectura 0x03 de una Flash SPI típica admite hasta 50 MHz) |
| MOSI | FPGA → Flash | Comando (0x03 leer, 0x02 escribir…), dirección de 24 bits y datos |
| MISO | Flash → FPGA | Datos leídos |
| CS | FPGA → Flash | Selección, activo en bajo; una transacción por cada CS bajo |
| SCL | FPGA → EEPROM | Reloj I2C (100 kHz; máx. 400 kHz) |
| SDA | Bidireccional | Dirección 0x50 + R/W, dirección interna, datos y ACK (open-drain con pull-up de 4,7 kΩ) |

## 3. Mapa de hardware y conteo de pines

**Total: 8 pines de la FPGA** (6 Flash + 2 I2C). Solo 2 llevan cables hacia afuera (SCL y SDA de la EEPROM); la Flash ya está soldada en la tarjeta.

| # | Señal (nombre en el .cst) | Pin FPGA | Va a | Pin del chip |
| --- | --- | --- | --- | --- |
| 1 | `i2c_scl` | K4 | 24LC256 SCL | 6 |
| 2 | `i2c_sda` | J4 | 24LC256 SDA | 5 |
| 3 | `flash_cs_n` | E6 | Flash de la tarjeta | — |
| 4 | `flash_sck` | E7 | Flash de la tarjeta | — |
| 5 | `flash_mosi` | D6 | Flash de la tarjeta | — |
| 6 | `flash_miso` | E5 | Flash de la tarjeta | — |
| 7 | `flash_wp_n` | D5 | Flash de la tarjeta | — |
| 8 | `flash_hold_n` | E4 | Flash de la tarjeta | — |

Alimentación de la EEPROM: 3,3 V y GND de la tarjeta, con un condensador de 100 nF entre VCC y GND junto al chip. 24LC256: A0, A1, A2 y WP (1, 2, 3, 7) a GND, y una resistencia de 4,7 kΩ de SDA y de SCL a 3,3 V.

> Los pines de la FPGA son la asignación propuesta del proyecto; se confirman con el grupo 6 contra el esquemático del Dock antes de cablear. Los de la Flash, además, requieren usar sus pines de configuración (MSPI) como E/S de usuario.

## 4. Registros de control (MMIO)

**SPI** (Flash de la tarjeta, `0x42_0000`):

| Desp. | Registro | Acceso | Contenido |
| --- | --- | --- | --- |
| 0x00 | CONTROL | L/E | bit0 START · bit1 RW (1 = escribir) · bits 15:8 comando · bit16 SIN_DIR (comando sin dirección) · bits 18:17 bytes a leer (1 o 3) |
| 0x04 | ADDR | L/E | Dirección de 24 bits |
| 0x08 | WDATA | L/E | Byte a escribir |
| 0x0C | RDATA | L | Dato leído (8 bits, o 24 para el ID de la Flash) |
| 0x10 | STATUS | L | bit0 BUSY · bit1 DONE (se limpia al leer) |
| 0x14 | CLK_DIV | L/E | SCK = 50 MHz / (2·(CLK_DIV+1)); reset 1 → 12,5 MHz |

**I2C** (`0x46_0000`):

| Desp. | Registro | Acceso | Contenido |
| --- | --- | --- | --- |
| 0x00 | CONTROL | E | bit0 START · bit1 STOP · bit2 WRITE · bit3 READ · bit4 ACK_OUT (0 = ACK, 1 = NACK al leer) |
| 0x04 | TX_DATA | L/E | Byte a enviar |
| 0x08 | RX_DATA | L | Byte recibido |
| 0x0C | STATUS | L | bit0 BUSY · bit1 ACK_IN (0 = la EEPROM respondió) · bit2 DONE |
| 0x10 | CLK_DIV | L/E | SCL = 50 MHz / (4·(CLK_DIV+1)); reset 124 → 100 kHz |

Uso: la CPU escribe los datos y el comando, sondea `STATUS.BUSY` hasta que valga 0 y lee el resultado. No se usan interrupciones.

## 5. Componentes a comprar: lista de precios

Precios consultados el 29 de octubre de 2026; los marcados como *aprox.* son referencias de mercado y conviene confirmarlos en la tienda.

| Componente | Cant. | Precio unitario (COP) | Subtotal (COP) | Dónde |
| --- | --- | --- | --- | --- |
| EEPROM 24LC256-I/P (DIP-8) | 1 | 11.900 (IVA incluido) | 11.900 | [Sigma Electrónica](https://www.sigmaelectronica.net/producto/24lc256/) |
| Resistencia 4,7 kΩ 1/4 W | 2 | ≈ 100 (aprox.) | 200 | Cualquier tienda de electrónica |
| Condensador cerámico 100 nF | 1 | ≈ 200 (aprox.) | 200 | Cualquier tienda de electrónica |
| Zócalo DIP-8 | 1 | ≈ 600 (aprox.) | 600 | Cualquier tienda de electrónica |
| Cables dupont macho-hembra (paquete) | 1 | ≈ 6.000 (aprox.) | 6.000 | Cualquier tienda de electrónica |
| Protoboard pequeña (si no la tienen) | 1 | ≈ 8.000 (aprox.) | 8.000 | Cualquier tienda de electrónica |
| **Total aproximado** | | | **≈ 26.900** | |

La Flash no se compra: es la de 64 Mbit que trae la Tang Primer 25K. Se recomienda comprar una EEPROM de repuesto.

## Cambio de diseño: se descarta la SPI-RAM (05/11/2026)

En la primera versión se usaba una SRAM serial 23LC1024 como memoria intermedia: los gráficos pasaban de la Flash a la SPI-RAM y de ahí a la BRAM. Al revisar el diseño se descartó porque:

1. **No cumplía una función propia.** Los gráficos de un panel de 64×64 caben en la BRAM de la FPGA, y la Flash se lee por el mismo SPI y a la misma velocidad que la SPI-RAM. El camino Flash → BRAM hace lo mismo con un paso menos.
2. **El autotest del SPI se hace con la Flash**, leyendo su JEDEC ID (comando 0x9F).
3. **No se consigue en Colombia**: solo por pedido al exterior, con semanas de espera.

El grupo sigue teniendo los dos protocolos (SPI con la Flash e I2C con la EEPROM) y un chip externo cableado. El diseño queda más simple: 8 pines en vez de 12, un controlador SPI en vez de dos instancias, y unos $11.800 menos en compras.
