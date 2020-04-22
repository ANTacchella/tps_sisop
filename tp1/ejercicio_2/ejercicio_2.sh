#!/bin/bash

Error_1(){
	echo "la ruta especificada no existe o no se tiene los permisos requeridos, $info"
	exit 1
}

info="para mas informacion ejecute $0 -h"

#el criterio que tomamos para manejar los errores es que no mostramos el error original y mostramos el nuestro
cd $1 2> /dev/null || Error_1

declare -a archivos
for arch in  ${archivos[*]}
do
        if ! test -r $arch
        then echo "no tiene permiso de lectura sobre el archivo $arch"
        else
        cat $arch >> .archOrdenado.txt
        fi
done
IFS=$'\n'


chmod 777 .archOrdenado.txt
#ordeno los datos para mandarlos ar alrchivo .archOrdendo.txt y creo el archivo .usuarios.txt que contiene los usuarios
cat .archOrdenado.txt | sort -k1 -t"_" | sort -k2 -t"_" | awk -F" _ " '{ gsub(/[:-]/," ",$1) } ;{us[$2]=$2}; {printf "%s _ %s _ %s\n",$1,$2,mktime($1)}; END{for(i in us) {print i > ".usuarios.txt" }}  ' > .archOrdenado.txt

#medoy permisos sobre el archivo .usuarios.txt
chmod 777 .usuarios.txt


#cargo un diccionario de vvalores vacion, con los usuarios como clave
declare -A usuarios

for item in `cat .usuarios.txt`
do
        usuarios[$item]=''

done

echo ${!usuarios[*]}
echo "** ${usuarios[*]} **"

#esto lo meti para probar si me funcionababien la logica de preguntar por lo que tenia el array de usuarios
for item in ${usuarios[*]}
do
        if [ -z "$item" ];#no se si esto esta funcinoando bien tanto aca como en la parte de abajo
        then
                echo "bien"
        else
                echo "mal"
        fi
done

declare -a lines
lines=(`cat .archOrdenado.txt`)


#los echo que hay tirados son para ir viendo lo que iba pasando cno las variables, la variable usuarios creo que no la estoy tratando bien o no esta funcionando

for l in ${lines[*]}
do

        usr=`echo $l | awk -F" _ " ' {print  $2}' `

        echo $usuarios[$usr]

        if [ -z  "$usuarios[$usr]" ];
        then
                usuarios[$usr]=`echo $l | awk -F" _ " '{print  strftime("%Y-%m-%d",$3) } '`
                t="$usuarios[$usr]"
                echo "1"
        else
                t="$usuarios[$usr]"
                usuarios[$usr]=''
        echo "0"
        fi

        echo "$t**$usr"
        nombre_arch=".$t.txt"
        echo $l >> $nombre_arch
done


rm .archOrdenado.txt

echo ${#lines[*]}



