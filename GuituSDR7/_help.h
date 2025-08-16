/*
Skupna dolžina paketov iz g90 je 372 bajtov. Struktura izgleda takole:
Bajti [0-19] - podatki o stanju;
Bajti [20-46] - prikaz besedila CW;
Bajti [47-367] - prikaz spektra;
Bajti [368-371] - kontrolna vsota.
 Ista metodologija kontrolne vsote kot glava->osnova
 (CRC32-Mpeg2, vsake 4 bajte podatkov se prevrnejo, preden se izračuna kontrolna vsota).

Podatki o stanju
===========
Bajt 0 - nivo signala v dBm = 256-bajt[0] = decimalna vrednost z znakom minus
 (na primer vrednost 0xB7 (0xFF-0xB7=0x49=73 Rezultat -73 dBm).
 S-meter se izračuna in prikaže na podlagi te vrednosti.
Bajt 1 - vrednost ravni moči oddajanja v desetinkah vata
 (na primer vrednost 0x80 = 128 = 12,8 W).
 S-meter za prenos se izračuna in prikaže na podlagi te vrednosti
Bajt 2 - vrednost SWR med prenosom. Razpon 0–44.
Bajt 3 - vrednost napajalne napetosti v desetinkah voltov (na primer vrednost 0x80 = 128 = 12,8 V)
Bajt 4 - ??? Bit 6 (32) krmili indikator CW v načinih CW
Bajt 5 je odstotek ALC. Vrednosti od 0 do 255 so preslikane v vrednosti od 0 do 100 za prikaz ALC
Bajti 6..7 - različica vdelane programske opreme g90 ([0x79,0x01] - v1.79)
Bajti 8-10 - neznano
Bajt 11 - ??? Prikaže se bit 7 (64), ki označuje prenos podatkov
Bajti 12-19 - neznano

CW
==============
Bajti 20-46 - Uporabljajo se za prikaz dekodiranega CW besedila.
Večinoma delujejo znaki Unicode od 0 do 255.
Pri nekaterih kontrolnih likih je nekaj nenavadnosti.
Besedilo je prikazano na sredini v območju besedila, ne da bi odstranili prejšnje besedilo.

FFT
==============
Bajti 47-367 - uporabljajo se za prikaz spektra. Skupaj je 160 vrednosti uInt16.
Standardni G90 prikazuje le obseg od 0 do 32.

Kontrolna vsota
==============
Bajti 368-371 so kontrolna vsota sporočila.
Head bo prezrl pakete z neveljavno kontrolno vsoto.
Kontrolna vsota je CRC32-Mpeg2, v kateri se bajti sporočila obrnejo vsake 4 bajte
(npr. [1,2,3,4,5,6,7,8] postane [4,3,2,1,8,7,6,5]), preden se izračuna kontrolna vsota.
*/
