# Grupo 5 · Juego y menú

Firmware C sin sistema operativo para integrar con FemtoRV32 en Tang Nano 9K.
Validado en Linux y compilado a objetos RV32I

- **PAC-MAN**: explorar el laberinto, recoger energía y evitar fantasmas. Un jugador o
  dos en cooperación, tres vidas compartidas y puntuación de equipo.
- **Snake**: individual o competitivo de dos jugadores, un punto por alimento,
  crecimiento hasta 128 segmentos, colisiones simultáneas y empate.
## Integrantes
- Sebastian Cespedes
- Juan Jose Mora
- Luis Felipe Cardenas
## Controles con dos joysticks

Cada eje usa la lectura MCP3008 de 10 bits (`0` a `1023`). El centro se fija en
`512` y hay una zona muerta de `±128`; para diagonales se elige el eje con mayor
desplazamiento (en empate, el vertical). Un valor X bajo/alto es izquierda/
derecha; Y bajo/alto es arriba/abajo.

| Contexto | Control |
|---|---|
| Menú: juego | J1 arriba/abajo elige PAC-MAN/Snake |
| Menú: jugadores | J1 izquierda/derecha elige uno/dos jugadores |
| Iniciar / repetir | Pulsador de J1 |
| Jugador 1 / jugador 2 | Joystick 1 / joystick 2, respectivamente |
| Partida | Pulsador J1 pausa/reanuda; pulsador J2 reinicia |
| Volver al menú | Pulsar ambos botones a la vez |

La selección inicial conserva dos jugadores. Los botones se detectan por flanco;
se deben soltar antes de repetir la acción. En modo individual solo J1 controla
al personaje.

El mapa de registros y la adquisición continua de los cuatro canales están
propuestos en [la documentación del Grupo 2](../grupo2-joystick/README.md);
el contrato MMIO aún debe confirmarse antes de conectar el firmware a la FPGA.
La demo HTML usa la Gamepad API como simulador independiente; no lee el MCP3008
ni sustituye la integración MMIO del firmware.

## Verificación reproducible

```sh
make test
make demo
make compare-html  # requiere Node.js; compara el canvas original
make sanitize
make size-rv32
```

`demo` genera PPM ampliados 6× sin interpolación en `build/`: menú, PAC-MAN,
sus dos fases y modo individual, Snake, pausa, Snake individual y resultado. PNG revisados en `docs/capturas/`.
`size-rv32` requiere `riscv64-unknown-elf-gcc` (configurable con RISCV_CC),
y produce objetos RV32I/ILP32; no una imagen arrancable.

Ver [diseño y memoria](docs/diseno.md), [integración](docs/contratos-MMIO.md)
y [estado de la referencia HTML](docs/fidelidad-visual.md).
