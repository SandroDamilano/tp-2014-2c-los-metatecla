#/usr/bin/env sh
# Correr este script para setear el entorno desde consola con solo ./set_environment.sh

# crea un links simbolico a la libreria
sudo ln -s /home/utnso/tp-2014-2c-los-metatecla/ansisop-panel/Debug/libansisop-panel.so /usr/lib/libansisop-panel.so
sudo ln -s /home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug/libMiniGral_lib.so /usr/lib/libMiniGral_lib.so
# env var del path del archivo de configuracion del proceso CONSOLA
# export ESO_CONFIG=~/git/tp-2014-2c-los-metatecla/Consola/Debug
