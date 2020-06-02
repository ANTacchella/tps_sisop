#!/bin/bash

# ejercicio_3.sh | Trabajo Práctico 1) Ejercicio 3) | Segunda reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

info="para más información ejecute $0 -h o $0 --help"

#Ayuda del script
Help(){
    echo "######    HELP $0     ######"
    echo -e "\nEste script se encargará de borrar los archivos de log antiguos de las compañías dejando únicamente el más reciente perteneciente a cada una."
    echo -e "Este proceso se repetirá constantemente en un intervalo de tiempo especificado por el usuario."
    echo -e "\nEl script recibe dos parámetros:\n-f 'path': Directorio en el que sen encuentran los archivos de log\n-t 'segundos': Intervalo de tiempo a evaluar el directorio (Entero positivo)"
    echo -e "\nPara detener el proceso luego de iniciado solo debe ejecutar el comando 'kill' seguido del PID que es impreso al llamar al script"
    exit 1
}

Error_1(){
    echo "La ruta especificada no existe o no se tienen los permisos requeridos, $info"
    exit 1
}

#Verifico que la cantidad de parámetros sea correcta
if test $# -ne 4 && test "$1" != "-h" && test "$1" != "--help"
then
    echo "Error: Los parámetros son inválidos, $info"
    exit 1
fi

#Formateo los parámetros
options=$(getopt -o f:t:h --long 'help' -- "$@")
[ $? -eq 0 ] || {
    echo "Error: Los parámetros son inválidos, $info"
    exit 1
}
eval set -- "$options"
while true; do
    case "$1" in
    -h|--help)
        Help
        exit 1
        ;;
    -f) 
        directorio=$2
        # echo "Directorio: $directorio"
        ;;
    -t)
        intervalo=$2
        # echo "Intervalo: $intervalo"
        [[ ! $intervalo =~ [0-9]+$ ]] && {
            echo "Error: El segundo parámetro debe ser un número entero positivo, $info"
            exit 1 
        }
        ;;
    --)
        shift
        break;;
    esac
    shift
done
    

cd $directorio 2> /dev/null || Error_1

echo "PID: $(($$+2))"

borrarArchivos(){

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
            archivos=($(echo $(ls -1 | grep -E $regex | grep "$i") | tr " " "\n"))
            
            for (( j=0; j<${#archivos[*]}-1; j++))
            do
                rm -f "${archivos[$j]}"
            done
        fi
    done 

    # echo "hecho"
    sleep $1

done

}

borrarArchivos $intervalo &