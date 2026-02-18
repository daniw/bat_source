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