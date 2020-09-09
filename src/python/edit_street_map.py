elenco_strade_errate = "strade_formato_errato.txt"
output_elenco_strade = "lista_strade.txt"
strade = []
vie = []
with open(elenco_strade_errate, errors='replace') as f:
	strade = f.readlines()
	
	for strada in strade:
		tipo = strada.split('(', 1)[1]
		tipo = tipo[tipo.find("(")+1:tipo.find(")")]
		nome_via = strada.split('(', 1)[0]
		
		
		upp_case = ""
		next = None
		l = len(nome_via)

		for index,c in enumerate(nome_via):
			if index < (l - 1):
				next = nome_via[index + 1]
			if c.isupper() or (c == " " and next.isupper):
				if(not next.islower()):
					upp_case = upp_case + c
					
		upp_case = upp_case.rstrip()
		upp_case = upp_case.lstrip()
		nome_via = nome_via[len(upp_case)+1:] + upp_case
		
		#print (nome_via)

		tipo = tipo.rstrip()
		tipo = tipo.lstrip()
		if (tipo == "Via Dell'" or tipo == "Vicolo Dell'" or tipo == "Viale Dell'" or tipo == "Parco Dell'"):
			via = tipo + nome_via + "\n"
		else:
			via = tipo + " " + nome_via + "\n"
		vie.append(via)

with open(output_elenco_strade, 'w') as file:
	for via in vie:
		file.write(via)
