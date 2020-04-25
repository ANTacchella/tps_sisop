#!/bin/bash

Error_1(){
    echo "La ruta especificada no existe o no se tienen los permisos requeridos, $info"
    exit 1
}

info="para más información ejecute $0 -h"


if test $# -ne 2
then
    echo "Error: Los parámetros son inválidos, $info"
    exit 1
fi

cd $1 2> /dev/null || Error_1

re_num='[0-9]+$'
if ! [[ $2 =~ $re_num ]]
then
    echo "Error: El segundo parámetro debe ser un número entero positivo, $info"
    exit 1
fi

declare -A apariciones
declare -a archivos

while true;
do
    #Regex para nombreEmpresa-numeroSemana.log
    regex='[a-zA-Z]+\-[0-9]+\.log$'

    #Guardo en este array el solo el nombre de la compañía de cada archivo válido
    archivos=$(ls | grep -E $regex | cut -f 1 -d "-")

    #Array asociativo por nombre de compañía
    #Incremento uno por cada aparición de un archivo de la compañía en el directorio
    for arch in ${archivos[*]}
    do
        (( apariciones[$arch]++ ))
    done

    #Recorro cada compañía y si tiene más de una aparición elimino las primeras/más antiguas
    for i in ${!apariciones[*]}
    do
        if test ${apariciones[$i]} -ge 2
        then 
            archivos=($(echo $(ls -1 | grep "$i") | tr " " "\n"))
            # echo ${#archivos[*]}
            # echo ${archivos[*]}
            for (( j=0; j<${#archivos[*]}-1; j++))
            do
                rm -f "${archivos[$j]}"
            done
        fi
    done 

    sleep $2

done