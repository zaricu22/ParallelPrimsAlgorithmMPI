# ParallelPrimsAlgorithmMPI
 
SEKVENCIJALNO RUCNO:

Ne mozemo samo jednostavno izabrati najmanje grane u grafu 
jer tako ne znaci da cemo dobiti povezano stablo

- U minimalnom stablu(niz gde za svaki cvor pamtimo vrednost njegove najmanje grane) 
na pocetku vred za svaki cvor stavimo beskonacno(nema granu/prazno stablo)
- Pocetni cvor(bilo koji) proglasimo novim minimalnim damo vrednost 0 i oznacimo kao predjeni
- Azuriramo vrednosti minimalnog stabla tako sto posmatramo nepredjene cvorove minimalnog stabla
i ako je vrednost izmedju tog nepredjenog cvora i novoizabranog minimalnog manja 
od trenutne vrednosti za taj nepredjeni cvor u minimalnom stablu promenimo je
- Medju nepredjenim cvorovima u minimalnom stablu pronadjemo onaj sa najmanjom vrednoscu(granom)
i proglasimo ga za novi minimalni i predjeni i ponovo azuriramo minimalno stablo

Algoritam ima tacno (br cvorova - 1) koraka


PARALELIZACIJA:

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

[Vreme izvrs na klasteru](Vreme izvrs na klasteru.pdf)
