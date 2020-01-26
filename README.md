# ParallelPrimsAlgorithmMPI
 
SEKVENCIJALNO RUČNO(SALG.c):

Ne mogu se samo jednostavno izabrati najmanje grane u grafu 
jer tako ne znači da se dobija povezano stablo.

- U minimalnom stablu(niz koji za svaki čvor stabla pamti vrednost njegove najmanje grane) 
na početku svakom čvora stabla dodeli se vrednost beskonačno(nema granu/prazno stablo)
- Početni čvor(bilo koji) se proglasi za novim minimalnim, dodelimu se vrednost 0 i označi se kao pređeni
- Ažuriraju se vrednosti minimalnog stabla tako što se posmatraju samo nepređeni čvorovi stabla:
   ako je vrednost(grana u grafu) između nepređenog čvora i novoizabranog minimalnog čvora
   manja od trenutne vrednosti(grana u stablu) za nepređeni čvor
   postavi se prva vrednost(grana u grafu) kao nova vrednost za nepređeni čvor u minimalnom stablu
- Među nepređenim čvorovima u minimalnom stablu pronađe se onaj sa najmanjom vrednošću(granom)
proglasi se za novi minimalni, označi se kao pređeni i ponovi se ažuriranje stabla za novi minimalni čvor
- Koraci pronalaženja minimalnog nepređenog čvora stabla i ažuriranja stabla za isti, 
ponavlja se dok ima nepređenih čvorova u stablu

Algoritam ima tačno (broj čvorova grafa - 1) koraka


PARALELIZACIJA(MINSTABLO.c):

- Podelimo graf(matrica) i minimalno stablo(niz) na procese
- Paralelizujemo samo azuriranje minimalnog stabla za novoizabrani minimalni cvor
root primi delove i nadje minimum koji prosledji svima ostalima i ponovo azuriranje


VREME IZVRSAVANJA NA KLASTERU:

Od samog pocetka razmatranja paralelizacije koraka Primovog algoritma
uocavaju se problemi sa prekomernom komunikacijom i razmenom podataka.

Sekvancijalan algoritam je vec izuzetno optimizovan,
tako da sa povecanjem broja procesa povecava se komunikacija
sto samo dovodi do veceg vremena izvrsavanja
Pre ce jedan proces da prodje kroz ceo niz i uporedi,
nego da svaki proces to uradi za svoj deo niza pa posalje glavnom procesu

MALO TOGA SE MOZE PARALELIZOVATI
- Ne moze se paralelizovati FOR petlja 
jer svaka naredna iteracija zavisi od novoizabranog minimalnog cvora iz prethodne,
- Ne moze se paralelizovati pronalazenje minimalnog cvora(zahteva nepotrebnu dodatnu komunukaciju)
- Mogu se paralelizovati koraci unutar FOR petlje i to azuriranje minimalnog stabla(niz) za novoizabrani minimalni cvor
(zbog komunikacije brze jedan proces)

[Vreme izvrs na klasteru](https://github.com/zaricu22/ParallelPrimsAlgorithmMPI/blob/master/Vreme%20izvrs%20na%20klasteru.pdf)
