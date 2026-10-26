# Genetic OpCode

Un piccolo laboratorio C++ per far evolvere sequenze di byte. L'obiettivo futuro è osservare come queste sequenze vengono interpretate da decoder, emulatori e, in una fase successiva, da hardware reale. Per ora il progetto è un **prototipo dell'algoritmo genetico**: genera cromosomi, li seleziona, li combina e li muta. I byte prodotti non vengono eseguiti né interpretati come istruzioni valide.

## Provalo

Richiede un compilatore C++17 e `make`.

```bash
make
make run
```

`make clean` rimuove l'eseguibile generato. Puoi anche compilare direttamente:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic main.cpp -o genetic_opcode
./genetic_opcode
```

Ogni esecuzione crea 300 sequenze di 1–15 byte e mostra il migliore di 5 generazioni. Per esempio:

```text
Generazione 0, migliore: 8d b8 fc b6 c9 ... (fitness 2207)
Generazione 1, migliore: 8d b8 fc b6 c9 ... (fitness 2207)
Generazione 2, migliore: 79 af f0 d1 ed ... (fitness 2273)
```

I risultati cambiano perché il generatore casuale riceve un nuovo seed a ogni avvio. Il valore di fitness è **solo dimostrativo**: oggi è la somma dei byte. Non misura validità, novità o comportamento di un'istruzione.

## Come funziona ora

```text
popolazione iniziale
       ↓
fitness provvisoria
       ↓
selezione dei genitori → crossover → mutazione
       ↓
nuova popolazione, con i due migliori conservati
```

| File | Ruolo |
| --- | --- |
| `main.cpp` | Crea la popolazione, esegue il ciclo e stampa il migliore. |
| `chromosome.hpp` | Rappresenta una sequenza di byte e il suo punteggio. |
| `geneticAlgo.hpp` | Selezione, crossover, mutazione ed elitismo. |
| `fitness.hpp` | Funzione di fitness provvisoria. |

## Direzione del progetto

L'idea è trasformare ogni sequenza di byte in un candidato da osservare con più oracoli indipendenti:

1. Confronto fra decoder x86, registrando validità, lunghezza e istruzione interpretata.
2. Confronto dello stato e delle eccezioni in ambienti di esecuzione controllati.
3. Misure ripetute di timing e altri effetti osservabili, con metadati sufficienti a riprodurre i risultati.
4. Una fitness che premi differenze e novità confermate, invece della somma dei byte.

Questi passaggi sono proposte: **non sono ancora implementati**. Il prossimo intervento previsto è dare un significato utile alla fitness e definire come registrare le osservazioni.

## Stato e limiti

Il ciclo attuale è pensato per essere leggibile e verificabile, non per produrre risultati di ricerca. Il crossover taglia i due genitori in punti indipendenti, quindi un figlio può essere più lungo o più corto di entrambi (sempre fra 1 e 15 byte). Alcuni operatori usano ancora puntatori grezzi e la mutazione della lunghezza (`mutateLength`) esiste ma non è usata nel ciclo. Nessun byte generato viene eseguito sulla CPU.
