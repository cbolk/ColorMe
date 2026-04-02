# ColorMe -- Versione 1

## Autore
Thomas Colombo

## Descrizione

Il sistema integra un sensore di colore (basato su tecnologia a fotodiodi con filtro IR) capace di identificare le componenti cromatiche di una superficie previa calibrazione da parte dell'utente.

Il segnale acquisito viene elaborato dalla scheda per generare coordinate nel modello di colore RGB e la relativa denominazione testuale (es. R: 255 G: 0 B: 0 colore = 'Rosso').

L'output è fruibile simultaneamente tramite un display OLED integrato e tramite due web app dedicate, gestite dal modulo ESP32:

+ **Web App di Configurazione** (Modalità Access Point): una rete Wi-Fi locale generata dal dispositivo permette l'accesso a un'interfaccia dedicata alla visualizzazione rapida dei dati e, soprattutto, all'impostazione di SSID e Password della rete esterna.

+ **Web App di Monitoraggio** (Modalità Station): una volta connesso al router, il dispositivo ospita una seconda interfaccia ottimizzata per la consultazione dei dati da qualsiasi browser all'interno della rete locale.




