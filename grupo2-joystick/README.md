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

# 1. Terminología básica

# 2. SPI y como funciona

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

## 2.5 Una transacción completa, pulso a pulso

Llamamos **transacción** a una conversación completa: CS baja, se envían 17 pulsos y CS sube.

**Estado de reposo (siempre antes y después):** CS = 1, SCLK = 0, MOSI = 0.

**Secuencia que planteamos:**

1. **Bajamos CS** a 0 y esperamos ≥ 100 ns (en la práctica, 1 o 2 ciclos del reloj de sistema).
2. **Ponemos el primer bit** (el start = 1) en MOSI.
3. **Repetimos 17 veces:** subimos SCLK y leemos MISO, luego bajamos SCLK y ponemos en MOSI el siguiente bit.
4. **Después del pulso 17**, dejamos SCLK en 0.
5. **Subimos CS** a 1 y lo mantenemos ≥ 270 ns antes de la siguiente transacción.
6. **Guardamos** el número de 10 bits recibido en el registro del canal.

### Trama de 17 pulsos

| Pulso | DIN (FPGA → ADC) | DOUT (ADC → FPGA) |
|---|---|---|
| 1 | Start = 1 | Alta impedancia |
| 2 | SGL/DIFF = 1 | Alta impedancia |
| 3 | D2 | Alta impedancia |
| 4 | D1 | Alta impedancia |
| 5 | D0 | Alta impedancia |
| 6 | no importa | Alta impedancia (muestreo) |
| 7 | no importa | Null bit = 0 |
| 8 a 17 | no importa | B9, B8, …, B0 |

### Comandos CH0 a CH3 (single-ended)

Los 4 bits del comando son `{SGL/DIFF, D2, D1, D0}`.

| Canal | Comando | Segundo byte (formato de 3 bytes) |
|---|---|---|
| CH0 | `1000` | `0x80` |
| CH1 | `1001` | `0x90` |
| CH2 | `1010` | `0xA0` |
| CH3 | `1011` | `0xB0` |

## 2.6 Ejemplo completo: leer el eje Y del jugador 1 (CH1)

Para verlo con números, suponemos que el eje mide **2.18 V**. Como 2.18 / 3.22 mV ≈ 677, el ADC debe devolver **677 = `10 1010 0101` en binario (0x2A5)**.

El comando para CH1 es `SGL/DIFF=1, D2=0, D1=0, D0=1`.

| Pulso | FPGA pone en MOSI | ADC pone en MISO | Qué hace la FPGA con lo recibido |
|---|---|---|---|
| 1 | **1** (start) | alta impedancia | Ignora |
| 2 | **1** (SGL/DIFF) | alta impedancia | Ignora |
| 3 | **0** (D2) | alta impedancia | Ignora |
| 4 | **0** (D1) | alta impedancia | Ignora |
| 5 | **1** (D0) | alta impedancia | Ignora |
| 6 | no importa | alta impedancia (muestreo) | Ignora |
| 7 | no importa | 0 (null bit) | Descarta |
| 8 | no importa | **1** (B9) | Guarda |
| 9 | no importa | **0** (B8) | Guarda |
| 10 | no importa | **1** (B7) | Guarda |
| 11 | no importa | **0** (B6) | Guarda |
| 12 | no importa | **1** (B5) | Guarda |
| 13 | no importa | **0** (B4) | Guarda |
| 14 | no importa | **0** (B3) | Guarda |
| 15 | no importa | **1** (B2) | Guarda |
| 16 | no importa | **0** (B1) | Guarda |
| 17 | no importa | **1** (B0) | Guarda |

Los bits guardados, en orden de llegada, son `1010100101`, es decir, 677. Los capturamos con un registro de desplazamiento: en cada pulso útil hacemos `resultado = {resultado[8:0], bit_recibido}`. Como B9 llega primero, termina en la posición más alta.

## 2.7 Variante de 3 bytes (24 pulsos)

Muchas librerías de microcontroladores hacen lo mismo en 3 bytes completos:

| Byte | Se envía | Se recibe |
|---|---|---|
| 1 | `0x01` (7 ceros y el start) | Ignorar |
| 2 | `0x90` para CH1 (`1 001 0000`) | Sus 2 bits bajos son B9 y B8 |
| 3 | `0x00` (relleno) | B7 a B0 |

Con el ejemplo anterior: `rx2[1:0] = 10` y `rx3 = 0xA5`, así que `valor = {rx2[1:0], rx3} = 0x2A5 = 677`.

Las dos variantes dan el mismo resultado. La de 17 pulsos es más corta y la de 24 es más fácil de alinear con bytes. Vamos a elegir una y usarla siempre.

## 2.8 Leer varios canales

El MCP3008 mide **un solo canal por transacción**. Para leer X e Y de dos jugadores tenemos que hacer 4 transacciones seguidas:

1. CH0 → registro X del jugador 1.
2. CH1 → registro Y del jugador 1.
3. CH2 → registro X del jugador 2.
4. CH3 → registro Y del jugador 2.

Entre una y otra, CS sube y espera ≥ 270 ns. Luego repetimos el ciclo sin parar, y así el procesador siempre encuentra el último valor leído en los registros.

## 2.9 Cómo lo traducimos a un circuito en la FPGA

Para implementarlo necesitamos 4 bloques sencillos:

| Bloque | Función |
|---|---|
| **Divisor de reloj** | Cuenta ciclos del reloj de sistema y da un "tick" a la velocidad de SCLK (≤ 1.35 MHz). |
| **Contador de pulsos** | Cuenta de 0 a 16 para saber en qué bit de la trama vamos. |
| **Registros de desplazamiento** | Uno envía el comando por MOSI y otro captura los 10 bits por MISO. |
| **FSM (máquina de estados)** | Coordina todo con la secuencia: `IDLE → CS_BAJA → ENVIAR_Y_LEER_17_PULSOS → CS_SUBE → ESPERA → SIGUIENTE_CANAL`. |

Una buena práctica que vamos a seguir es **no usar SCLK como reloj interno** de la FPGA. Mantenemos todo en el reloj de sistema y generamos SCLK como una señal normal que cambia cuando llega el tick del divisor. Así evitamos problemas de sincronización.

## 2.10 Errores comunes y cómo reconocerlos

Esta tabla nos servirá para depurar cuando probemos el diseño:

| Síntoma | Causa probable |
|---|---|
| Siempre lee 0 | MISO desconectado, CS nunca baja, o el comando está mal formado. |
| Siempre lee 1023 | VREF o VDD sin alimentar, o MISO flotante en alto. |
| Valores a la mitad o al doble de lo esperado | Un bit de desfase: se leyó un pulso antes o después. Revisar dónde se descarta el null bit. |
| Valores ruidosos o erráticos | SCLK demasiado rápido, cables largos, o falta de tierra común. |
| Canal equivocado | Bits D2 D1 D0 mal ordenados. D2 sale primero. |
| Funciona en simulación, falla en la placa | Tiempos (tSUCS, tCSH) no respetados, o modo SPI equivocado. |
| Solo cambia un bit del resultado | Se está usando el flanco de bajada para leer MISO. |

**Prueba de sanidad que vamos a hacer en la placa:**

- Entrada a GND → resultado ≈ 0.
- Entrada a 3.3 V → resultado ≈ 1023.
- Joystick centrado → resultado ≈ 512.

## 2.11 Lista de verificación antes de simular o probar

- [ ] En reposo: CS = 1, SCLK = 0.
- [ ] SCLK ≤ 1.35 MHz y semiperiodo > 125 ns.
- [ ] CS baja ≥ 100 ns antes del primer pulso.
- [ ] El start bit está en MOSI antes de la primera subida.
- [ ] MOSI cambia en las bajadas y MISO se lee en las subidas.
- [ ] Se descarta el null bit, y B9 es el primer bit guardado.
- [ ] El comando de cada canal es correcto (CH0 = `1000`, CH1 = `1001`, CH2 = `1010`, CH3 = `1011`).
- [ ] CS en alto ≥ 270 ns entre transacciones.
- [ ] Todas las tierras unidas, y VDD = VREF = 3.3 V.


# 3. Diseño

En esta sección se detalla la arquitectura interna del controlador Verilog, el funcionamiento de la máquina de estados (FSM) que gestiona la comunicación SPI y el cálculo de división de reloj.

## 3.1 Arquitectura General y Diagrama de Bloques

El módulo se compone de cuatro sub-bloques funcionales diseñados para operar de forma síncrona con el reloj principal de la FPGA y abstraer la complejidad del protocolo SPI al procesador:Generador de Reloj (clk_divider): Toma el reloj del sistema de la FPGA y genera impulsos de habilitación (sclk_tick) para obtener un $SCLK \le 1.35\text{ MHz}$.Máquina de Estados Principal (spi_fsm): Coordina los ciclos de lectura secuencial de los 4 canales (CH0 a CH3) y controla los tiempos de espera de Chip Select ($t_{SUCS}$ y $t_{CSH}$).Registros de Desplazamiento (shift_registers):Registro de Transmisión (Tx): Mantiene la trama de comandos de 5 bits (Start, SGL/DIFF, D2, D1, D0) y coloca cada bit en la línea MOSI.Registro de Recepción (Rx): Captura en serie los bits que entrega el ADC por la línea MISO, descartando el null bit y acumulando los 10 bits de datos (B9 a B0).Banco de Registros e Interfaz de Bus (reg_file): Mantiene almacenados los valores actualizados de los ejes (J1_X, J1_Y, J2_X, J2_Y) y gestiona las lecturas/escrituras del procesador FemtoRV.

## 3.2 Diagramas y Archivos Adjuntos

Esquema detallado de bloques y diagrama de estados de la FSM: [Ver diagramas](./docs/Diagramas.md)

Especificaciones eléctricas y de tiempos del ADC: [Ver datasheet](./docs/Datasheet_MCP3008.md)


_(Diagrama de bloques, diagrama de flujo, cálculos. Guardar los archivos en `docs/`.)_

## 4. Registros
Propuesta de registros del periférico de joystick, ubicado a partir de la dirección base `0x43_0000`. Planteamos registros de 32 bits alineados a palabra (desplazamientos de 4 en 4). Los valores de 10 bits del ADC quedan en los bits `[9:0]` y el resto se lee como 0.

## 4.1 Mapa de registros

| Desplazamiento | Nombre | Acceso | Descripción |
|---|---|---|---|
| `0x00` | `CTRL` | R/W | Control del periférico. Bit 0 `ENABLE`: activa el barrido continuo de los 4 canales. Bit 1 `START`: pide un barrido único y se limpia solo al terminar. |
| `0x04` | `STATUS` | R | Estado. Bit 0 `BUSY`: hay una conversión en curso. Bit 1 `VALID`: ya se completó al menos un barrido y los registros de datos son válidos. |
| `0x08` | `J1_X` | R | Eje X del jugador 1 (canal CH0 del MCP3008). Bits `[9:0]` con el valor 0 a 1023. |
| `0x0C` | `J1_Y` | R | Eje Y del jugador 1 (canal CH1). Bits `[9:0]`. |
| `0x10` | `J2_X` | R | Eje X del jugador 2 (canal CH2). Bits `[9:0]`. |
| `0x14` | `J2_Y` | R | Eje Y del jugador 2 (canal CH3). Bits `[9:0]`. |
| `0x18` | `BUTTONS` | R | Estado de los pulsadores. Bit 0 `J1_BTN` y bit 1 `J2_BTN` (1 = presionado). |
| `0x1C` | `CLKDIV` | R/W | (Opcional) Divisor del reloj de sistema para generar SCLK, de modo que SCLK ≤ 1.35 MHz. |

## 4.2 Funcionamiento previsto

- El periférico hace los barridos por su cuenta: mide CH0, CH1, CH2 y CH3 en orden y va actualizando `J1_X`, `J1_Y`, `J2_X` y `J2_Y`.
- El procesador solo lee: consulta `STATUS` si necesita saber que hay datos válidos y luego lee los ejes y los botones cuando los necesite.
- El procesador solo escribe `CTRL` (y `CLKDIV`, si lo dejamos) para encender el periférico o pedir un barrido.

## 4.3 Puntos por confirmar

1. **Ancho y alineación del bus:** supusimos palabras de 32 bits con desplazamientos múltiplos de 4. Hay que verificar cómo accede el SoC a los periféricos del laboratorio.
2. **Pulsador:** el MCP3008 solo lee señales analógicas, así que el botón de cada joystick debe entrar por un pin digital de la FPGA. Falta decidir si se agrega antirrebote.
3. **Cantidad de jugadores:** el diseño asume dos joysticks (cuatro canales). Si fuera uno, bastaría con `J1_X`, `J1_Y` y un solo bit en `BUTTONS`.
4. **Utilidad de `CLKDIV`:** es opcional. Sin él, el divisor queda fijo en el diseño.

## 5. Pines

| Señal | Pin FPGA | Conector | Nota |
| --- | --- | --- | --- |
| | | | |

## 6. Simulación

_(Cómo correr el testbench y qué se verificó. Archivos en `sim/`.)_

## 7. Avances

| Fecha | Avance |
| --- | --- |
| | |

## 8. Pendientes y problemas

-
