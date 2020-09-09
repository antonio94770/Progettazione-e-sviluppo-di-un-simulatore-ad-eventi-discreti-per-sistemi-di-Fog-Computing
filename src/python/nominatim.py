from geopy.geocoders import Nominatim
from geopy.exc import GeocoderTimedOut
import time

elenco_strade = "lista_strade.txt"
output_file = 'lista_dati.txt'
strade = []
data = []


def do_geocode(address, addressdetails=True):
	time.sleep(4)
	geopy = Nominatim(user_agent="Thesis")
	try:
		return geopy.geocode(address, addressdetails=True, timeout = 100)
	except GeocoderTimedOut:
		return do_geocode(address, addressdetails=True)
		
	

with open(elenco_strade, errors='replace') as f:
	strade = f.readlines()
	
	
for strada in strade:
	line = strada.rstrip()
	location = do_geocode(line + " MODENA")
	#reverse_location = do_geocode_reverse(location.latitude, location.longitude)
	if(location):				
		line = line + "\t" + "{:.9f}".format(location.latitude) + "\t" + "{:.9f}".format(location.longitude) + "\t"
		if 'suburb' in location.raw['address']:
			line = line + location.raw['address']['suburb']
			print(line)
			data.append(line + "\n")
		
		
with open(output_file, 'w') as file:
	for i in data:
		file.write(i)
