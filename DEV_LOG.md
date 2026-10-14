# CineMod - Denník vývoja & Záznamy projektu (Dev Log)

Tento súbor slúži ako centrálny pracovný denník vývoja. Obsahuje aktuálny stav projektu, zoznam dokončených úloh, to-do zoznam bezprostredných aj budúcich krokov, chronologický záznam zmien a dôležité technické poznámky.

---

## 📊 Prehľad stavu projektu (Status Dashboard)

* **Aktuálna fáza:** Fáza 3 & 4 – Rozširovanie interakcie viewportu a dátového modelu
* **Celkový stav:** Fázy 1 a 2 kompletne funkčné a úspešne skompilované do `CineMod.exe`! Okno, OpenGL 4.5, C4D Docking layout, 3D kamera, ground grid a manažéri bežia.
* **Cieľový stack:** C++20, CMake 3.24+, MinGW GCC 15.2, GLFW 3.4, Glad OpenGL 4.5 Core, Dear ImGui (Docking), GLM 1.0.1.
* **Posledná aktualizácia:** 14. 10. 2026

---

## ✅ Čo je už hotové (Done)

- [x] **Definícia vízie a požiadaviek projektu** ([agents.md](file:///c:/0_dev/_cinemod/agents.md)):
  - Ciel: 1:1 workflow klon Maxon Cinema 4D v C++.
  - Identické UI, layout, klávesové skratky, správanie nástrojov.
- [x] **Prieskum a technická dokumentácia** v priečinku [`dev_materials/`](file:///c:/0_dev/_cinemod/dev_materials):
  - [PROJECT_ROADMAP.md](file:///c:/0_dev/_cinemod/dev_materials/PROJECT_ROADMAP.md) — 8-fázový plán.
  - [C4D_ARCHITECTURE_SPEC.md](file:///c:/0_dev/_cinemod/dev_materials/C4D_ARCHITECTURE_SPEC.md) — C++ SDK hierarchia, HPB rotácia, `BaseContainer`.
  - [C4D_UI_LAYOUT_GUIDE.md](file:///c:/0_dev/_cinemod/dev_materials/C4D_UI_LAYOUT_GUIDE.md) — UI geometria, traffic lights, hex téma.
  - [C4D_SHORTCUTS_REFERENCE.md](file:///c:/0_dev/_cinemod/dev_materials/C4D_SHORTCUTS_REFERENCE.md) — Matica skratiek.
- [x] **Zriadenie denníka vývoja** ([DEV_LOG.md](file:///c:/0_dev/_cinemod/DEV_LOG.md)).
- [x] **Fáza 1: Build systém a infraštruktúra (C++20, CMake, GLAD, GLFW, ImGui, GLM)**:
  - Vygenerovaný GLAD loader pre OpenGL 4.5 Core.
  - Nastavený `CMakeLists.txt` s automatickým FetchContent pre GLFW 3.4, GLM 1.0.1 a Dear ImGui (`docking`).
  - Vstupný bod `main.cpp` a jadro `Application`.
- [x] **Fáza 2: C4D UI Shell, Téma & Docking Layout**:
  - `C4DTheme` s presnou farebnou schémou Cinema 4D Dark Theme (pozadie `#2B2B2B`, oranžový akcent `#EA940D`, zaoblenia, paddingy).
  - Plnohodnotný C4D DockSpace layout:
    - Hlavný Menu Bar (File, Edit, Create, Select, Tools, Render, Window, Help).
    - Horná lišta nástrojov (Undo, Redo, Select 9, Move E, Rotate R, Scale T, Cube, Sphere, Render).
    - Bočná lišta režimov (Model, Points, Edges, Polygons).
    - Object Manager (strom objektov, semaforové bodky viditeľnosti Editor/Render, tagy).
    - Attribute Manager (záložky Basic, Coord, Object s číselnými editormi a scrubbmi).
    - Coordinates Manager (súradnice P, S, R s HPB rotáciou a prepínačom World/Object).
    - Material Manager (paleta materiálov).
    - Timeline (ovládanie prehrávania, playhead, frame counter, keyframe tlačidlo `F9`).
    - 3D Viewport s HUD ikonami pre Pan `1`, Zoom `2`, Orbit `3` a 4-Split `#`.
- [x] **Fáza 3 (Základ): 3D Viewport, Grid & C4D Navigačná Kamera**:
  - Renderovacia slučka do OpenGL Framebufferu (FBO).
  - 3D Ground Grid so zvýraznenými stredovými osami X (červená) a Z (modrá).
  - 3D Orbit/Pan/Dolly kamera okolo bodu záujmu (Pivot) ovládaná klávesmi `1`, `2`, `3` a myšou (`Alt` + tlačidlá, koliesko, `S` pre Frame Selected, `H` pre Frame All).
- [x] **Fáza 4 & 5: Plnohodnotná 3D interakcia (ImGuizmo, Raycast Picking, viac primitívov)**:
  - **Integrácia `ImGuizmo`**:
    - Skutočný manipulačný 3D gizmo s plynulým ťahaním myšou vo viewporte.
    - Nástroje: **Move (`E`)** s osami a rovinami, **Rotate (`R`)** s kružnicami, **Scale (`T`)**.
    - Prepínanie **World / Object** priestoru cez kláves **`W`**.
    - Obojsmerné prepojenie v reálnom čase s Attribute a Coordinates Managerom.
  - **3D Raycast Picking**:
    - Výber objektu kliknutím myšou priamo vo viewporte cez lúč kamery a AABB test.
    - Kliknutie do prázdneho priestoru zruší výber.
  - **Parametrické telesá**:
    - Generovanie sietí s normálami: **Cube (Kocka)**, **Sphere (Guľa)**, **Plane (Rovina)**.
  - **Klonovanie / Duplikácia**:
    - Okamžitá duplikácia objektu cez klávesovú skratku **`Ctrl + D`** alebo tlačidlo v Object Manageri.

---

## 🔄 Čo sa aktuálne rieši (In Progress)

- [ ] **Fáza 6 & 7: Command systém, Undo/Redo a serializácia scény:**
  - Plnohodnotný transakčný Undo/Redo stack (`Ctrl+Z`, `Ctrl+Y`) pre posun objektov, zmeny hodnôt a pridávanie/mazanie.
  - Ukladanie a načítanie projektu (`Ctrl+S`, `Ctrl+O`).

---

## 📋 Čo treba urobiť (To-Do List)

### Najbližšie kroky (Fáza 1 & 2):
- [ ] Vytvoriť `CMakeLists.txt` podporujúci C++20 a automatické sťahovanie závislostí (GLFW, Glad, Dear ImGui, GLM).
- [ ] Vytvoriť vstupný bod aplikácie: `src/main.cpp` a triedu `Application`.
- [ ] Implementovať C4D Dark Theme pre ImGui (nastavenie farieb, zaoblení rohov, paddingu podľa `C4D_UI_LAYOUT_GUIDE.md`).
- [ ] Implementovať `DockSpace` layout kopírujúci Cinema 4D:
  - Menu Bar (File, Edit, Create...)
  - Top Toolbar (Transformácie, Režimy)
  - 3D Viewport okno
  - Object Manager (panel vpravo hore)
  - Attribute Manager (panel vpravo dole)
  - Timeline & Transport Bar (panel dole)
  - Coordinates Manager & Material Manager (spodné panely)

### Následné kroky (Fáza 3 – Viewport & 3D Kamera):
- [ ] Implementovať renderovaciu triedu pre 3D mriežku (Ground Grid) s osami X (červená) a Z (modrá).
- [ ] Implementovať 3D kameru s presným C4D správaním:
  - Skratky `1` (Pan), `2` (Zoom/Dolly), `3` (Orbit).
  - Skratka `F5` / stredný klik pre prepnutie na 4 pohľady.
  - HUD ikony v pravom hornom rohu viewportu.

### Dátový model & Scéna (Fáza 4):
- [ ] Vytvoriť základné triedy `BaseDocument`, `BaseObject`, `BaseTag`.
- [ ] Vytvoriť prvé parametrické telesá: Kocka (`CubeObject`) a Guľa (`SphereObject`).
- [ ] Prepojiť strom scény s Object Managerom vrátane semaforových bodiek viditeľnosti (traffic lights).

---

## 📝 Chronologický denník vývoja (Changelog & Diary)

### [14. 10. 2026] – Implementácia Fázy 4 & 5: ImGuizmo, Raycast Picking a viac primitívov
* **Aktivita:** Integrácia 3D manipulačného gizma, výberu objektov lúčom kamery a procedurálnych telies.
* **Vykonané zmeny:**
  * Pridaná a skompilovaná knižnica `ImGuizmo` priamo do `imgui_lib`.
  * Prepojený `ImGuizmo` s `ViewportManagerom`:
    * Posun (`E`), Rotácia (`R`), Škálovanie (`T`).
    * Prepínanie svetového a lokálneho priestoru (`W`).
    * Okamžitá dekompozícia do pozície, HPB rotácie a škálovania spätne do označeného `BaseObject`.
  * Implementovaný 3D Raycasting výber objektov (`Scene::PickObject`, `Camera::GetRay` a `Ray::IntersectsAABB`).
  * Pridané generovanie sietí pre **Sphere (Guľa)** a **Plane (Rovina)** v `ViewportRenderer`.
  * Pridaná podpora duplikácie objektov (`Scene::DuplicateObject`, skratka `Ctrl+D`).
  * Aktualizované panely `ObjectManager` a `AttributeManager` pre nové typy telies.
  * Projekt skompilovaný s 0 chybami a 0 varovaniami do `build/CineMod.exe`.

### [14. 10. 2026] – Implementácia Fázy 1 & 2 a úspešný build CineMod.exe
* **Aktivita:** Vytvorenie C++20 aplikačnej kostry, OpenGL 4.5 viewportu a C4D UI docking manažérov.
* **Vykonané zmeny:**
  * Vygenerované knižničné jadro GLAD pre OpenGL 4.5 Core (`external/glad/`).
  * Nastavený `CMakeLists.txt` (automatický fetch GLFW 3.4, GLM 1.0.1, Dear ImGui `docking`).
  * Vytvorená trieda `C4DTheme` s identickou C4D farebnou paletou (tmavé rámy `#1E1E1E`, okná `#2B2B2B`, oranžový akcent `#EA940D`).
  * Vytvorený 3D renderovací pipeline `ViewportRenderer` s OpenGL FBO, 3D Ground Gridom (červená X, modrá Z os), tieňovaním meshu a 3D gizmom.
  * Implementovaná navigačná kamera inšpirovaná C4D (`1` Pan, `2` Zoom, `3` Orbit, `S` Frame Selected, `H` Frame All, Alt+myš, koliesko).
  * Vytvorené všetky kľúčové manažérske panely:
    * `ObjectManager` (strom uzlov, traffic lights viditeľnosť, tagy).
    * `AttributeManager` (Basic, Coord, Object záložky, drag numeric scrubbing).
    * `CoordinatesManager` (P, S, R s HPB rotáciou a prepínačom World/Object).
    * `MaterialManager` (swatches paleta).
    * `TimelineManager` (prehrávanie, transportné tlačidlá, scrubber, kláves `F8`).
    * `ViewportManager` (HUD navigačné ikony v rohu).
  * Projekt úspešne skompilovaný s GCC 15.2 bez chýb do `build/CineMod.exe`.

### [14. 10. 2026] – Založenie projektu a výskumná fáza
* **Aktivita:** Inicializácia projektu CineMod.
* **Vykonané zmeny:**
  * Analyzované požiadavky zadané v `agents.md`.
  * Vyhľadaná a spracovaná oficiálna špecifikácia architektúry Maxon Cinema 4D (C++ SDK hierarchia, triedy `BaseDocument`, `BaseObject`, `BaseTag`, `BaseContainer`, rotačné pravidlo HPB).
  * Vytvorená referenčná dokumentácia v priečinku `dev_materials/`:
    * `PROJECT_ROADMAP.md` – 8-fázový míľnikový plán.
    * `C4D_ARCHITECTURE_SPEC.md` – technická špecifikácia tried a vzorov.
    * `C4D_UI_LAYOUT_GUIDE.md` – vizuálny a funkčný manuál okien a farieb.
    * `C4D_SHORTCUTS_REFERENCE.md` – zoznam všetkých štandardných klávesových skratiek.
  * Založený tento súbor `DEV_LOG.md` pre sledovanie postupu.

---

## 💡 Dôležité technické a architektonické poznámky

1. **Dear ImGui Docking Branch:**
   * Pre identické rozloženie okien ako v Cinema 4D musíme použiť ImGui z vetvy `docking`.
   * Vlajky: `ImGuiConfigFlags_DockingEnable` a `ImGuiConfigFlags_ViewportsEnable` (voliteľné pre odtrhnutie okien na sekundárny monitor).
2. **Kamerový model:**
   * V Cinema 4D kamera rotuje okolo bodu záujmu (Pivot / Target distance). Orbitovanie musí brať do úvahy stred záujmu, inak nebude pôsobiť prirodzene ako v C4D.
3. **Eulerov systém HPB:**
   * Pri rotáciách nesmieme používať klasické XYZ Eulerove poradie! C4D striktne používa **Heading** (Y), **Pitch** (X), **Bank** (Z).
4. **Pamäťový model objektov:**
   * Zvoliť moderný systém vlastníctva: `std::unique_ptr` / `std::shared_ptr` v kombinácii so surovými ukazovateľmi pre stromovú navigáciu (`GetUp()`, `GetDown()`, `GetPred()`, `GetSucc()`).
