# Aula Conectada

Proyecto escolar del curso 801 del Colegio Jorge Gaitan Cortes. El prototipo utiliza una ESP32-S3, una pantalla LCD 16x2, WiFi y una interfaz web local para visualizar informacion escolar.

## Problema

Los registros manuales de asistencia o salida pueden perderse, contener errores y ser dificiles de consultar. Aula Conectada propone organizar la informacion mediante un prototipo fisico y digital.

## Objetivo

Diseñar y construir un prototipo con ESP32-S3 que permita visualizar informacion escolar mediante una pantalla LCD, conexion WiFi y una interfaz web.

## Componentes

- ESP32-S3 N16R8
- Pantalla LCD 16x2 paralela
- Cables jumper y protoboard para pruebas
- Resistencias de 220 ohmios
- LED rojo y LED verde
- Buzzer
- Carcasa diseñada en OpenSCAD

## Carpetas

- `firmware`: programas para Arduino IDE.
- `hardware`: carcasa OpenSCAD, diagramas y conexiones.
- `docs`: guias tecnicas y pruebas.
- `images`: evidencias recientes del montaje.

## Conexion principal de la LCD

| LCD | ESP32-S3 |
|---|---|
| RS | GPIO21 |
| E | GPIO20 |
| D4 | GPIO19 |
| D5 | GPIO18 |
| D6 | GPIO17 |
| D7 | GPIO16 |
| VDD y BLA | 3V3 |
| GND, VO, RW y BLK | GND |

## Como cargar el programa

1. Abrir `firmware/aulaconectada_esp32.ino` en Arduino IDE.
2. Cambiar el nombre y la clave del WiFi en el archivo.
3. Cambiar `API_BASE_URL` por la IP actual del computador.
4. Seleccionar `ESP32S3 Dev Module`.
5. Seleccionar el puerto COM conectado.
6. Compilar y cargar el programa.

## Seguridad

El archivo del repositorio no contiene la clave real del WiFi. Cada integrante debe poner sus datos localmente y evitar subir contraseñas a un repositorio publico.

## Estado

La ESP32-S3 y la pantalla LCD fueron probadas durante el montaje. Los LED, el buzzer, la identificacion real de estudiantes y la base de datos en la nube quedan como funciones para una siguiente version.

## Integrantes

- Isabel Sofia Cattaneo Contreras
- Greysis Nicolles Torres Alvarado
- Dariana Hurtado Moreno
- Alejandro Mahecha Mantilla
- Hary Sofia Banqueth Arias

## Institucion

Colegio Jorge Gaitan Cortes - Tecnologia - Curso 801
- Proyecto desarrollado por el Equipo de Trabajo de 801
