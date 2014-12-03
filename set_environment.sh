#/usr/bin/env sh
# Correr este script para setear el entorno desde consola con solo ./set_environment.sh

# crea un links simbolico a la libreria
sudo ln -s /home/utnso/git/tp-2014-2c-los-metatecla/ansisop-panel/Debug/libansisop-panel.so /usr/lib/libansisop-panel.so
sudo ln -s /home/utnso/git/tp-2014-2c-los-metatecla/MiniGral_lib/Debug/libMiniGral_lib.so /usr/lib/libMiniGral_lib.so
# creamos directorio para alojar archivos del swapping
mkdir /home/utnso/archivos_swap
# seteamos var de entorno para el path del archivo de configuracion del proceso CONSOLA
echo 'export ESO_CONFIG=~/tp-2014-2c-los-metatecla/Consola/Debug/config.cfg' >> ~/.bashrc
