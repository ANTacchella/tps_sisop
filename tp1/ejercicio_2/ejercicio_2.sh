#!/bin/bash

info="para mas informacion ejecute $0 -h"

Error_1(){

echo "la ruta especificada no existe o no se tiene los permisos requeridos, $info "
exit 1

}

#el criterio que tomamos para manejar los errores es que no mostramos el error original
#y mostramos el error nuestro
cd $1 2> /dev/null || Error_1


declare -A archivosCreados

declare -a archivos
archivos=(`ls`)
for arch in  ${archivos[*]}
do
	if ! test -r $arch
	then echo "no tiene permiso de lectura sobre el archivo $arch"
	else
		cat $arch >> .archOrdenado.txt
	fi
done
IFS=$'\n'
archivosCreados[".archOrdenado.txt"]=".archOrdenado.txt"
chmod 777 .archOrdenado.txt


#ordeno los datos para mandarlos ar alrchivo .archOrdendo.txt y creo el archivo .usuarios.txt que contiene los usuarios
sort -t "_" -k 1 .archOrdenado.txt | sort -t "_" -k 2 > .archOrdenado1.txt #lo envio a otro archivo porque si lo enviava al mismo archivo rompia
chmod 777 .archOrdenado1.txt

awk -F" _ " '{ gsub(/[:-]/," ",$1) } ;{us[$2]=$2}; {printf "%s _ %s _ %s\n",$1,$2,mktime($1)}; END{for(i in us) {print i > ".usuarios.txt" }}  ' .archOrdenado1.txt  > .archOrdenado.txt


archivosCreados[".archOrdenados1.txt"]=.archOrdenado1.txt

#medoy permisos sobre el archivo .usuarios.txt
chmod 777 .usuarios.txt
archivosCreados[".usuarios.txt"]=".usuarios.txt"

#cargo un diccionario de vvalores vacion, con los usuarios como clave
declare -A usuarios
usuarios=()
for item in `cat .usuarios.txt`
do
	usuarios[$item]=""
done


#cargo un array con las lineas del archivo que ordene
declare -a lines
lines=(`cat .archOrdenado.txt`)


#cat .archOrdenado.txt #aca muestro el archivo ordenado para ver que halla quedado bien


for l in ${lines[*]}
do
	usr=`echo $l | awk -F" _ " ' {print  $2}' ` # me fijo en el usuario de la linea

	#si para el usuario de la linea no hay una llamada iniciada, entonces guardo cuando se inicio la llamada y la linea
	#la mado al archivo de la fecha de inicio de la llamada, en caso de que la llamada este iniciada, mando la linea al archivo de la fecha de inicio
	#y marco que no hay llamadas activas
	if [ -z  "${usuarios[$usr]}" ];
	then
		usuarios[$usr]=`echo $l | awk -F" _ " '{print  strftime("%Y-%m-%d",$3) } '`
		t="${usuarios[$usr]}"
	else
		t="${usuarios[$usr]}"
		usuarios[$usr]=''
	 fi

	nombre_arch=".$t.txt"
	archivosCreados[$nombre_arch]=$nombre_arch
	echo $l >> $nombre_arch
done
#la parte del codigo de arriba me termina generando N archivos dependiendo los N dias que se iniciaron llamadas(los fines de llamadas se cuentan para el dia del inicio de la misma)
#los N archivos estan ordenados individualmente por fecha hora y usuario

#en este pedaso tendria que ir la parte que resuelve lo que pide el enunciado
.
.
.
.
#



#elimino los archivos auxiliares que cree
for j in ${archivosCreados[*]}
do
     rm $j
done