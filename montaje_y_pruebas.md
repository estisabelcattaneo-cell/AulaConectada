# Montaje y pruebas del prototipo

## 1. Objetivo de la prueba

Validar que AulaConectada IA detecta estudiantes mediante señales BLE, clasifica su ubicacion aproximada y registra la asistencia en un panel web.

## 2. Materiales

| Elemento | Cantidad | Funcion |
|---|---:|---|
| ESP32 NodeMCU | 1 | Gateway IoT del aula |
| LED | 1 | Confirmacion visual |
| Buzzer | 1 | Confirmacion sonora |
| Protoboard | 1 | Conexion de componentes |
| Cables Dupont | 10 | Conexion electrica |
| Computador | 1 | Servidor y panel web |
| Beacon BLE o celular BLE | 1 por estudiante | Identificador de presencia |

## 3. Conexion fisica

| Componente | Conexion |
|---|---|
| LED positivo | GPIO 2 con resistencia de 220 ohm |
| LED negativo | GND |
| Buzzer positivo | GPIO 15 |
| Buzzer negativo | GND |

## 4. Preparacion del servidor

1. Abrir PowerShell en la carpeta del proyecto.
2. Ejecutar `python server\app.py`.
3. Entrar a `http://localhost:8080`.
4. Confirmar que aparecen los estudiantes registrados.

## 5. Prueba simulada

Ejecutar:

```powershell
python server\simulate_events.py
```

Resultado esperado:

- El panel registra eventos de asistencia.
- El resumen cambia los valores de presentes, ausentes y zonas comunes.
- La seccion Gateway muestra `ESP32-AULA-10`.

## 6. Prueba con ESP32

1. Cambiar WiFi e IP del servidor en el archivo `.ino`.
2. Cargar el firmware en el ESP32.
3. Abrir el monitor serial a 115200 baudios.
4. Verificar conexion WiFi.
5. Acercar un beacon BLE registrado.
6. Revisar que llegue un evento al panel.

## 7. Criterios de aceptacion

| Criterio | Cumple cuando |
|---|---|
| Conexion WiFi | El ESP32 obtiene IP |
| Comunicacion HTTP | El servidor recibe heartbeat |
| Deteccion BLE | El monitor serial muestra MAC y RSSI |
| Clasificacion | El panel muestra estado del estudiante |
| Alerta local | LED/buzzer responden al evento |

## 8. Limitaciones del prototipo

- Los celulares modernos cambian su MAC BLE por privacidad. Para una demo estable conviene usar beacons BLE configurables.
- RSSI no entrega una ubicacion exacta; sirve para proximidad aproximada.
- La clasificacion mejora si se toman muestras reales del aula y se ajustan los umbrales.

## 9. Mejoras futuras

- Registrar historial por fecha y exportar reportes.
- Agregar autenticacion para coordinadores.
- Usar Firebase o ThingSpeak para almacenamiento en la nube.
- Entrenar un modelo con datos reales de RSSI por zona.
- Instalar varios ESP32 en puerta, aula y pasillo para triangulacion basica.
