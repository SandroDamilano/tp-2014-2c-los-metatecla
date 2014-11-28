IPVM2 := 127.0.0.1
IPVM3 := 127.0.0.1
IPVM4 := 127.0.0.1

clonarcommons:
	git clone https://github.com/sisoputnfrba/so-commons-library.git ../so-commons-library

instalarcommons:
	-cd ../so-commons-library && sudo $(MAKE) install

#En caso de que no esten la commons en la maquinas
#Clona y instala las commons
commons:
	$(MAKE) clonarcommons
	$(MAKE) instalarcommons

#En caso de no tener internet
#Copia a las maquinas de la red las commons y el tp (Setear ips antes)
scp-copiar-red:
	-cd .. && scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IPVM2):/home/utnso/
	-cd .. && scp -rpC ./so-commons-library utnso@$(IPVM2):/home/utnso/
	
	-cd .. && scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IPVM3):/home/utnso/
	-cd .. && scp -rpC ./so-commons-library utnso@$(IPVM3):/home/utnso/
    	
	-cd .. && scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IPVM4):/home/utnso/
	-cd .. && scp -rpC ./so-commons-library utnso@$(IPVM4):/home/utnso/

#Setear entorno con las librerias
setear-entorno:
	./set_environment.sh
