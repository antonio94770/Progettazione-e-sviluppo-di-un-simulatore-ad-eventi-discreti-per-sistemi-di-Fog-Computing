import sqlite3
import os
import random

database_sorgente = "Tesi.db"
percorso = "../../simulations/"

textFileini = """[General]
network = Map
sim-time-limit = 3600s
Map.Modena.**.sendInterval = exponential(1)
Map.Modena.**.serviceTime = exponential(0.002)\n"""

Source = []
LinkSourceFog = []
Fog = []
LinkFogSink = []
Sink = []

def read_database(c, percentuale):
	c.execute('SELECT * FROM Source')
	for row in c.fetchall():
		rng = random.randint(1,100)
		if (rng <= percentuale):
			Source.append(row)
			print (row)
	
	c.execute('SELECT * FROM LinkSourceFog')
	print ("\n\n\n")
	for row in c.fetchall():
		if (row[0] in [s[0] for s in Source]):
			LinkSourceFog.append(row)
			print (row)
			
	c.execute('SELECT * FROM Fog')
	print ("\n\n\n")
	for row in c.fetchall():
		if (row[0] in [l[1] for l in LinkSourceFog]):
			Fog.append(row)
			print (row)
			
	c.execute('SELECT * FROM LinkFogSink')
	print ("\n\n\n")
	for row in c.fetchall():
		if (row[0] in [l[0] for l in Fog]):
			LinkFogSink.append(row)
			print (row)
			
	c.execute('SELECT * FROM Sink')
	print ("\n\n\n")
	for row in c.fetchall():
		if (row[0] in [l[1] for l in LinkFogSink]):
			Sink.append(row)
			print (row)
			
def write_database(c, conn, percentuale):
	
	c.execute('''CREATE TABLE IF NOT EXISTS Source (
		ID	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
		Indirizzo	TEXT NOT NULL,
		Latitudine	REAL NOT NULL,
		Longitudine	REAL NOT NULL,
		Periferia	TEXT NOT NULL
		)''');

	
	c.execute('''CREATE TABLE IF NOT EXISTS Fog (
		ID	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
		Indirizzo	TEXT NOT NULL,
		Latitudine	REAL NOT NULL,
		Longitudine	REAL NOT NULL,
		Periferia	TEXT NOT NULL
		)''');
		
	
	c.execute('''CREATE TABLE IF NOT EXISTS Sink (
		ID	INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE,
		Indirizzo	TEXT NOT NULL,
		Latitudine	REAL NOT NULL,
		Longitudine	REAL NOT NULL,
		Periferia	TEXT NOT NULL
		)''');
		
		
	c.execute('''CREATE TABLE IF NOT EXISTS LinkSourceFog (
		IDSource	INTEGER NOT NULL,
		IDFog	INTEGER NOT NULL,
		Distanza	REAL,
		FOREIGN KEY(IDSource) REFERENCES Source(ID),
		FOREIGN KEY(IDFog) REFERENCES Fog(ID),
		PRIMARY KEY(IDSource,IDFog)
		)''');
		
	c.execute('''CREATE TABLE IF NOT EXISTS LinkFogSink (
		IDFog	INTEGER NOT NULL,
		IDSink	INTEGER NOT NULL,
		Distanza	REAL,
		FOREIGN KEY(IDFog) REFERENCES Fog(ID),
		FOREIGN KEY(IDSink) REFERENCES Sink(ID),
		PRIMARY KEY(IDFog,IDSink)
		)''');
		
	conn.commit()
	


	c.execute("DELETE FROM Source")
	c.execute("DELETE FROM sqlite_sequence where name='Source'")
	
	c.execute("DELETE FROM Sink")
	c.execute("DELETE FROM sqlite_sequence where name='Sink'")
	
	c.execute("DELETE FROM Fog")
	c.execute("DELETE FROM sqlite_sequence where name='Fog'")
	
	c.execute("DELETE FROM LinkSourceFog")
	
	c.execute("DELETE FROM LinkFogSink")
	
	conn.commit()
		
	for s in Source:
		c.execute("INSERT INTO Source(ID,Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?,?)", (s[0], s[1], s[2], s[3], s[4]))
		
	for s in Sink:
		c.execute("INSERT INTO Sink(ID,Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?,?)", (s[0], s[1], s[2], s[3], s[4]))
		
	for f in Fog:
		c.execute("INSERT INTO Fog(ID,Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?,?)", (f[0], f[1], f[2], f[3], f[4]))
		
	for l in LinkSourceFog:
		c.execute("INSERT INTO LinkSourceFog(IDSource,IDFog, Distanza) VALUES(?,?,?)", (l[0], l[1], l[2]))
		
	for l in LinkFogSink:
		c.execute("INSERT INTO LinkFogSink(IDFog,IDSink, Distanza) VALUES(?,?,?)", (l[0], l[1], l[2]))
		
	conn.commit()
	
	
def write_fileini(nome_nuovo_file):
	with open(percorso + nome_nuovo_file + ".ini", 'w') as f:
		f.write(textFileini)
		f.write("Map.Modena.subsampling_database = \"" + "simulations/databases/" + nome_nuovo_file + ".db\"\n")
		
		
		
def main():
	conn = sqlite3.connect(database_sorgente)
	c = conn.cursor()
	
	while True:
		try:
			percentuale = int(input('Percentuale Source: '))
			if percentuale < 1 or percentuale > 100:
				raise ValueError
			break
		except ValueError:
			print("Inserisci un intero tra 1 e 100.")

	#carico l'intero database
	read_database(c, percentuale)
	
	c.close()
	conn.close()
	
	#creo nuovo database contenente il subsampling
	nome_nuovo_file = "PercSource_" + str(percentuale)

	if not os.path.exists(percorso + "databases/"):
		os.makedirs(percorso + "databases/")

	conn = sqlite3.connect(percorso + "/databases/"+ nome_nuovo_file + ".db")
	c = conn.cursor()
	
	write_database(c, conn, percentuale)
	write_fileini(nome_nuovo_file)
	
 
if __name__ == '__main__':
    main()