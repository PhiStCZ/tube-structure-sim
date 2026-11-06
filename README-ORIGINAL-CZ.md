### Štrobl Filip
(předmět: Programování v c++)


# Zápočtový program - fyzická simulace 2d struktur z trubek

## (IMPORTANT) - Návod na spuštění
Projekt je závislý na SFML, pro spuštění je tedy třeba si ji stáhnout nebo použít GitHub release obsahující všechny potřebné zkompilované soubory.

Pro spuštění přednastavené simulace spusťte .exe soubor a zadejte mu jméno souboru přednastavené simulace (buď jako argument, nebo do std. vstupu po spuštění). Současně jsou 3 testovací soubory (t1.txt, t2.txt, t3.txt) a 3 ukázkové soubory(e1.txt, e2.txt, e3.txt). (info o vytvoření vlastního souboru simulace níže)


## Hrubý plán ("úrovně" simulace)
1. [x] Zobrazování struktury v daném stavu (co nejobecněji)
2. [x] Fyzická propagace sil v rámci jedné souvislé struktury 
3. [x] Kolize mezi jednotlivými strukturami (nehledě na to, zda jsou propojeny)


## Todo
- Úroveň 0:
  - [x] Reprezentace bodů a trubek (vhodná datová struktura)
  - [x] Inicializace, načítání dat ze souboru
  - [x] Vykreslování z dané instance dat
- Úroveň 1:
  - [x] Udělat základní smyčku zpracování (chceme opakovatelnost tahů, takže frametime bude vždy konstantní)
  - [x] Propagace sil v konstrukci
  - [x] Omezená nosnost konstrukcí, rozbíjení
  - [x] Různé typy materiálů (rozdíl v pevnosti, pružnosti, ...)
- Úroveň 2:
  - [x] Implementovat meziobjektovou kolizi
  - [x] Tření při kolizi
  - [x] Optimalizace

## Problémy a možná řešení

### Vibrace (vyřešeno)
Simulace je diskrétní, takže při extrémních hodnotách (krátké trubky a velká rychlost a síla) může dojít k nestabilní konstrukci se sklony k vibracím. Problém byl ošetřen omezením maximální síly trubek a přidáním odporu proti změně deformace trubek), takže nestabilita již není tolik extrémní, ale zatím se stále vyskytuje.

Tyto extrémní situace se dají zmenšit větší 'hustotou' updatů (menší gravitace a síla konstrukcí + víc framů za sekundu). Tím se zvýší výpočetní náročnost, nicméně program je natolik rychlý, že to zatím není překážkou.

### Propadávání (vyřešeno)
Při meziobjektové kolizi někdy docházelo k propadávání skrz trubky. To bylo vyřešeno zvětšením tloušťky trubek a menší změnou výpočtu odrazu (např. přidáním moveBuffer).

### Performance (vyřešeno)
Ve větších projektech docházelo k nedostatku výpočtové síly, pravděpodobně kvadratickým časem výpočtu kolizí. Implementací 'sweep & prune' algoritmu se podařilo výkon výrazně zvýšit.

### Tření (vyřešeno)
Problémy s třením byly způsobené nepřímo, nestabilitou trubek, které jsou příliš krátké.

## Info

### SFML
Program používá grafickou knihovnu SFML pro zobrazování stavu simulace, nicméně je navržen tak, aby samostatná simulace byla od SFML oddělená (takže v případě odstranění nebo změny grafického rozhraní nebude třeba zasahovat do kódu samotné simulace).

### Formát vstupu pro načítání
Na každé řádce může být:
- `x;y` - značí pevný bod na souřadnicích *x;y* (kterým nelze hnout); pokud na místě již bod je, jeho status se nastaví na pevný
- `x1;y1|x2;y2` - značí trubku mezi body na souřadnicích *x1;y1* a *x2;y2*; pokud body neexistují, vytvoří se nové; pokud již nějaký v místě existuje (i pevný), použije se jako koncový
- `mMx1;y1|x2;y2` - stejně jako předchozí, ale tentokrát separátor `M` slouží k určení materiálu *m* podle čísla materiálu:
  - 0: defaultní materiál (bílý)
  - 1: silný materiál (oranžový)
  - 2: klouzavý materiál (modrý)
  - 3: pružný materiál (zelený)

Prázdné řádky nebo řádky začínající na `#` se při zpravování se ignorují.
Souřadnice používají datový typ `float`.

### Důležité třídy
Pro práci se 2d souřadnicemi byla vytvořena třída `vector2f`, jejíž varianta sice existuje v SFML, ale v zájmu nezávislosti na něm byla radši vytvořena samostatně.

Simulace používá třídy `node` a `tube`, kde `node` obsahují hlavní vlastnosti struktury (umístění, pohyb, ...) a `tube` popisuje vlastnosti propojení 2 instancí `node` (původní délka, materiál, ...).

Třída `material` slouží k bližšímu popisu vlastností `tube` (odrazivost, pevnost, pružnost, ...). Pro ušetření místa `tube` obsahují na materiál pouze odkaz.

Třída `tb_str_simulator` tvoří jádro simulace. Obsahuje kontejnery pro `node` a `tube`, a v průběhu simulace s nimi pracuje. Její metoda `update` se periodicky volá pro postup simulace do dalšího stavu.
Během `update` se provedou 4 věci:
- aplikuje se gravitace
- vypočítají se vnitřní síly (propojení trubkami)
- vypočítají se vnější síly (meziobjektová kolize)
- na výsledný stav konstrukce se aplikuje pohyb (rychlost se převede na rozdíl umístění)

Standartně se `tb_str_simulator` získá přes speciální třídu `tb_str_builder`, která zajistí aby byly splněny invarianty struktury (hlavně validní pointery, které mohou být invalidovány postupnou výstavbou).
`tb_str_builder` staví na základě manuálně volaných metod. Pro jednoduché načítání scény ze složky, std. vstupu apod. existuje třída `simple_istream_loader`, která `tb_str_builder` obsahuje.

Pro zobrazování se stará jednoduchá třída `render`, která má jedinou důležitou metodu vykreslení dané instance trubkové struktury.
