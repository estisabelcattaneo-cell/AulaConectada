# AulaConectada IA - conexiones reales del prototipo

Este montaje usa la pantalla LCD 16x2 de la foto, con 16 pines. Esa pantalla no es I2C de 4 cables, por eso se conecta en modo paralelo.

## Archivos importantes

- Codigo final: `C:\Users\usuario\OneDrive\Documentos\AulaConectadaIA\firmware\aulaconectada_esp32\aulaconectada_esp32.ino`
- Codigo de prueba rapida: `C:\Users\usuario\OneDrive\Documentos\AulaConectadaIA\firmware\aulaconectada_esp32_prueba\aulaconectada_esp32_prueba.ino`
- Diagrama PNG: `C:\Users\usuario\OneDrive\Documentos\AulaConectadaIA\docs\DIAGRAMA_REAL_ESP32S3_LCD_PARALELO.png`
- Diagrama SVG: `C:\Users\usuario\OneDrive\Documentos\AulaConectadaIA\docs\DIAGRAMA_REAL_ESP32S3_LCD_PARALELO.svg`

## Conexiones de alimentacion

| Pieza | Pin | Va conectado a |
| --- | --- | --- |
| ESP32-S3 | GND | Linea GND de la protoboard |
| ESP32-S3 | 5Vin | Linea +5V de la protoboard |
| LCD | 1 GND | GND |
| LCD | 2 VCC | +5V |
| LCD | 3 VO | GND |
| LCD | 5 RW | GND |
| LCD | 15 BLA | +5V pasando por resistencia de 220 ohmios |
| LCD | 16 BLK | GND |

## Conexiones de la pantalla LCD

| LCD | ESP32-S3 |
| --- | --- |
| Pin 4 RS | GPIO21 |
| Pin 6 E | GPIO20 |
| Pin 11 D4 | GPIO19 |
| Pin 12 D5 | GPIO18 |
| Pin 13 D6 | GPIO17 |
| Pin 14 D7 | GPIO16 |

Los pines LCD 7, 8, 9 y 10, que son D0, D1, D2 y D3, se dejan sin conectar.

## Conexiones de luces y buzzer

| Pieza | Conexion |
| --- | --- |
| LED verde | GPIO2 -> resistencia 220 ohmios -> pata larga del LED. Pata corta -> GND |
| LED rojo | GPIO4 -> resistencia 220 ohmios -> pata larga del LED. Pata corta -> GND |
| Buzzer 2 patas | Pata + -> GPIO15. Pata - -> GND |

## Orden recomendado para armar

1. Conectar GND de la ESP32 a la linea GND de la protoboard.
2. Conectar 5Vin de la ESP32 a la linea +5V de la protoboard.
3. Conectar la pantalla LCD completa.
4. Conectar LED verde con su resistencia.
5. Conectar LED rojo con su resistencia.
6. Conectar buzzer de dos patas.
7. Subir primero el codigo de prueba rapida.
8. Si pantalla, LEDs, buzzer y pagina responden bien, subir el codigo final.

## Prompt para pedir otra imagen a una IA

Crear un diagrama tecnico claro de conexion para un prototipo escolar llamado AulaConectada IA. Mostrar una ESP32-S3 con dos puertos USB en el centro, una pantalla LCD 16x2 de 16 pines abajo, un LED verde, un LED rojo, dos resistencias de 220 ohmios, un buzzer activo de dos patas, protoboard y cables jumper. La pantalla NO es I2C. Conectar: LCD 1 GND a GND, 2 VCC a 5V, 3 VO a GND, 4 RS a GPIO21, 5 RW a GND, 6 E a GPIO20, 11 D4 a GPIO19, 12 D5 a GPIO18, 13 D6 a GPIO17, 14 D7 a GPIO16, 15 BLA a 5V con resistencia de 220 ohmios, 16 BLK a GND. Dejar LCD D0-D3 sin conectar. LED verde: GPIO2 a resistencia 220 ohmios a pata larga del LED, pata corta a GND. LED rojo: GPIO4 a resistencia 220 ohmios a pata larga del LED, pata corta a GND. Buzzer de dos patas: positivo a GPIO15 y negativo a GND. Todos los GND deben estar unidos. Dibujar los cables llegando exactamente a cada pin, sin cables flotando, con etiquetas grandes y legibles.
