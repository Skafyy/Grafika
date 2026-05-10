# Biliárd Szimuláció - Grafika Programozás Projekt

Egy interaktív, 3D biliárd szimuláció, amely C nyelven és OpenGL technológiával készült. A projekt célja a grafikai alapelvek (világítás, textúrázás, 3D modellezés) és az alapvető fizikai interakciók bemutatása.

##Assets letöltése

A projekt futtatásához szükséges 3D modellek és textúrák méretük miatt külső tárhelyen találhatóak. A program **nem fog megfelelően elindulni** ezek nélkül.

**[Assets Letöltése (Bitly link)](https://bit.ly/4wltyvf)**

### Telepítés:
1. Töltsd le az `assets.zip` fájlt a fenti linkről.
2. Csomagold ki a projekt gyökérkönyvtárába.
3. Ellenőrizd, hogy létezik-e az `assets/models` és `assets/textures` elérési út.

Technikai jellemzők

### Megjelenítés és Grafika
* **Dákótartó (Cue Rack):** Egyedi 3D modell a falon, amely procedurális textúra-leképezést (auto-mapping) használ a valósághű fa erezet megjelenítéséhez.
* **Dinamikus dákók:** 5 darab különböző színű dákó a tartóban, egyedi anyagjellemzőkkel.
* **Világítás:** Phong-modell alapú világítás. A dákótartó és az asztal lábai lakkozott hatást keltő *specular* (tükröződő) csillogással rendelkeznek.
* **Textúrázás:** Többszörös textúra-kezelés (posztó, fa, padló, falak).

### Játékmenet
* **Fizika:** Golyó-fal és golyó-golyó ütközések kezelése.
* **Erőmérő (HUD):** Interaktív erőmérő csík a pontos lökések kivitelezéséhez.
* **Kamera:** Szabad nézelődés (Yaw/Pitch) és mozgás a biliárdteremben.

---

## Irányítás

| Gomb | Funkció |
| :--- | :--- |
| **W, A, S, D** | Mozgás a teremben |
| **Egér** | Kamera forgatása (Nézelődés) |
| **Space (Hold)** | Ütés erejének feltöltése |
| **Space (Release)** | Lövés a fehér golyóval |

---

## Fordítás és Futtatás

A projekt lefordításához GCC fordító és a szükséges OpenGL könyvtárak (GLFW3, GLU) megléte szükséges.

1. Nyiss egy terminált a projekt mappájában.
2. Futtasd a `make` parancsot:
   ```bash
   make
