Questa cartella contiene un insieme di script che permettono di ricavare informazioni(Via, latitudine,longitudine,periferia) da uno stradario ed inserirle in un database:

-modifica_formato_strade.py: modifica il formato della stringhe, che si trovano all'interno di "strade_formato_errato.txt", e le inserisce nel file "lista_strade.txt".
-modifica_nominatim.py: permette di ottenere altitudine, longitudine e periferia da una via, che verranno salvati nel file "lista_dati.txt".
-insert_data.py: inserisce le informazioni raccolte nel file "lista_strade.txt" nel database "Tesi.db".

il database è gestito con il browser sqlite
	sudo apt-get install sqlitebrowser
