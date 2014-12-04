IP-VM2 := 192.168.1.10
IP-VM3 := 192.168.1.11
IP-VM4 := 192.168.1.12

all:

	@echo 'Compilando Commons Library...'
	-cd so-commons-library/ && sudo $(MAKE) install
	@echo 'Fin Commons'

	@echo 'Compilando ansisop-panel...'
	-cd ansisop-panel/Debug && sudo $(MAKE)
	@echo 'Fin ansisop-panel'

	@echo 'Compilando MiniGral_lib...'
	-cd MiniGral_lib/Debug && sudo $(MAKE)
	@echo 'Fin MiniGral_lib'

	@echo 'Compilando MSP...'
	-cd MSP/Debug && sudo $(MAKE)
	@echo 'Fin MSP'

	@echo 'Compilando CPU...'
	-cd CPU/Debug && sudo $(MAKE)
	@echo 'Fin CPU'

	@echo 'Compilando Kernel...'
	-cd Kernel/Debug && sudo $(MAKE)
	@echo 'Fin Kernel'

	@echo 'Compilado Consola...'
	-cd Consola/Debug && sudo $(MAKE)	
	@echo 'Fin Consola'

	@echo 'Make OK'
	
clean:

	@echo 'Compilando Commons Library...'
	-cd so-commons-library/ && sudo $(MAKE) clean
	@echo 'Fin Commons'

	@echo 'Compilando ansisop-panel...'
	-cd ansisop-panel/Debug && sudo $(MAKE) clean
	@echo 'Fin ansisop-panel'

	@echo 'Compilando MiniGral_lib...'
	-cd MiniGral_lib/Debug && sudo $(MAKE) clean
	@echo 'Fin MiniGral_lib'

	@echo 'Compilando MSP...'
	-cd MSP/Debug && sudo $(MAKE) clean
	@echo 'Fin MSP'

	@echo 'Compilando CPU...'
	-cd CPU/Debug && sudo $(MAKE) clean
	@echo 'Fin CPU'

	@echo 'Compilando Kernel...'
	-cd Kernel/Debug && sudo $(MAKE) clean
	@echo 'Fin Kernel'

	@echo 'Compilado Consola...'
	-cd Consola/Debug && sudo $(MAKE) clean
	@echo 'Fin Consola'

	@echo 'Clean OK'
	
#En caso de no tener internet
#Copia a las maquinas de la red las commons y el tp (Setear ips antes)
scp-copiar-red:
	-scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IP-VM2):/home/utnso/

	-scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IP-VM3):/home/utnso/
    	
	-scp -rpC ./tp-2014-2c-los-metatecla utnso@$(IP-VM4):/home/utnso/

#Setear entorno con las librerias
setear-entorno:
	./set_environment.sh
