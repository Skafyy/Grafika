🎱 Billiard Pro FPS

Leírás

A projekt egy 3D biliárd szimuláció, amely C nyelven készült OpenGL és GLFW használatával.
A játék egy első személyű (FPS) nézetből irányítható, ahol a játékos szabadon mozoghat a szobában, célozhat, és meglökheti a fehér golyót.

A program tartalmaz alapvető fizikai szimulációt, beleértve:
-golyók mozgását
-ütközéseket
-súrlódást
-lyukakba esést

🚀 Funkciók
-FPS kamera (egér + WASD mozgás)
-Biliárd golyók fizikai szimulációja
-Golyó–golyó ütközés
-Falról visszapattanás
-Lyukakba esés kezelése
-Lövés erősségének szabályozása (Space)
-HUD erőmérő (töltés közben)
-Állítható fényerő
-Automatikus újraindítás, ha minden golyó eltűnt

🎮 Irányítás
-Billentyű	Funkció
-W A S D	Mozgás
-Egér	Nézés / célzás
-SPACE	Lövés (nyomva tartva erő gyűjtése)
+ / -	Fényerő állítása
-F1	Súgó megjelenítése
-ESC	Kilépés

🧱 Technológiák
-C
-OpenGL
-GLU
-GLFW
-stb_image (textúra betöltés)

📂 Projekt struktúra
.
├── src/
│   ├── main.c        # Renderelés és fő ciklus
│   ├── physics.c     # Fizikai számítások és input kezelés
├── include/
│   └── billiard.h    # Adatszerkezetek és konstansok
├── assets/
│   ├── wall.jpg
│   ├── carpet.jpg
│   ├── cloth.jpg
├── Makefile
└── README.md

⚙️ Fordítás

🪟 Windows (MSYS2 - ajánlott)
Nyisd meg a MSYS2 MinGW64 terminált, majd:
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-glfw
mingw32-make

🐧 Linux
make

Szükséges csomagok (pl. Ubuntu):

sudo apt install build-essential libglfw3-dev libglu1-mesa-dev
▶️ Futtatás
./BilliardProjekt

⚠️ Megjegyzések
A program OpenGL fixed pipeline-t használ (glBegin, stb.)
A fizika egyszerűsített, nem teljesen realisztikus

👤 Szerző
Varga István Hunor - Skafyy
