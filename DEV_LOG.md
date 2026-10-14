# CineMod - Denník vývoja & Záznamy projektu (Dev Log)

Tento súbor slúži ako centrálny pracovný denník vývoja. Obsahuje aktuálny stav projektu, zoznam dokončených úloh, to-do zoznam bezprostredných aj budúcich krokov, chronologický záznam zmien a dôležité technické poznámky.

---

## 📊 Prehľad stavu projektu (Status Dashboard)

* **Aktuálna fáza:** Fáza 6 & 7 – Transakčný systém, Quick Commander, Material Editor a serializácia
* **Celkový stav:** Fázy 1 až 6 kompletne funkčné a úspešne skompilované do `CineMod.exe` bez chýb a varovaní!
* **Cieľový stack:** C++20, CMake 3.24+, MinGW GCC 15.2, GLFW 3.4, Glad OpenGL 4.5 Core, Dear ImGui (Docking), ImGuizmo, GLM 1.0.1.
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
- [x] **Fáza 3: 3D Viewport, Grid & C4D Navigačná Kamera**:
  - Renderovacia slučka do OpenGL Framebufferu (FBO).
  - 3D Ground Grid so zvýraznenými stredovými osami X (červená) a Z (modrá).
  - 3D Orbit/Pan/Dolly kamera okolo bodu záujmu (Pivot) ovládaná klávesmi `1`, `2`, `3` a myšou (`Alt` + tlačidlá, koliesko, `S` pre Frame Selected, `H` pre Frame All).
- [x] **Fáza 4 & 5: Plnohodnotná 3D interakcia (ImGuizmo, Raycast Picking, viac primitívov)**:
  - Integrácia `ImGuizmo` (Move `E`, Rotate `R`, Scale `T`, World/Object `W`).
  - 3D Raycast Picking kliknutím do viewportu.
  - Parametrické telesá: Cube, Sphere, Plane, Light, Camera, Null.
  - Duplikácia objektu cez `Ctrl+D` alebo kontextové menu.
- [x] **Fáza 6: Transakčný Undo / Redo systém & Cinema 4D Quick Commander**:
  - Architektúra príkazov `ICommand` a `CommandManager`:
    - `TransformCommand`: zachytenie počiatočného a koncového stavu pri ťahaní gizma.
    - `AddObjectCommand`: pridávanie telies s možnosťou vrátenia.
    - `DuplicateObjectCommand`: klonovanie objektov.
    - `DeleteObjectCommand`: nedeštruktívne odpojenie a opätovné vloženie objektu.
  - Prepojenie v `Edit -> Undo / Redo` (dynamické názvy akcií), na hornej lište aj klávesových skratkách `Ctrl+Z` a `Ctrl+Y` / `Ctrl+Shift+Z`.
  - **Cinema 4D Quick Commander (`Shift+C`)**:
    - Plávajúce vyhľadávacie okno s okamžitou filtráciou príkazov a objektov.
    - Navigácia šípkami, spustenie cez `Enter`, zatvorenie cez `Esc`.
  - **Material Editor**:
    - Dvojklik na materiál otvorí editor s výberom farby, presetmi (Plastic, Metal, Glass, Gold, Ruby), roughness a reflectance.

---

## 🔄 Čo sa aktuálne rieši (In Progress)

- [ ] **Fáza 7: Serializácia scény a ukladanie projektov:**
  - Ukladanie a načítanie projektu (`Ctrl+S`, `Ctrl+O`) do formátu `.cmod` (JSON).

---

## 📋 Čo treba urobiť (To-Do List)

### Následné kroky (Serializácia & Pokročilé modelovanie):
- [ ] Implementovať export a import scény (pozície, HPB rotácie, škálovanie, materiály, strom hierarchie).
- [ ] Implementovať `Make Editable` (kláves `C` v C4D) na konverziu parametrických telies na polygónový mesh (EditMode: Points, Edges, Polygons).

---

## 📝 Chronologický denník vývoja (Changelog & Diary)

### [14. 10. 2026] – Implementácia Fázy 6: Undo/Redo systém, Quick Commander (Shift+C) a Material Editor
* **Aktivita:** Implementácia transakčného manažéra príkazov, interaktívneho vyhľadávača Commander a plnohodnotného editora materiálov.
* **Vykonané zmeny:**
  * Vytvorená infraštruktúra `CommandManager` a rozhranie `ICommand`.
  * Implementované príkazy:
    * `TransformCommand` (Move, Rotate, Scale drag delta).
    * `AddObjectCommand` (pridávanie objektov).
    * `DuplicateObjectCommand` (klonovanie objektov).
    * `DeleteObjectCommand` (nedeštruktívne mazanie s `DetachObject`/`InsertObject`).
  * Prepojený Undo/Redo na:
    * Klávesové skratky `Ctrl+Z`, `Ctrl+Y`, `Ctrl+Shift+Z`.
    * Menu `Edit -> Undo [Názov]` a `Edit -> Redo [Názov]`.
    * Hornú nástrojovú lištu s dynamickými tooltipmi.
    * Kláves `Delete` / `Backspace`.
  * Vytvorený **Cinema 4D Quick Commander (`Shift+C`)**:
    * Centrálne HUD dialógové okno s live fulltext vyhľadávaním.
    * Kategórie: Create, Tools, Edit, View, Material, Animation.
    * Ovládanie šípkami hore/dole a klávesom Enter.
  * Dokončený **Material Editor**:
    * Interaktívna editácia farby, roughness a reflectance.
    * Paleta rýchlych presetov (Plastic, Metal, Glass, Gold, Ruby) a tlačidlo priradenia na označený objekt.
  * Build úspešne dokončený do `build/CineMod.exe` (0 chýb, 0 varovaní).

### [14. 10. 2026] – Implementácia Fázy 4 & 5: ImGuizmo, Raycast Picking a viac primitívov
* **Aktivita:** Integrácia 3D manipulačného gizma, výberu objektov lúčom kamery a procedurálnych telies.
* **Vykonané zmeny:**
  * Pridaná a skompilovaná knižnica `ImGuizmo` priamo do `imgui_lib`.
  * Vyriešený lifecycle `ImGuizmo`:
    * Doplnené `ImGuizmo::SetImGuiContext()` a `ImGuizmo::BeginFrame()` do hlavnej renderovacej slučky.
    * Upravené poradie spracovania: `RenderGizmo` sa vyhodnocuje pred vstupmi, čím sa predchádza nechcenému odznačeniu objektu pri kliknutí na šípky gizma.
    * Pridané `ImGuizmo::PushID` / `PopID` pre stabilnú správu inštancií.
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
