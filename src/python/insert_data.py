import sqlite3
from geopy.distance import vincenty
from collections import OrderedDict


conn = sqlite3.connect('Tesi.db')
c = conn.cursor()

elenco_strade = "lista_dati.txt"
circoscrizione = []
generator = []
fog = []
sink = []

def delete_generator_table():
	c.execute("DELETE FROM Source")
	c.execute("DELETE FROM sqlite_sequence where name='Source'")
	conn.commit()

def insert_generator_table():
	for gen in generator:
		c.execute("INSERT INTO Source(Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?)", (gen.strip().split('\t')[0], gen.strip().split('\t')[1], gen.strip().split('\t')[2], gen.strip().split('\t')[3]))
	conn.commit()
	
	
def delete_sink_table():
	c.execute("DELETE FROM Sink")
	c.execute("DELETE FROM sqlite_sequence where name='Sink'")
	conn.commit()

def insert_sink_table():
	for s in sink:
		c.execute("INSERT INTO Sink(Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?)", (s.strip().split('\t')[0], s.strip().split('\t')[1], s.strip().split('\t')[2], s.strip().split('\t')[3]))
	conn.commit()
	
def delete_fog_table():
	c.execute("DELETE FROM Fog")
	c.execute("DELETE FROM sqlite_sequence where name='Fog'")
	conn.commit()

def insert_fog_table():
	for f in fog:
		c.execute("INSERT INTO Fog(Indirizzo,Latitudine,Longitudine,Periferia) VALUES(?,?,?,?)", (f.strip().split('\t')[0], f.strip().split('\t')[1], f.strip().split('\t')[2], f.strip().split('\t')[3]))
	conn.commit()

def delete_linksourcefog_table():
	c.execute("DELETE FROM LinkSourceFog")
	conn.commit()	
	
def insert_linksourcefog_table():
	c.execute("INSERT INTO LinkSourceFog(IDSource,IDFog) SELECT Source.ID, Fog.ID FROM Fog JOIN Source ON Fog.Periferia = Source.Periferia")
	conn.commit()
	
	c.execute("SELECT * FROM LinkSourceFog")
	empty_rows = c.fetchall()
	for row in empty_rows:
		c.execute("SELECT Source.Latitudine, Source.Longitudine, Fog.Latitudine, Fog.Longitudine FROM Source, Fog WHERE Source.ID = ? AND Fog.ID = ?", (row[0], row[1]))
		lat_long_source_fog = c.fetchall()
		for row1 in lat_long_source_fog:
			meters = vincenty((row1[0], row1[1]), (row1[2], row1[3])).meters
			c.execute("UPDATE LinkSourceFog SET Distanza = ? WHERE IDSource = ? AND IDFog = ? ", (meters, row[0], row[1]))
			conn.commit()
	
def delete_linkfogsink_table():
	c.execute("DELETE FROM LinkFogSink")
	conn.commit()
	
def insert_linkfogsink_table():
	c.execute("INSERT INTO LinkFogSink(IDFog,IDSink) SELECT Fog.ID, Sink.ID FROM Fog, Sink")
	conn.commit()
	
	c.execute("SELECT * FROM LinkFogSink")
	empty_rows = c.fetchall()
	for row in empty_rows:
		c.execute("SELECT Fog.Latitudine, Fog.Longitudine, Sink.Latitudine, Sink.Longitudine FROM Fog, Sink WHERE Fog.ID = ? AND Sink.ID = ?", (row[0], row[1]))
		lat_long_fog_sink = c.fetchall()
		for row1 in lat_long_fog_sink:
			meters = vincenty((row1[0], row1[1]), (row1[2], row1[3])).meters
			c.execute("UPDATE LinkFogSink SET Distanza = ? WHERE IDFog = ? AND IDSink = ? ", (meters, row[0], row[1]))
			conn.commit()


with open(elenco_strade, errors='replace') as f:
	strade = f.readlines()
	

for strada in strade:
	circoscrizione.append(strada.strip().split('\t')[3])
	
#questa operazione rallenta di molto le operazioni perchè mantiene un elenco ordinato	
circoscrizione = OrderedDict.fromkeys(circoscrizione)

#aggiungo via del mercato per il sink
sink.append("Viale del MERCATO	44.6594137	10.9365955	Crocetta-San Lazzaro-Modena Est")

for circ in circoscrizione:
	for strada in strade:
		if circ in strada.strip().split('\t')[3:] and strada.strip() not in sink and strada.strip() not in fog:
			fog.append(strada.strip())
			break

for strada in strade:
	if strada.strip() not in fog and strada.strip() not in sink:
		generator.append(strada.strip())

		
		
delete_generator_table()		
insert_generator_table()

delete_fog_table()		
insert_fog_table()

delete_sink_table()		
insert_sink_table()

delete_linksourcefog_table()
insert_linksourcefog_table()

delete_linkfogsink_table()
insert_linkfogsink_table()


c.close()
		


	
	
#print
'''for f in fog:
	print (f)
	
for g in generator:
	print (g)'''
