#!/bin/bash

Error_1(){
	echo "la ruta especificada no existe o no se tiene los permisos requeridos, $info"
	exit 1
}

info="para mas informacion ejecute $0 -h"

#el criterio que tomamos para manejar los errores es que no mostramos el error original y mostramos el nuestro
cd $1 2> /dev/null || Error_1

declare -a archivos
archivos=(`ls`)
for arch in ${archivos[*]}
do 
	if ! test -r $arch
	then echo "no tiene permiso de lectura sobre el archivo $arch"
	else
	cat $arch >> .archOrdenado.txt
	fi
done
IFS=$'\n'
chmod 777 .archOrdenado.txt
cat .archOrdenado.txt | sort -k1 -t"_" | sort -k2 -t"_" | awk -F" _ " '{ gsub(/[:-]/," ",$1)} ; {printf "%s _ %s _ %s\n",$1,$2,mktime($1) }' > .archOrdenado.txt

declare -a lines
lines=(`cat .archOrdenado.txt`)

declare -A llamada_activa

for l in ${lines[*]}
do
	t=`echo $1 | awk -F" _ " '{print strftime("%Y-%m-%d",$3) } '`
	nombre_arch=".$t.txt"
	echo $1 >> $nombre_arch
done
