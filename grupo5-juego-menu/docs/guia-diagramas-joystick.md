# Diagramas de flujo del grupo 5 — dos joysticks analógicos SPI

**Versión contrastada con el commit `c81486d` del repositorio `wwilchesc/Proyecto-Final-Digital-Grupo-10` (5 de noviembre de 2026).** Documento técnico de diseño, **no** evidencia de una ejecución en la FPGA. Los diagramas están en `diagramas-grupo5-joystick.drawio` (9 páginas), con nueve PNG de referencia.

## Índice y trazabilidad

| Página | Función documentada | Fuente |
|---|---|---|
| 01 Arquitectura | Joysticks, ADC, procesador, firmware y pantalla | `platform.c`, grupo 2 README |
| 02 Máquina de estados | MENU, PACMAN, SNAKE, RESULT; bandera `paused` | `console.c` |
| 03 Menú | Ejes J1, flancos, selección e inicio | `console.c` |
| 04 Control analógico | Centro 512, zona muerta 128, eje prioritario, botones | `controls.c`, `console.h` |
| 05 PAC-MAN | Puntos, paredes, fantasmas, vidas y resultado | `maze.c` |
| 06 Snake | Movimiento continuo, colisiones simultáneas y alimento | `snake.c` |
| 07 Render | Buffer lógico 64×64 y superposición de pausa | `draw.c` |
| 08 Integración | `console_frame()` y callbacks de plataforma | `platform.c`, `platform.h` |
| 09 Temporización | Relación conceptual CLK, tick, cálculo y visualización | propuesta de integración |

## Joystick y registros de hardware: contrato por confirmar

El grupo 2 prevé **MCP3008**, cuatro canales de ADC de diez bits (`CH0=J1_X`, `CH1=J1_Y`, `CH2=J2_X`, `CH3=J2_Y`) mediante SPI modo 0. Los botones son **GPIO**, no canales ADC. Base MMIO propuesta: `0x430000`; `STATUS` + `0x04`, `J1_X` + `0x08`, `J1_Y` + `0x0C`, `J2_X` + `0x10`, `J2_Y` + `0x14`, `BUTTONS` + `0x18`. El grupo 6 debe confirmar el mapa final. El bit `VALID` no garantiza por sí mismo una lectura atómica de los cuatro canales.

### Interfaz C que SÍ está implementada

- `JoystickInput` posee `uint16_t x[2]`, `y[2]` y `uint8_t buttons`.
- `joystick_direction()` resta **512**, compara magnitudes y devuelve ninguna dirección cuando **ambas magnitudes son ≤128**. Si `|x|>|y|`, decide izquierda/derecha; en otro caso arriba/abajo. Los empates son verticales. No utiliza máscaras `K_*` ni lecturas PS/2.
- En `console_step()` las direcciones del **menú** se detectan por flanco. El joystick debe volver al centro o cambiar dirección para que se registre una entrada nueva igual.
- Botón **J1** en menú: inicia el juego seleccionado; en resultados: repite último juego; en partida: alterna `paused`.
- Botón **J2** en partida: reinicia. **Ambos pulsadores** en partida o resultados: vuelven al menú. En menú la combinación no es un atajo equivalente: el código procesa J1 como inicio.
- En **modo individual solo el joystick 1** controla al personaje. En **PAC-MAN** el personaje no avanza sin dirección; **Snake** continúa avanzando en la dirección anterior si los ejes vuelven al centro.
- `console_frame()` acepta callback `read_joysticks`; si no existe, usa ambos ejes centrados (512) y botones liberados. Después llama `console_step`, `console_render`, `present` y `play_sound` opcionales.

### Lo que AÚN no está implementado o verificado

- La implementación real de `read_joysticks()` para obtener los registros MCP3008 por MMIO; también la captura/antirrebote de botones y la calibración física del centro/orientación de cada joystick.
- `main`, `crt0.S`, `linker.ld`, temporizador de pasos, buses, reproducción I2S y escritura real del framebuffer a la matriz LED 64×64.
- Los récords son de sesión; no están persistidos en EEPROM.
- **Inconsistencia real de render:** `firmware/draw.c` aún muestra las cadenas `ENTER INICIAR`, `ENTER REPETIR`, `P CONTINUAR`, `R REINICIAR` y `ESC MENU` pese al cambio físico a joysticks. Debe corregirse en otro commit y validar las capturas/test.

## Reloj (página 09, esquema conceptual)

La Tang Nano 9K dispone de un reloj de referencia de **27 MHz** (`≈37.04 ns`). El tick de lógica de **150 ms** es solamente una propuesta: implicaría 4 050 000 ciclos a 27 MHz si se contaran directamente. El reloj **SPI SCLK** del MCP3008, generado por el grupo 2, NO es el reloj base ni el tick de juego. El HUB75 refresca la imagen de forma independiente. Las anchuras de la página 09 no son tiempos de ejecución medidos ni una exportación de GTKWave.

## Cómo verificar y documentar el trabajo del estudiante

1. Abrir el draw.io en diagrams.net y repasar las nueve páginas, comparando con los archivos C indicados.
2. Ejecutar en `grupo5-juego-menu/`: `make test`, `make demo` y `make compare-html` (este último requiere Node.js). Guardar en el registro de avances los resultados **que realmente se obtengan**, sin atribuir al archivo pruebas que no se hicieron.
3. Revisar y explicar las diferencias entre firmware C ejecutado en Linux y el SoC RISC-V/FPGA aún por integrar.
4. Crear un commit documental identificable desde la cuenta de quien efectivamente revisa y aporta los diagramas.
