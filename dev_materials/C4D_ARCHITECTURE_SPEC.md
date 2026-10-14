# Cinema 4D Architektonická špecifikácia (SDK Core Design)

Tento dokument slúži ako technický referenčný manuál pre vnútornú štruktúru tried, dátových modelov a objektového systému inšpirovaného oficiálnou architektúrou Maxon Cinema 4D C++ SDK.

---

## 1. Dedičná hierarchia jadra

V Cinema 4D je takmer každý prvok scény odvodený zo stromu `C4DAtom` -> `GeListNode` -> `BaseList2D`:

```text
C4DAtom
  │  (Základná pamäťová správa, typové identifikátory, systém posielania správ / Message)
  │
  └── GeListNode
        │  (Obojstranne viazaný zoznam a stromová štruktúra - rodič, deti, predchodca, nasledovník)
        │
        └── BaseList2D
              │  (Podpora pre parametre, dynamické kontajnery, animačné stopy a názov)
              │
              ├── BaseDocument    (Koreňový kontajner celej scény/projektu)
              ├── BaseObject      (Priestorový uzol: geometrie, svetlá, kamery, null objekty)
              ├── BaseTag         (Modifikátory priradené k BaseObjectu: Phong, Material, Transform)
              ├── BaseMaterial    (Materiály a shadery)
              └── BaseShader      (Procedurálne shadery: Noise, Gradient...)
```

---

## 2. Popis kľúčových tried

### `GeListNode` (Hierarchia a navigácia)
Poskytuje ukazovatele a operácie pre obojsmerný strom scény:
* `GetUp()`: Vráti rodiča (`parent`).
* `GetDown()`: Vráti prvé dieťa (`firstChild`).
* `GetPred()`: Vráti predchádzajúceho súrodenca (`previousSibling`).
* `GetSucc()`: Vráti nasledujúceho súrodenca (`nextSibling`).
* `InsertUnder(GeListNode* parent)`: Vloží uzol ako prvé dieťa daného rodiča.
* `InsertUnderLast(GeListNode* parent)`: Vloží uzol ako posledné dieťa.
* `Remove()`: Odpojí uzol z hierarchie.

### `BaseDocument` (Kontajner scény)
Predstavuje otvorený projekt. Zodpovedá za:
* Správu zoznamu koreňových objektov: `GetFirstObject()`, `InsertObject(BaseObject* op)`.
* Správu materiálov: `GetFirstMaterial()`, `InsertMaterial(BaseMaterial* mat)`.
* Správu globálneho času a snímkovej frekvencie: `GetTime()`, `SetTime(BaseTime time)`, `GetFps()`.
* Aktívny výber: `GetActiveObject()`, `GetActiveObjects()`, `SetActiveObject(BaseObject* op)`.
* Undo/Redo operácie: `StartUndo()`, `AddUndo(...)`, `EndUndo()`.

### `BaseObject` (3D Priestorový uzol)
Každý vizuálny alebo logický 3D prvok v scéne:
* **Transformácie:**
  * Lokálna matica (`GetMl()`, `SetMl(Matrix m)`)
  * Globálna (svetová) matica (`GetMg()`, `SetMg(Matrix m)`)
  * Relatívne pozície: `GetRelPos()`, `GetRelRot()`, `GetRelScale()`
* **Zoznam tagov (komponentov):**
  * `GetFirstTag()`: Začiatok zoznamu tagov priradených k tomuto objektu.
  * `InsertTag(BaseTag* tag)`: Priradenie tagu.
* **Viditeľnosť (Traffic Lights):**
  * `GetEditorMode()` / `SetEditorMode()`: (MODE_UNDEF / MODE_ON / MODE_OFF)
  * `GetRenderMode()` / `SetRenderMode()`: (MODE_UNDEF / MODE_ON / MODE_OFF)
* **Virtuálna generácia (Generators & Deformers):**
  * Metóda `GetVirtualObjects()` pre parametrické objekty (kocka, guľa) na vygenerovanie polygonálnej cache siete (`PolygonObject`).

### `BaseTag` (Modifikátory objektu)
Tagy neexistujú samostatne; sú vždy pripojené k `BaseObject`:
* **Phong Tag:** Vyhladzovanie normál povrchu s nastaveným uhlom (Phong Angle, Use Angle Limit).
* **Texture / Material Tag:** Prepojenie objektu s `BaseMaterial` a definícia projekcie (UVW, Spherical, Cubic, Flat).
* **Protection Tag:** Uzamknutie transformácie proti neúmyselnému posunu.

---

## 3. Matematický systém: Rotačný systém HPB (Heading, Pitch, Bank)

Cinema 4D používa na reprezentáciu Eulerových uhlov špecifický systém **H, P, B**:
* **H (Heading / Azimut / Y-osa):** Rotácia okolo vertikálnej osi Y.
* **P (Pitch / Náklon / X-osa):** Rotácia okolo priečnej osi X.
* **B (Bank / Klopenie / Z-osa):** Rotácia okolo pozdĺžnej osi Z.

Matica rotácie $R$ je skladaná v poradí:
$$R = R_H \times R_P \times R_B$$

Vektor súradníc v C4D používa pravotočivý súradnicový systém:
* **+X** smeruje doprava (červená farba).
* **+Y** smeruje nahor (zelená farba).
* **+Z** smeruje do hĺbky / dopredu (modrá farba).

---

## 4. Dátový model parametrov: BaseContainer & Description

V C4D nie sú parametre iba pevnými členskými premennými C++ triedy. Namiesto toho používajú dynamický kontajner s ID indexovaním:
* `BaseContainer`: Kľúč-hodnota úložisko (`Int32 ID -> GeData`), kde hodnota môže byť `Int32`, `Float`, `String`, `Vector`, `Matrix`, pointer na iný uzol, atď.
* Príklad:
  * ID pre Cube Size X: `PRIM_CUBE_LEN` (Vector)
  * ID pre Cube Segments: `PRIM_CUBE_SUBX`, `PRIM_CUBE_SUBY`, `PRIM_CUBE_SUBZ`
  * ID pre Fillet: `PRIM_CUBE_DOFILLET` (Bool), `PRIM_CUBE_FRAD` (Float)
* **Výhoda tohto prístupu pre náš klon:**
  1. Univerzálny **Attribute Manager** – nemusíme písať vlastný UI kód pre každý nový objekt; inšpektor automaticky prechádza zoznam parametrov a zobrazuje príslušné widgety.
  2. Jednoduchá serializácia a ukladanie do súboru.
  3. Automatizovaný systém Undo/Redo.
  4. Jednoduchá animácia a kľúčovanie ľubovoľného parametra.

---

## 5. Správa príkazov (Command System) a Undo/Redo

Každá akcia používateľa v C4D je zapuzdrená v príkaze:
* Akcia vytvorí Undo krok v dokumente:
  ```cpp
  doc->StartUndo();
  doc->AddUndo(UNDOTYPE_CHANGE, op);
  op->SetRelPos(newPos);
  doc->EndUndo();
  ```
* Podporované typy Undo:
  * `UNDOTYPE_CHANGE`: Zmena stavu existujúceho objektu.
  * `UNDOTYPE_NEW`: Pridanie nového objektu (Undo ho odstráni).
  * `UNDOTYPE_DELETE`: Zmazanie objektu (Undo ho vráti na pôvodné miesto v strome).
  * `UNDOTYPE_HIERARCHY`: Zmena poradia alebo rodiča v strome.
