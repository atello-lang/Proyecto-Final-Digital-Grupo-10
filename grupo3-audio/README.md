# Grupo 3 · Audio

Periférico de sonido: genera los efectos de la consola y los envía al amplificador.

## Integrantes

- John Fernando Franco Rojas
- Yury Tatiana Muños Perdomo
- Gabriel Andrés Arenas Carmona

## Qué hace este grupo

Este subsistema se encarga de la generación, síntesis y reproducción del audio de la consola retro en la FPGA. La función principal del grupo es diseñar e implementar una matriz de sonidos (tabla de frecuencias, duraciones y tipos de onda para los efectos SFX y la música de fondo estilo chiptune), así como el módulo transmisor I2S en Verilog que convierte los datos en audio serie para reproducir los sonidos de los juegos Pac-Man y Snake.

- Dispositivo(s): Amplificador DAC Clase D MAX98357A (3.2 W) + parlante de 8 Ω (1–3 W)
- Protocolo: I2S (Inter-IC Sound)
- Dirección base en el mapa de memoria: `0x47_0000` (por evaluar el 0mapear a `0x1000_0020` en el espacio MMIO del FemtoRV)

## Protocolo I2S

El protocolo I2S (Inter-IC Sound) es un estándar de comunicación síncrono diseñado específicamente para la transferencia de audio digital PCM serie entre circuitos integrados.

* **Naturaleza síncrona:** Es un protocolo síncrono y unidireccional que comparte una señal de reloj dedicada entre la FPGA (maestro) y el DAC MAX98357A (esclavo).
* **Señales del bus:**
  * **BCLK (Bit Clock):** Reloj de bits a 1.35 MHz, generado al dividir el reloj base de la FPGA (27 MHz) entre 20. Marca el ritmo de envío de cada bit individual.
  * **LRCK / WS (Word Select):** Reloj de selección de canal que establece la frecuencia de muestreo de audio ($f_s = 42.1875\text{ kHz}$). Determina si el dato en tránsito pertenece al canal izquierdo o al derecho.
  * **DIN (Data In):** Línea de datos serie donde se transmiten las muestras de 16 bits con signo, enviando siempre el bit más significativo primero (MSB).
* **Formato de trama:** Cada trama completa dura 32 ciclos de BCLK (16 bits para el canal izquierdo y 16 bits para el derecho).

## Tareas

- [ ] Estudiar el protocolo I2S y el módulo amplificador
- [ ] Definir los registros del periférico
- [ ] Diseñar el generador de tonos y el transmisor I2S
- [ ] Definir los efectos de sonido
- [ ] Escribir el testbench
- [ ] Probar en la placa

## Diseño

### 1. Diagrama de Flujo, caja negra y mas
[Ver Diagramas de Flujo/ Cana Negra](./docs/Diagrama_de_Flujo.md)


### 2. DataSheet
[Tarjeta MAX98357A](./docs/MAX98357_ALLDATASHEET.pdf)

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
