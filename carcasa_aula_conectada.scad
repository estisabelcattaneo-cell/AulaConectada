// ====================================================================
// PROYECTO: Aula Conectada / Gatekeeper 32
// CARCASA 3D EN OPENSCAD
// Version pensada para: ESP32-S3 + LCD 16x2 + cables + WiFi/web
// LED y buzzer quedan como huecos opcionales para futuras mejoras.
// ====================================================================

$fn = 48;

// Cambia esto para exportar STL por partes:
// "todo" = ver base y tapa separadas
// "base" = solo base
// "tapa" = solo tapa
parte = "todo";

// --------------------------------------------------------------------
// 1. MEDIDAS GENERALES DE LA CAJA
// --------------------------------------------------------------------
ancho_caja = 130;
prof_caja  = 95;
alto_base  = 38;
pared      = 2.5;

// Tornillos M3
radio_tornillo_m3 = 1.7;     // agujero para tornillo M3
radio_poste_m3    = 4.2;     // poste o torre interna

// --------------------------------------------------------------------
// 2. MEDIDAS DE LA PANTALLA LCD 16x2 PARALELA
// --------------------------------------------------------------------
// Modulo LCD normal: placa aprox. 80 x 36 mm
lcd_placa_x = 80;
lcd_placa_y = 36;

// Ventana visible: un poco mas grande que la zona de texto
lcd_ventana_x = 72;
lcd_ventana_y = 25;

// Distancia aproximada entre huecos del LCD
lcd_huecos_x = 75;
lcd_huecos_y = 31;

// Posicion del LCD sobre la tapa
lcd_centro_x = ancho_caja / 2;
lcd_centro_y = 52;

// --------------------------------------------------------------------
// 3. MEDIDAS PARA ESP32-S3
// --------------------------------------------------------------------
// No todas las ESP32-S3 tienen huecos de montaje iguales.
// Por eso usamos una "cama" con rieles, no tornillos exactos.
esp32_largo = 62;
esp32_ancho = 34;
esp32_x = 18;
esp32_y = 20;
esp32_z = pared;

// --------------------------------------------------------------------
// 4. OPCIONES FUTURAS
// --------------------------------------------------------------------
usar_huecos_led = true;       // deja 2 huecos en la tapa para LED futuro
usar_hueco_buzzer = true;     // deja una rejilla circular para buzzer futuro

// --------------------------------------------------------------------
// MODULOS BASICOS
// --------------------------------------------------------------------
module poste_m3(alto) {
    difference() {
        cylinder(r = radio_poste_m3, h = alto);
        translate([0, 0, -1])
            cylinder(r = radio_tornillo_m3, h = alto + 2);
    }
}

module agujero_m3(alto) {
    translate([0, 0, -1])
        cylinder(r = radio_tornillo_m3 + 0.2, h = alto + 2);
}

module texto_tapa() {
    linear_extrude(height = 1.2)
        text("Aula Conectada",
             size = 8,
             font = "Arial:style=Bold",
             halign = "center",
             valign = "center");
}

// --------------------------------------------------------------------
// BASE
// --------------------------------------------------------------------
module base() {
    difference() {
        // bloque exterior
        cube([ancho_caja, prof_caja, alto_base]);

        // hueco interior
        translate([pared, pared, pared])
            cube([ancho_caja - 2*pared, prof_caja - 2*pared, alto_base]);

        // abertura para cable USB-C de la ESP32-S3
        // Se deja en el lado izquierdo para conectar cable sin abrir caja.
        translate([-1, esp32_y + 10, 8])
            cube([pared + 2, 18, 10]);

        // ranura extra para cables de prueba
        translate([ancho_caja - pared - 1, 20, 8])
            cube([pared + 2, 22, 12]);
    }

    // postes de esquinas para atornillar la tapa
    translate([8, 8, pared]) poste_m3(alto_base - pared);
    translate([ancho_caja - 8, 8, pared]) poste_m3(alto_base - pared);
    translate([8, prof_caja - 8, pared]) poste_m3(alto_base - pared);
    translate([ancho_caja - 8, prof_caja - 8, pared]) poste_m3(alto_base - pared);

    // cama para ESP32-S3: rieles sencillos para que no se mueva
    color([0.65, 0.65, 0.65]) {
        translate([esp32_x, esp32_y, esp32_z])
            cube([esp32_largo, 3, 5]);
        translate([esp32_x, esp32_y + esp32_ancho - 3, esp32_z])
            cube([esp32_largo, 3, 5]);
        translate([esp32_x, esp32_y, esp32_z])
            cube([3, esp32_ancho, 5]);
        translate([esp32_x + esp32_largo - 3, esp32_y, esp32_z])
            cube([3, esp32_ancho, 5]);
    }

    // soporte sencillo para cables/protoboard pequeno
    color([0.75, 0.75, 0.75])
        translate([88, 18, pared])
            cube([25, 52, 4]);
}

// --------------------------------------------------------------------
// TAPA SUPERIOR
// --------------------------------------------------------------------
module tapa() {
    grosor_tapa = 3;

    difference() {
        union() {
            cube([ancho_caja, prof_caja, grosor_tapa]);

            // borde interno para que la tapa encaje sobre la base
            translate([pared, pared, -3])
                difference() {
                    cube([ancho_caja - 2*pared, prof_caja - 2*pared, 3]);
                    translate([pared, pared, -1])
                        cube([ancho_caja - 4*pared, prof_caja - 4*pared, 5]);
                }

            // texto en relieve
            translate([ancho_caja/2, 16, grosor_tapa])
                texto_tapa();
        }

        // ventana para ver la pantalla LCD
        translate([lcd_centro_x - lcd_ventana_x/2,
                   lcd_centro_y - lcd_ventana_y/2,
                   -1])
            cube([lcd_ventana_x, lcd_ventana_y, grosor_tapa + 3]);

        // huecos para tornillos de la tapa
        translate([8, 8, 0]) agujero_m3(grosor_tapa);
        translate([ancho_caja - 8, 8, 0]) agujero_m3(grosor_tapa);
        translate([8, prof_caja - 8, 0]) agujero_m3(grosor_tapa);
        translate([ancho_caja - 8, prof_caja - 8, 0]) agujero_m3(grosor_tapa);

        // huecos de montaje del LCD
        translate([lcd_centro_x - lcd_huecos_x/2, lcd_centro_y - lcd_huecos_y/2, 0]) agujero_m3(grosor_tapa);
        translate([lcd_centro_x + lcd_huecos_x/2, lcd_centro_y - lcd_huecos_y/2, 0]) agujero_m3(grosor_tapa);
        translate([lcd_centro_x - lcd_huecos_x/2, lcd_centro_y + lcd_huecos_y/2, 0]) agujero_m3(grosor_tapa);
        translate([lcd_centro_x + lcd_huecos_x/2, lcd_centro_y + lcd_huecos_y/2, 0]) agujero_m3(grosor_tapa);

        // huecos opcionales para LEDs
        if (usar_huecos_led) {
            translate([30, 80, -1]) cylinder(r = 2.6, h = grosor_tapa + 3);
            translate([44, 80, -1]) cylinder(r = 2.6, h = grosor_tapa + 3);
        }

        // rejilla opcional para buzzer
        if (usar_hueco_buzzer) {
            for (i = [-1:1]) {
                translate([103 + i*5, 80, -1])
                    cylinder(r = 1.4, h = grosor_tapa + 3);
            }
            for (i = [-1:1]) {
                translate([103 + i*5, 86, -1])
                    cylinder(r = 1.4, h = grosor_tapa + 3);
            }
        }
    }
}

// --------------------------------------------------------------------
// VISTA FINAL
// --------------------------------------------------------------------
if (parte == "base") {
    base();
} else if (parte == "tapa") {
    tapa();
} else {
    color([0.88, 0.88, 0.88]) base();
    translate([0, prof_caja + 20, 0])
        color([0.1, 0.35, 0.9]) tapa();
}
