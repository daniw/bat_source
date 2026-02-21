# Übersicht der Mechanik zur 60V-Quelle

## step-Dateien der PCBs

Dateien von Dani bereitgestellt am 15.02.2026. Vereinfachte Modelle, für bessere Handhabung.

"D:\GIT_Projekte\bat_source\mech_Samuel_v2\PCB_step_files_leer_geraeumt\Hauptplatine\PS1-1BA_3D STEP_2026-02-16.step"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\PCB_step_files_leer_geraeumt\UI\PS1-2BA_3D STEP_2026-02-16.step"

D:\GIT_Projekte\bat_source
D:\GIT_Projekte\bat_source\mech_Samuel_v2
D:\GIT_Projekte\bat_source\mech_Samuel_v2\01_Hauptplatine
D:\GIT_Projekte\bat_source\mech_Samuel_v2\02_UserInterface
D:\GIT_Projekte\bat_source\mech_Samuel_v2\99_PCB_step_files_leer_geraeumt\Hauptplatine
D:\GIT_Projekte\bat_source\mech_Samuel_v2\99_PCB_step_files_leer_geraeumt\UserInterface

# Übersicht bestehender Mechanik-Teile

"D:\GIT_Projekte\bat_source\mech"

Datei | Inhalt
----  | ----
00_bat_source.FCStd               | Assembly vom gesamten Gehäuse
00_bat_source.step                | Assembly vom gesamten Gehäuse
Battery_Holder.3mf                | Druck-Datei für die Halterungen der Zellen und die banana-plug-Führungen
Battery_Holder.FCStd              | FreeCAD-Datei für die Halterung der Zellen.
CBOX11004.FCStd                   | FreeCAD-Datei vom Gehäuse. Hauptteil
CBOX11004-Körper.step             | Gehäuse-Grundform. Hauptteil
Holder_Banana_Plugs.FCStd         | FreeCAD-Datei für die banana-plug-Führungen. Hier schon mit den Bohrungen
Holder_Banana_Plugs.stp           | Äußere Führung für die banana-plugs
Holder_Banana_Plugs_Drill.stp     | Äußere Führung für die banana-plugs mit Bohrungen
Knob.3mf                          | Druckdatei für irgendwelche knobs. Auf dem Poti??
Knob.FCStd                        | FreeCAD-Datei für irgendeinen knob. Auf dem Poti??
Lid_CSACTT004.FCStd               | FreeCAD-Datei für den Deckel
Transformer_Spacer_0.5mm.FCStd    | FreeCAD-Datei für einen Spacer


# Dateien umbenennen

"D:\GIT_Projekte\bat_source\mech_Samuel_v2\00_main_assembly.asm"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\01_housing.prt"

"D:\GIT_Projekte\bat_source\mech_Samuel_v2\banana_position_skeleton.prt"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\battery_holder-holder_cut.prt"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\cbox11004-körper.prt"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\config.pro"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\holder_banana_plugs_drill.prt"
"D:\GIT_Projekte\bat_source\mech_Samuel_v2\pcb_assembly.asm"


Länge Platine: 154mm beim Ausschnitt


## Montage und Zusammenbau

- Die Knöpfe und "Regler" werden am Deckel verschraubt.
- Danach wird die UI-Platine am Deckel und den Knöpfen verlötet.

- Die Bananen-Plugs werden in das Gehäuse geschraubt.
- Dann wird die Hauptplatine ins Gehäuse gesetzt und mit den Bananen-Steckern verlötet.
- Hauptplatine wird mit dem Gehäuse verschraubt.

- Deckel wird auf das Gehäuse gesetzt, die Stiftleiste stellt dann zwischen der Hauptplatine und der UI-Platine die Verbindung her. Deckel wird mit vier Schrauben mit dem Gehäuse verschraubt.

# Aufbau CAD-Modell

CAD-Modell wird ähnlich aufgebaut, wie es auch in der Realität gemacht wird.

Folgend zwei Darstellungen des Aufbaus als Flowchart.

## Aufbau, Variante 1
```mermaid
flowchart TD
    A[Haupt-Assembly, 00_main_assembly1.asm]

    A --> B[Gehäuse-Assembly, 00a_housing_assembly.asm]
	
    B --> B1[Gehäuse, 01_housing.prt]
    B --> E[Hauptplatinen-Assembly, 10_main_pcb_assembly.asm]
    B --> B2[Deckel Sicherung, 06_fuse_lid.prt]

    A --> C[Deckel-Assembly, 00b_lid_assembly.asm]
	
    C --> C1[Deckel, 02_lid.prt]
    C --> C2[UI-Platine]
	
	A --> D[Dichtung, 03_seal.prt]
	
	E --> E1[Hauptplatine]
	E --> E2[Zellenhalter, 2x, 05_cell_holder.prt]

````

## Anpassungen im CAD-Modell

* Hauptplatine wird mit vier metrischen Schrauben befestigt.

* In das Gehäuse müssen vier Hülsen mit Gewinde eingeschmolzen werden.

* Welche Schrauben können sinnvoll genutzt werden? Bei Bedarf müssen die Röhren-Füße verlängert und die Zellhalter niedriger werden, damit kürzere Schrauben verwendet werden können.

* 3D-Modell DIN M3x25 und Einschmelzgewinde suche, runterladen und einbauen.

https://www.3djake.de/3djake/gewindeeinsaetze-50er-set?sai=9419

* Vier Schrauben raussuchen für die Verbindung zwischen Deckel und Gehäuse
* Dichtung zwischen Deckel und Gehäuse auf die Innenseite der Schrauben verlegen. Schrauben-Rohr viel weiter nach außen, direkt in die Ecke. Dasselbe gilt für den Deckel der Sicherung.

* Deckel der Sicherung so machen, dass man damit die Sicherung reindrücken kann. Zwischen Deckel und Sicherung ein Band machen, mit dem man mit dem Deckel die Sicherung rausziehen kann.

* Deckel für die Sicherung muss viel größer werden. Nachbauen, drucken und testen.

* Rundung unten außen machen, dass man die PS besser greifen kann. Es sollten mindestens 10mm Radius sein, mehr wäre besser. Was macht die Wandstärke damit? Welche Rundung geht dann noch innen? Wie passt das mit den Zellenhaltern?

* Schrauben zwischen Hauptplatine und Gehäuse: M3x40, https://www.bossard.com/ch-de/eshop/schrauben-mit-innenantrieb/zylinderschrauben-mit-innensechskant-mit-schaft/p/4/

* Andere Schraube, M3x25: https://www.bossard.com/ch-de/eshop/schrauben-mit-innenantrieb/zylinderschrauben-mit-innensechskant-ohne-schaft/p/272/

* Oberkante vom Encoder definiert die Innenseite vom Deckel. Die Taster definieren die Außenkante vom Deckel und liegen auf dem Deckel auf.

* UI-Platine am Deckel muss von innen an den Deckel geschraubt werden. Aufgrund der geringeren Kräfte sind hier Kunststoff-Schrauben ausreichend.

* Auszugkraft der Stiftleiste: 0.6 N = 60 gramm pro Pin. Gesamtkraft: 24x0.6N = 14.4N = 1.440 kg.

* Auf die LED der UI-Platine einen Lichtleiter machen. Einfacher Zylinder mit Halbkugel oben drüber. Dazu dann vier Bohrungen in den Deckel machen. Müssen dann eingeklabt werden. Die Lichtleiter haben keinen direkten Kontakt zu den LED.

* Dichtring mit 12mm-Innendurchmesser unter die Bananen-Stecker machen, damit die gedichtet sind.

* Stecker: Spannscheibe und Ringmutter mit ins Modell reinnehmen.

* Gehäuse soll aus ABS gedruckt werden. Grobe Abschätzung der Kosten machen. Basierend auf bambu-Filament. https://eu.store.bambulab.com/de/products/abs-filament

* Ungefähre Abmessungen für die Verschraubung des Deckels: 166x77,5 und 4.5x7.5

* Abmessungen der Taster und vom Poti

	* Taster: Durchmesser 12mm; Breite zwischen den beiden Abflachungen: 11.19 (Länge Abflachung: 4.33404)
	* Poti: Länge: 13.75, Breite 11.70, Durchmesser unten 7, Höhe Zylinder 7, Nase 2x0.8, Abstand Nase 0.1

* Dichtungen am Deckel für die Sicherung: Gerundete Varianten wegmachen. Es bleibt die ganz gerade Variante übrig und wird verwendet. Wandstärke nach innen hin von 0.5mm auf 1mm erhöhen. An der Lid müssen noch für die beiden Schrauben Vertiefungen gemacht werden, wo die Schrauben-Köpfe reinpassen.

## Dichtung Abmessungen

* Radius Ecken 3mm

* Breite 1.5mm

* Dichtung liegt mittig. Abstand zur Innenkante 0.75mm

## Dichtunge Abmessungen IMD-Tester

* Box hat Nase, die die Dichtung im Deckel drückt. Kante hat Breite von 1.5mm. Ende sauber auf der Innenseite vom Gehäuse. Kante ist 1.5mm hoch.

* Dichtung liegt im Deckel. Fuge für die Dichtung ist 2.5mm breite. Dichtung selbst ist auch 2-2.5mm breit.

## Weiteres Vorgehen in der Konstruktion:

* Gehäuse nochmal neu machen. Wichtig: Rundungen unten außen machen zum besseren Anheben.

* Dann die Füße für den Deckel machen und die Dichtung auf die Innenseite der Schrauben legen.

* Zylinder-Röhren-Füße anpassen für Einschmelzgewinde für die Platine.

* Bohrungen in den Zellenhaltern etwas größer machen, dass safe eine M3 durchgeht.

* Bohrungen in die Zellenhalter machen, damit man die Hauptplatine und die Zellenhalter verschrauben kann.

* Dichtung und Deckel für die Sicherung anpassen. 

* Passende Schrauben suchen und als Modell einbauen.

* Überlegen, wie man ein Spiel in alle drei Raumrichtungen einbauen kann, siehe Diskussion mit Luise. Es dürfen keine Kräfte auf den Viel-Pin-Stecker ausgeübt werden. Und Fertigungstoleranzen müssen ausgeglichen werden können.

* Reste der Dichtungen an der Fuse-Lid aufräumen. Dateien passend umbenennen.

## Aufbau, Variante 2


```mermaid
flowchart TD
    A[Haupt-Assembly]

    subgraph GA [Gehäuse-Assembly]
        G1[Gehäuse]
        G2[Hauptplatine]
        G3[Dichtung]
    end

    subgraph DA [Deckel-Assembly]
        D1[Deckel]
        D2[UI-Platine]
    end

    A --> GA
    A --> DA

````

Kleinst-möglicher Abstand zwischen den beiden Platinen sind 9.5mm. Also etwas mehr Abstand einplanen.