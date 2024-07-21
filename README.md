# Translation Lookaside Buffer (TLB)

## Recherche und Implementierung eines TLB in SystemC

Ein Translation Lookaside Buffer (TLB) ist ein spezieller Cache, der die
Adressübersetzung von virtuellen in physische Adressen beschleunigt.
Moderne TLBs haben meist 32 bis 512 Einträge, größere TLBs sind in Servern und
High-End-Systemen üblich. Die Effektivität eines TLBs bemisst sich in seiner
Trefferrate, welche häufig über 99% liegt. Ein TLB-Treffer kann Speicherzugriffe
erheblich beschleunigen, indem er mehrere Speicherzugriffe für die Adressübersetzung
vermeidet.

TLB-Zugriffe dauern nur wenige CPU-Zyklen, während vollständige Page-Table-Walks
ohne TLB-Unterstützung deutlich länger brauchen. Mehrstufige Page-Tables in modernen
Systemen reduzieren den Speicherbedarf, erhöhen jedoch die Zugriffszeiten, die
ein TLB minimieren kann. Hauptspeicherzugriffe dauern oft 100 oder mehr CPU-Zyklen;
TLBs verringern diese Zahl erheblich und verbessern so die Gesamtleistung.


## Ergebnis der Berechnung der Summe über einer verketteten Liste

Im Rahmen des Projekts sollten wir zudem das Speicherzugriffsverhalten einer Summe über 
einer verketteten Liste untersuchen. Dazu haben wir für die TLB-Latenz den Wert 3, für 
die Speicher-Latenz den Wert 200 und für den Parameter Blocksize den Wert 4096 gewählt. 

Als TLB-Größe haben wir alle Zweierpotenzen zwischen 0 und 1024 gewählt und Listengrößen 
zwischen 1000 und 100000 Einträgen simuliert. Wie aus den Tabellen zu entnehmen ist, 
sind wir zu dem Ergebnis gekommen, dass die Laufzeit/Anzahl der Zyklen direkt von der 
TLB-Größe abhängt. Gerade der TLB mit 1024 Einträgen weist dabei eine sehr hohe Hit-Rate
mit durchgängig über 99% auf, während TLBs mit Größe <= 16 bereits bei Listen mit 
Länge 1000 schon unter 60% liegen. 

Die Bedeutung des TLB zeigt sich hier aber besonders dadurch, dass selbst der TLB mit nur 
einer Cachezeile circa 25% weniger Zyklen als eine Implementation ohne TLB benötigt. Der 
TLB mit den meisten Einträgen kann die Anzahl der Zyklen sogar knapp halbieren.

#### Hit-Rate in Abhängigkeit von der TLB-Größe
|        | 1       | 2       | 4       | 8       | 16      | 32     | 64      | 128     | 256     | 512     | 1024    | 
|--------|---------|---------|---------|---------|---------|--------|---------|---------|---------|---------|---------|
| 1000   | 50,6    | 51,15   | 52,25   | 54,85   | 59,1    | 69,1   | 86      | 95,65   | 95,65   | 95,65   | 95,65   |
| 10000  | 49,9    | 50,27  | 50,885  | 52,32   | 54,87   | 59,885 | 69,825  | 90,62   | 99,21   | 99,21   | 99,21   |
| 50000  | 49,735  | 49,849 | 50,069  | 50,472  | 51,32   | 52,992 | 56,346  | 63,005  | 76,61   | 99,53   | 99,53   |
| 100000 | 49,6715 | 49,738 | 49,8505 | 50,0965 | 50,5405 | 51,489 | 53,3455 | 56,9985 | 64,4575 | 79,3395 | 99,5695 |

#### Zyklen in Abhängigkeit von der TLB-Größe
|         | 0        | 1        | 2        | 4        | 8        | 16       | 32       | 64       | 128      | 256      | 512      | 1024     |
|---------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|----------|
| 1000    | 808000   | 605600   | 603400   | 599000   | 588600   | 571600   | 531600   | 464000   | 425400   | 425400   | 425400   | 425400   |
| 10000   | 8080000  | 6084000  | 6069200  | 6044600  | 5987200  | 5885200  | 5684600  | 5287000  | 4455200  | 4111600  | 4111600  | 4111600  |
| 50000   | 40400000 | 30453000 | 30430200 | 30386200 | 30305600 | 30136000 | 29801600 | 29130800 | 27799000 | 25058000 | 20494000 | 20494000 |
| 100000  | 80800000 | 60931400 | 60904800 | 60859800 | 60761400 | 60583800 | 60204400 | 59461800 | 58000600 | 55017000 | 49064200 | 40972200 |


## Beiträge der Gruppenmitglieder

### Lukas Greinwald
- Parsing von CSV-Dateien und Behandlung von Fehlern im Format inklusive Tests.
- Berechnung der benötigten Gatter.
- Erstellung von Analysen und Auswertungen.

### Enes Rafioglu
- Recherche zu TLBs in modernen Systemen.
- TLB-Circuit-Design in Logisim Evolution.
- Implementierung des überarbeiteten TLB (nicht mehr basierend auf einer `std::map`).

### Rubin Raithel
- Parsen und Validieren von Argumenten inklusive Tests.
- Erstellung des Makefiles und der Buildumgebung.
- Entwicklung des Simulationsrahmens (CPU, MMU, Memory Module) und Utility-Funktionen/Makros.
- Aufsetzen der Testumgebung und Durchführung von Simulationstests.

Es sei angemerkt, dass sich viele Arbeitsbereiche auch überschnitten haben und
eine scharfe Trennung nicht möglich ist.
