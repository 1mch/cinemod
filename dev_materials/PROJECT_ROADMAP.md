# CineMod - Plán vývoja (Project Roadmap)

Tento dokument definuje systematický plán vývoja projektu **CineMod** — C++ klonu programu Cinema 4D so zameraním na identické UI/UX, klávesové skratky a pracovný postup.

---

## 1. Technologický stack a architektúra

* **Jazyk:** C++20 / C++23 (moderné koncepty, `std::span`, modularita, inteligentné smerníky).
* **Build systém:** CMake (verzia 3.24+) + Ninja / MSVC.
* **Správca balíčkov:** `vcpkg` (alebo Git submodules / FetchContent pre externé závislosti).
* **GUI & Docking:** **Dear ImGui** (branch `docking`) s vlastnou Cinema 4D Dark témou, vlastnými widgetmi (čísla s drag scrubbingom, traffic lights, tag ikony) a dockingom.
* **Grafické API:**
  * **Fáza 1–3:** OpenGL 4.5 Core (pre okamžitý a stabilný nábeh viewportu, mriežky, shaderov a gizma).
  * **Dlhodobý cieľ:** Abstraktná vrstva `RenderDevice` s podporou **Vulkan** backendu.
* **Matematická knižnica:** GLM (OpenGL Mathematics) pre vektory, matice, kvaternióny a transformácie.
* **Windowing & Input:** GLFW (alebo SDL2) pre multi-platformové vytváranie okien, kontextu a správu vstupov.

---

## 2. Navrhovaná štruktúra projektu

```text
_cinemod/
├── dev_materials/             # Špecifikácie, architektúra, manuály, referencie
│   ├── PROJECT_ROADMAP.md
│   ├── C4D_ARCHITECTURE_SPEC.md
│   ├── C4D_UI_LAYOUT_GUIDE.md
│   └── C4D_SHORTCUTS_REFERENCE.md
├── src/
│   ├── core/                  # Jadro dátového modelu (BaseDocument, BaseObject, BaseTag, BaseContainer)
│   ├── math/                  # Matematické typy, transformácie (Matrix, Vector3, HPB Euler)
│   ├── scene/                 # Správa scény, hierarchia, kamera, scéna
│   ├── renderer/              # Render pipeline viewportu, shadery, mriežka, gizmo
│   │   ├── opengl/            # OpenGL 4.5 implementácia
│   │   └── rhi/               # Render Hardware Interface (abstrakcia)
│   ├── ui/                    # UI Manažéri a ImGui layout
│   │   ├── managers/          # ObjectManager, AttributeManager, MaterialManager, TimelineManager
│   │   ├── theme/             # C4D farebná schéma, fonty, štýly
│   │   └── widgets/           # C4D špecifické prvky (scrubbery, stromové uzly, bodky viditeľnosti)
│   ├── commands/              # Command systém a Undo/Redo stack
│   └── app/                   # Vstupný bod (main), okno aplikácie, event loop
├── assets/
│   ├── shaders/               # GLSL shadery (grid, wireframe, standard mesh)
│   ├── fonts/                 # UI fonty
│   └── icons/                 # C4D štýl ikon (objekty, tagy, transformácie)
└── CMakeLists.txt             # Hlavný build konfiguračný súbor
```

---

## 3. Vývojové fázy (Milestones)

### Fáza 1: Projektová kostra & Okenné prostredie (Základ)
- [ ] Nastavenie `CMakeLists.txt` a stiahnutie kľúčových závislostí (GLFW, GLM, Glad/GLEW, Dear ImGui).
- [ ] Vytvorenie hlavného okna cez GLFW a inicializácia OpenGL 4.5 kontextu.
- [ ] Integrácia Dear ImGui s povoleným dockingom (`ImGuiConfigFlags_DockingEnable`).
- [ ] Vytvorenie základnej C4D tmavej farebnej palety (charakteristické odtiene sivej, žlté/oranžové akcenty výberu).

### Fáza 2: C4D UI Shell & Rozloženie manažérov
- [ ] Predvolený DockSpace layout kopírujúci Cinema 4D:
  - **Menu Bar** (File, Edit, Create, Select, Tools, Mesh, Animate, Render, Window, Help).
  - **Top Tool Bar** (Undo, Redo, Selection, Move, Rotate, Scale, Render View, Render to Picture Viewer).
  - **Left Mode Bar** (Model Mode, Polygon, Edge, Point Mode, Axis Mode).
  - **Centrálny 3D Viewport** s horným lištovým menu (View, Cameras, Display, Options, Filter).
  - **Object Manager** (vpravo hore).
  - **Attribute Manager** (vpravo dole).
  - **Material Manager & Coordinates Manager** (dole).
  - **Timeline / Transport Bar** (play/pause, keyframing, frame counter).

### Fáza 3: 3D Viewport, Grid & C4D Kamera
- [ ] Render mriežky (3D Ground Grid) a osí (X=červená, Y=zelená, Z=modrá).
- [ ] Kamera a navigačný model Cinema 4D:
  - `1` + ťahanie myšou: Pan (posun kamery).
  - `2` + ťahanie myšou: Dolly / Zoom.
  - `3` + ťahanie myšou: Orbit / Tumble (rotácia okolo bodu záujmu).
  - Alternatíva: `Alt` + stredné / pravé / ľavé tlačidlo myši.
- [ ] Štvordielny pohľad (4-View: Perspektíva, Zhora, Zspredu, Zprava) cez kláves `F5` / stredné tlačidlo myši na viewport.
- [ ] HUD navigačné ikony v pravom hornom rohu viewportu.

### Fáza 4: Dátový model scény (C4D Scene Graph Core)
- [ ] Implementácia architektúry podľa C4D SDK:
  - `BaseList2D` & `GeListNode` (obojstranný zoznam a stromová hierarchia rodič/dieťa).
  - `BaseDocument` (kontajner scény, čas, nastavenia, aktívny výber).
  - `BaseObject` (3D uzol s maticou transformácie, lokálnymi a svetovými súradnicami).
  - `BaseTag` (komponenty pripojené k objektu — napr. Phong tag, Material tag).
- [ ] Parametrické primitívy:
  - Cube (Kocka: Size X/Y/Z, Segments X/Y/Z, Fillet).
  - Sphere (Guľa: Radius, Segments, Type).
  - Cylinder (Valec: Radius, Height, Segments).
  - Plane (Rovina: Width, Height, Segments).
- [ ] Prepojenie s **Object Managerom**:
  - Zobrazenie stromu objektov.
  - C4D "traffic light" bodky viditeľnosti (sivá / zelená / červená pre Editor a Render).
  - Zoznam tagov vedľa názvu objektu.

### Fáza 5: Manipulačné nástroje & 3D Gizmo
- [ ] Implementácia interaktívneho 3D Gizma vo viewporte:
  - **Move Tool (`E`)**: 3 osi (X, Y, Z) + rovinné štvorce (XY, XZ, YZ).
  - **Scale Tool (`T`)**: osi škálovania a uniformné škálovanie.
  - **Rotate Tool (`R`)**: rotačné kružnice.
- [ ] Výber objektov vo viewporte pomocou Raycastingu (kliknutie na objekt).
- [ ] Klávesová skratka `Space` pre prepínanie medzi aktuálnym transformačným nástrojom a Live Selection (`9`).

### Fáza 6: Attribute Manager & Coordinates Manager
- [ ] Dynamický Attribute Manager zobrazujúci vlastnosti označeného objektu:
  - Karta **Basic** (Name, Layer, Visible in Editor, Visible in Render, Enabled).
  - Karta **Coord.** (Position X/Y/Z, Scale X/Y/Z, Rotation H/P/B).
  - Karta **Object** (špecifické parametre vybraného primitívu — segmenty, veľkosť, zaoblenie).
- [ ] Samostatný **Coordinates Manager** v dolnej lište s okamžitým prepisovaním hodnôt a prepínačom World / Object space.
- [ ] Numerické polia s C4D "drag scrubbing" interakciou (ťahanie myšou po šípkach).

### Fáza 7: Command systém, Undo/Redo & Serializácia
- [ ] Návrhový vzor Command (`BaseCommand`, `UndoStep`).
- [ ] Undo (`Ctrl+Z`) a Redo (`Ctrl+Y`) pre posun objektov, pridávanie a zmeny parametrov.
- [ ] Ukladanie a načítanie scény do/z JSON/binárneho formátu scény.

### Fáza 8: Rozšírené funkcie (Modelovanie, Materiály, Animácia)
- [ ] Kláves `C` (Make Editable) — prevod parametrického primitívu na polygonálny mesh (`PolygonObject`).
- [ ] Režimy bodov, hrán a polygónov.
- [ ] Materiálový systém a základný PBR / Phong shader.
- [ ] Timeline a kľúčové snímky (Keyframing transformácií).
