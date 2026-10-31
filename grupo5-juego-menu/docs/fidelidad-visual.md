# Comparación con el HTML original — revisión final

Referencia: `docs/referencia-html.html`. Alcance exclusivo: las llamadas de dibujo
sobre el canvas de **64×64**, sin carcasa, paneles, CSS ni controles exteriores.
El HTML se conserva sin modificaciones.

## Método y resultado reproducible

`make compare-html` compila `sim/render_reference.c` y ejecuta
`sim/compare_html.cjs` con Node.js (sin paquetes adicionales). Ejecuta el script
original en un contexto con DOM mínimo y rasteriza sus llamadas enteras fillRect.
Compara índices de color con el render C; no es una prueba del navegador real.

| Fixture | Píxeles comparados | Excluidos intencionalmente | Diferencias |
|---|---:|---:|---:|
| Menú inicial | 3008 | 1088: pie compacto | 0 |
| PAC-MAN individual, cuatro direcciones × dos fases | 4096 | 0 | 0 |
| PAC-MAN doble, cuatro direcciones × dos fases | 4096 | 0 | 0 |
| Snake individual, inicial y primer paso | 3584 | 512: HUD inferior ampliado | 0 |
| Snake doble, inicial y primer paso | 3584 | 512: HUD inferior ampliado | 0 |

PAC-MAN usa puntuación 67 en las fixtures para comprobar los dígitos corregidos.
Sus dos fases y cuatro direcciones se renderizan con posiciones fijas: los
4096 píxeles coinciden en las 16 fixtures de PAC-MAN, incluidos personajes,
fantasmas, paredes, puntos y HUD. No se afirma equivalencia de trayectorias/IA
ni de todos los estados posibles. Menú y Snake mantienen las exclusiones indicadas.

## Restauración de PAC-MAN

Se recuperan los jugadores amarillo/verde con boca orientada y los fantasmas
rojo/magenta con ojos blancos y pies alternados, directamente del HTML.
El menú conserva la grafía compacta original «PACMAN»; resultados y documentación
usan «PAC-MAN». La lógica funcional y el render de Snake no cambian.

## Mejoras funcionales conservadas

- Fuente 3×5: los dígitos 6 y 7 ahora coinciden con el original.
- PAC-MAN individual conserva la energía en la casilla donde aparecería J2.
- El píxel cian indica que ambos jugadores comparten casilla.
- En individual, flechas y WASD controlan J1 en ambos juegos. Ante teclas
  simultáneas, la prioridad fija es arriba, derecha, abajo, izquierda.
- PAC-MAN verifica el choque antes de recoger energía, como el HTML.
- Snake actualiza al superviviente y su puntuación antes de resolver un choque
  del rival en el mismo paso; actualiza también el indicador interno alive.
- El generador verifica explícitamente modo y cantidad de jugadores de las
  capturas principales, y comprueba la paleta sin ocultar errores con una máscara.

## Diferencias intencionales conservadas

| Aspecto | HTML | Firmware C |
|---|---|---|
| Identidad laberinto | Personajes clásicos | PAC-MAN: jugadores y fantasmas originales, dos fases animadas |
| Menú | Dos pantallas: juego, luego jugadores; flechas alternan | Una pantalla; arriba/abajo elige juego, izquierda/derecha jugadores; 1/2 inicia directamente |
| Inicio por Enter | Confirma cada etapa | Inicia con selección visible |
| Laberinto | Movimiento continuo, giro pendiente | Movimiento solo con tecla mantenida, sin giro en cola |
| Enemigos | Buscan al más cercano, con decisiones pseudoaleatorias | Cada fantasma persigue al jugador asignado; ambos a J1 en individual; desempate fijo |
| Controles | Eventos de pulsación | Máscara de teclas mantenidas; acciones de menú por flanco |
| Pausa/salida | Escape pausa primero; Enter o P reanuda; R vuelve al menú | P pausa/reanuda; Escape va al menú; R reinicia; instrucciones visibles |
| Resultados | Mensaje y récord | Juego, resultado, puntuaciones, récord de sesión e instrucciones |
| Récords | localStorage, separados por juego y número de jugadores | RAM de sesión, un máximo por juego compartido entre individual/doble |
| Snake HUD | Dos cifras mínimas; sin etiqueta individual | Tres cifras y etiqueta SOLO; saturación visual para evitar desbordar |
| Snake longitud | Puede llenar la arena y ganar | Máximo 128 segmentos por serpiente para limitar RAM; continúa puntuando, sin victoria por llenar arena |
| Tiempo | 130 ms laberinto / 155 ms Snake | Paso abstracto; 150 ms propuestos, temporizador aún pendiente |

Se conservan las paredes azules y bordes cian, ocho colores exactos, posiciones
iniciales, retícula discreta Snake, ojos blancos, cuerpos tramados, alimento rojo,
crecimiento, tres vidas compartidas, cooperación y colisiones competitivas.

## Capturas actuales

`make demo` produce PPM ampliados 6× sin interpolación. PNG revisados en
`docs/capturas/`: `menu.png`, `pacman.png`, `snake.png`, `pausa.png`,
`snake-solo.png`, `resultado.png` y mosaico `resumen.png`.
`pacman.png` muestra ahora el estado inicial actual con dos jugadores.
El archivo heredado `nexo.png` se actualiza con la misma imagen para conservar
los archivos existentes; el mosaico también muestra PAC-MAN.
Las aserciones del generador evitan etiquetar PAC-MAN como Snake o viceversa.

## Verificación y límites

Pasaron `make test`, `make demo`, `make compare-html`, `make sanitize` y
`make size-rv32`. Compilación final sin avisos (warnings tratados como errores).
Las regresiones incluyen energía individual, control con flechas, dos fases de
animación, casilla compartida y puntuación del superviviente en el turno final.
Se mantienen las comprobaciones de paleta y centinelas en 1200 renders.
ASan/UBSan pasan; LeakSanitizer sigue desactivado por el entorno ptrace.
El primer intento del comparador encontró EPERM al lanzar un subproceso Node;
se verificó después con ejecución autorizada fuera del sandbox.

No se probó en FPGA. Pendientes: contratos MMIO, temporizador, arranque y linker,
main embebido, libgcc, integración y validación del SoC, síntesis y prueba en placa.
