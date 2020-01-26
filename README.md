# ParallelPrimsAlgorithmMPI

Aplikacija predstavlja pralelizaciju sekvencijalnog Primovog algoritma 
za pronalaženje minimalnog stabla unutar povezanog neusmerenog grafa.
Korišćena je MPI tehnologija u programskom jeziku C.

#### OPIS ALGORITMA
 
SEKVENCIJALNO(SALG.c):

Ne mogu se samo jednostavno izabrati najmanje grane u grafu 
jer tako ne znači da se dobija povezano stablo.

- U minimalnom stablu(niz koji za svaki čvor stabla pamti vrednost njegove najmanje grane) 
na početku svakom čvora stabla dodeli se vrednost beskonačno(nema granu/prazno stablo)
- Početni čvor(bilo koji) se proglasi za novim minimalnim, dodelimu se vrednost 0 i označi se kao pređeni
- Ažuriraju se vrednosti minimalnog stabla tako što se posmatraju samo nepređeni čvorovi stabla:<br/>
&nbsp;ako je vrednost(grana u grafu) između nepređenog čvora i novoizabranog minimalnog čvora<br/>
&nbsp;manja od trenutne vrednosti(grana u stablu) za nepređeni čvor<br/>
&nbsp;postavi se prva vrednost(grana u grafu) kao nova vrednost za nepređeni čvor u minimalnom stablu
- Među nepređenim čvorovima u minimalnom stablu pronađe se onaj sa najmanjom vrednošću(granom)
proglasi se za novi minimalni, označi se kao pređeni i ponovi se ažuriranje stabla za novi minimalni čvor
- Koraci pronalaženja minimalnog nepređenog čvora stabla i ažuriranja stabla za isti, 
ponavlja se dok ima nepređenih čvorova u stablu

Algoritam ima tačno (broj čvorova grafa - 1) koraka


PARALELIZACIJA(MINSTABLO.c):

- Podeli se graf(matrica) i minimalno stablo(niz) na procese
- Paralelizuje se samo ažuriranje minimalnog stabla za novi minimalni čvor stabla:<br/>
   svaki proces računa svoj deo stabla na osnovu grafa za prosleđeni minimalan čvor<br/>
   root proces prima obrađene delove ostalih procesa u celo stablo<br/>
   pronalazi minimum u stablu koji prosleđuje svim ostalim procesima 
- Postupak se ponavlja

#### REZULTATI
[Pogledati ovde](https://github.com/zaricu22/ParallelPrimsAlgorithmMPI/blob/master/Vreme%20izvrs%20na%20klasteru.pdf)

VREME IZVRŠAVANJA NA KLASTERU:

Od samog početka razmatranja paralelizacije koraka Primovog algoritma
uočavaju se problemi sa prekomernom komunikacijom i razmenom podataka.

Sekvancijalan algoritam je već izuzetno optimizovan,
tako da sa povećanjem broja procesa povećava se komunikacija
što samo dovodi do većeg vremena izvršavanja.
Brže jedan proces prolazi kroz ceo niz i upoređuje nego da 
svaki proces to uradi za svoj deo niza i pošalje glavnom procesu.

MALO TOGA SE MOŽE PARALELIZOVATI
- Ne može se paralelizovati FOR petlja 
jer svaka naredna iteracija zavisi od novoizabranog minimalnog čvora iz prethodne
- Ne može se paralelizovati pronalaženje minimalnog čvora(zahteva nepotrebnu dodatnu komunukaciju)
- Mogu se paralelizovati koraci unutar FOR petlje i to ažuriranje minimalnog stabla(niz) za novoizabrani minimalni cvor
(zbog komunikacije brze jedan proces)


