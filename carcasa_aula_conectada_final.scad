// Aula Conectada: carcasa para LCD, ESP32-S3, LED rojo/verde y buzzer.
// Medidas en mm. MEDIR los componentes antes de imprimir.
$fn = 64;
parte = "todo"; // "base", "tapa", "todo"
ancho = 160;
fondo = 110;
altura = 45;
pared = 3;
tapa_grosor = 3;
// Valores iniciales, no medidas verificadas de sus materiales.
lcd_ventana = [72, 25];
lcd_tornillos = [75, 31];
lcd_centro = [80, 51];
led_diametro = 5.3;
buzzer_diametro = 12; // medir el cuerpo del buzzer
buzzer_centro = [124, 86];
// Montaje SIN protoboard. Medir la placa completa, incluidos los USB.
esp_largo = 69;
esp_ancho = 29;
esp_origen = [22, 30];
esp_soporte_alto = 8; // espacio para pines y conexiones bajo la placa
// Los dos rieles sostienen los bordes; una brida sujeta la placa.
module soporte_esp() {
    for (y = [0, esp_ancho-3])
        translate([esp_origen[0], esp_origen[1]+y, pared])
            difference() {
                cube([esp_largo,3,esp_soporte_alto]);
                translate([esp_largo/2-3,-1,2]) cube([6,5,3]);
            }
}
module guia_cables(x,y) {
    translate([x,y,pared]) difference() {
        cube([10,8,6]);
        translate([2,-1,2]) cube([6,10,2]);
    }
}
module hueco(d, h) { translate([0,0,-1]) cylinder(d=d,h=h+2); }
module texto(s, tam) {
    linear_extrude(0.8) text(s,size=tam,halign="center",valign="center");
}
module base() {
    difference() {
        cube([ancho,fondo,altura]);
        translate([pared,pared,pared])
            cube([ancho-2*pared,fondo-2*pared,altura]);
        // Pasacables amplio: ambos USB pueden salir mediante cables cortos.
        translate([-1,30,12]) cube([pared+2,36,18]);
        translate([ancho-pared-1,35,14]) cube([pared+2,18,10]);
    }
    // Tuercas M3 en alojamientos hexagonales abiertos por arriba.
    for(x=[9,ancho-9], y=[9,fondo-9])
        translate([x,y,pared]) difference() {
            cylinder(d=12,h=altura-pared);
            translate([0,0,-1]) cylinder(d=3.5,h=altura);
            translate([0,0,altura-pared-3]) cylinder(d=6.6,h=4,$fn=6);
        }
    soporte_esp();
    // Pasos para bridas: ordenar cables y sujetar conexiones aisladas.
    guia_cables(106,32);
    guia_cables(106,65);
}
module tapa() {
    difference() {
        union() {
            cube([ancho,fondo,tapa_grosor]);
            translate([ancho/2,17,tapa_grosor]) texto("Aula Conectada",8);
            translate([37,97,tapa_grosor]) texto("VERDE",3.5);
            translate([63,97,tapa_grosor]) texto("ROJO",3.5);
            translate([124,101,tapa_grosor]) texto("BUZZER",3.5);
            // Aro interior para sujetar el buzzer con adhesivo removible.
            translate([buzzer_centro[0],buzzer_centro[1],-5]) difference() {
                cylinder(d=buzzer_diametro+5,h=5);
                translate([0,0,-1]) cylinder(d=buzzer_diametro+0.8,h=7);
                translate([-2,-buzzer_diametro, -1]) cube([4,2*buzzer_diametro,7]);
            }
        }
        translate([lcd_centro[0]-lcd_ventana[0]/2,lcd_centro[1]-lcd_ventana[1]/2,-1])
            cube([lcd_ventana[0],lcd_ventana[1],tapa_grosor+2]);
        // LCD bajo la tapa, con separadores: no apretar sobre el cristal.
        for(dx=[-1,1],dy=[-1,1])
            translate([lcd_centro[0]+dx*lcd_tornillos[0]/2,
                       lcd_centro[1]+dy*lcd_tornillos[1]/2,0]) hueco(3.3,tapa_grosor);
        for(x=[9,ancho-9],y=[9,fondo-9]) translate([x,y,0]) hueco(3.5,tapa_grosor);
        for(x=[37,63]) translate([x,86,0]) hueco(led_diametro,tapa_grosor);
        for(dx=[-3,0,3],dy=[-3,0,3])
            translate([buzzer_centro[0]+dx,buzzer_centro[1]+dy,0]) hueco(1.6,tapa_grosor);
    }
}
if(parte=="base") base();
else if(parte=="tapa") translate([0,0,5]) tapa();
else {
    color("silver") base();
    translate([0,fondo+20,5]) color("royalblue") tapa();
}
