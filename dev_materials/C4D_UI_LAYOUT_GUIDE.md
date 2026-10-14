# Cinema 4D UI & Layout Špecifikácia

Tento dokument detailne definuje vizuálny layout, rozmiestnenie okien a správanie používateľského rozhrania, ktoré musí projekt **CineMod** presne kopírovať.

---

## 1. Schéma rozloženia obrazovky (Default Standard Layout)

```text
┌────────────────────────────────────────────────────────────────────────────────────────┐
│ Main Menu Bar (File, Edit, Create, Select, Tools, Mesh, Animate, Render, Window, Help) │
├────────────────────────────────────────────────────────────────────────────────────────┤
│ Top Tool Bar: [Undo][Redo] | [Select][Move][Rotate][Scale] | [Render][RenderPic][Set]  │
├────┬────────────────────────────────────────────────────────────┬───────────────────────┤
│    │ 3D Viewport Toolbar: [View] [Cameras] [Display] [Options] │ Object Manager        │
│    ├───────────────────────────────────────────────────────────┤  - Search bar         │
│ L  │                                     [HUD: 1][2][3][Quad]  │  - Object Tree:       │
│ e  │                                                           │    • [o o] Cube [T1]  │
│ f  │                                                           │    • [o o] Light      │
│ t  │               3D VIEWPORT                                 │    • [o o] Camera     │
│    │               (Ground Grid & 3D Gizmo)                    ├───────────────────────┤
│ T  │                                                           │ Attribute Manager     │
│ o  │                                                           │  - Tabs: Basic, Coord │
│ o  │                                                           │  - Parameters:        │
│ l  │                                                           │    Size X: [200 cm  ] │
│ s  │                                                           │    Size Y: [200 cm  ] │
│    │                                                           │    Size Z: [200 cm  ] │
├────┴───────────────────────────────────────────────────────────┴───────────────────────┤
│ Timeline / Transport Controls: [|<] [<] [Play] [>] [>|] [● Key]  Frame: [  0 / 90]     │
├────────────────────────────────────────┬───────────────────────────────────────────────┤
│ Material Manager                       │ Coordinates Manager                           │
│  [New Material] [Mat Swatches...]      │  P: [X: 0 ] [Y: 0 ] [Z: 0 ] cm  [Object/World]│
│                                        │  S: [X: 1 ] [Y: 1 ] [Z: 1 ]                   │
│                                        │  R: [H: 0°] [P: 0°] [B: 0°]       [Apply]     │
└────────────────────────────────────────┴───────────────────────────────────────────────┘
```

---

## 2. Podrobný rozpis jednotlivých manažérov

### A. Object Manager (Správca objektov)
* **Umiestnenie:** Vpravo hore.
* **Hierarchické zobrazenie:** Stromová štruktúra s možnosťou rozbaľovania/zbaľovania (expand/collapse šípky).
* **Indikátory viditeľnosti ("Traffic Lights"):**
  * Každý riadok obsahuje dve zvislé bodky v stĺpci pred názvom:
    * **Horná bodka:** Viditeľnosť v Editore (Viewport).
    * **Dolná bodka:** Viditeľnosť pri Renderovaní.
  * **Stavy:**
    * *Sivá (Default / Dedené od rodiča)*
    * *Zelená (Vynútená viditeľnosť)*
    * *Červená (Vynútené skrytie)*
  * Kliknutie na bodku cykluje stavy: Sivá -> Zelená -> Červená -> Sivá.
* **Ikona objektu:** Grafická ikona typu (kocka, guľa, kamera, svetlo, null objekt...).
* **Názov objektu:** Dvojklik umožňuje premenovanie.
* **Lišta tagov (Tag Strip):** Vpravo od názvu sa vodorovne zobrazujú ikony pripojených tagov (Phong, Materiály, Ochrana). Kliknutie na tag ho označí a zobrazí jeho vlastnosti v Attribute Manageri.

### B. Attribute Manager (Inšpektor vlastností)
* **Umiestnenie:** Vpravo dole (pod Object Managerom).
* **Záložkový systém (Tabs):**
  * **Záložka "Basic":**
    * Názov (`Name`), Vrstva (`Layer`), Viditeľnosť (`Visible in Editor`, `Visible in Render`), Zapnuté (`Enabled`).
  * **Záložka "Coord." (Súradnice):**
    * Pozícia: `P.X`, `P.Y`, `P.Z`
    * Mierka: `S.X`, `S.Y`, `S.Z`
    * Rotácia: `R.H`, `R.P`, `R.B` (v stupňoch)
  * **Záložka "Object" (alebo podľa typu Tagu/Materiálu):**
    * Pre Kocku: `Size X, Y, Z`, `Segments X, Y, Z`, `Fillet` (zaoblenie), `Fillet Radius`.
    * Pre Guľu: `Radius`, `Segments`, `Type` (Standard, Hemisphere, Icosahedron).
* **Interaktivita numerických polí (Draggable scrubbers):**
  * Kliknutie a ťahanie myšou na malých šípkach vedľa vstupného poľa umožňuje plynulé zvyšovanie/znižovanie číselných hodnôt.

### C. 3D Viewport (Pracovná plocha)
* **Horné menu viewportu:**
  * `View`: Redraw, Frame Selected (`S`), Frame All (`H`), Default View.
  * `Cameras`: Perspective, Top, Front, Right, Use Camera.
  * `Display`: Gouraud Shading (`N~A`), Quick Shading (`N~C`), Wireframe (`N~G`), Isoparms.
  * `Options`: Zobrazenie textúr, tieňov, osvetlenia.
  * `Filter`: Filtrovanie zobrazenia (zobraziť/skryť mriežku, svetlá, kamery, vodiace čiary).
* **HUD Navigačné ikony (pravý horný roh):**
  * Ikona 1 (Kríž so šípkami): Pan / Posun.
  * Ikona 2 (Šípky od seba/k sebe): Dolly / Zoom.
  * Ikona 3 (Kruh so šípkami): Orbit / Rotácia.
  * Ikona 4 (Štyri štvorce): Prepínač 1-okno vs. 4-okná (Perspektíva, Horný, Predný, Bočný pohľad).

### D. Coordinates Manager (Správca súradníc)
* **Umiestnenie:** Vpravo dole pod viewportom.
* Poskytuje okamžitý číselný prehľad o transformačnom stave aktívneho objektu.
* Prepínač súradnicového systému: **World** (svetový) vs. **Object** (lokálny).
* Tlačidlo **Apply**: Potvrdenie manuálne zadaných súradníc.

### E. Material Manager (Správca materiálov)
* **Umiestnenie:** Vľavo dole pod viewportom.
* Zobrazuje miniatúry (swatches) vytvorených materiálov.
* Dvojklik na prázdnu plochu: Vytvorenie nového materiálu.
* Dvojklik na materiál: Otvorenie podrobného okna Material Editora (Farba, Odrazivosť/Reflectance, Drsnosť/Roughness, Svietivosť/Luminance, Reliéf/Bump).
* Podpora pretiahnutia (Drag & Drop) materiálu priamo na objekt vo viewporte alebo v Object Manageri.

### F. Timeline / Transport Bar (Časová os a prehrávanie)
* Posuvník aktuálneho snímku (Playhead / Scrubber).
* Textové polia: Aktuálny snímok (napr. `0 F`), Začiatok animácie (`0 F`), Koniec animácie (`90 F`).
* Tlačidlá ovládania prehrávania: Prehrať vpred/vzad, skok na začiatok/koniec, krok o jeden snímok.
* Tlačidlo **Record Keyframe** (klávesová skratka `F9`): Uloženie kľúčovej snímky pre vybraný objekt.

---

## 3. Farebná schéma a vizuálny štýl (C4D Dark Theme)

| Prvok UI | Hex kód | Popis |
| :--- | :--- | :--- |
| **Pozadie okien (Window Bg)** | `#2B2B2B` | Tmavosivá základná farba |
| **Hlavičky a panely (Header Bg)** | `#363636` | O niečo svetlejšia sivá |
| **Titulkové pruhy dokovaných okien** | `#202020` | Tmavý kontrastný pruh |
| **Vstupné polia (Input Fields)** | `#1E1E1E` | Veľmi tmavé pozadie pre čitateľnosť textu |
| **Farba textu** | `#DDDDDD` | Svetlosivá, jemná pre oči |
| **Zvýraznenie výberu (Selection Accent)** | `#E59500` / `#FF9900` | Charakteristická oranžovo-žltá farba výberu v C4D |
| **3D Os X (Červená)** | `#E74C3C` | Smer X v gizme a koordinátoch |
| **3D Os Y (Zelená)** | `#2ECC71` | Smer Y v gizme a koordinátoch |
| **3D Os Z (Modrá)** | `#3498DB` | Smer Z v gizme a koordinátoch |
| **3D Mriežka (Grid lines)** | `#3A3A3A` | Decentné tmavosivé linky podlahy |
| **Hlavná mriežková os (Center Axes)** | `#4A4A4A` | Zvýraznený stred sveta |
