#/usr/bin/env sh
# Correr este script para setear el entorno desde consola con solo ./set_environment.sh

sudo ln -s /home/utnso/tp-2014-2c-los-metatecla/ansisop-panel/Debug/libansisop-panel.so /usr/lib/libansisop-panel.so
sudo ln -s /home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug/libMiniGral_lib.so /usr/lib/libMiniGral_lib.so
echo 'Soft-links creados con éxito'

mkdir /home/utnso/archivos_swap
echo 'Directorio para swapping creado con éxito'

echo 'export ESO_CONFIG=~/tp-2014-2c-los-metatecla/Consola/Debug/config.cfg' >> ~/.bashrc
echo 'Variable de entorno ESO_CONFIG alojada en .bashrc con éxito'

echo '\nFin seteo de entorno.'
