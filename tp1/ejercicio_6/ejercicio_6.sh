#!/bin/bash

# ejercicio_6.sh | Trabajo Práctico 1) Ejercicio 6) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

info="para más información ejecute $0 -h o $0 --help"

#Ayuda del script
Help(){
    echo "######    HELP $0     ######"
    echo -e "\nEste script se encargará de sumar todos los números fraccionarios contenidos en el archivo provisto por el usuario y de devolver el número resultante en un archivo 'salida.out' ubicado en el mismo directorio del script."
    echo -e "Las fracciones del archivo enviado deben estar separadas por coma (,) y las mismas pueden ser positivas, negativas o en formato mixto."
    echo -e "\nEl script recibe un único parámetro:\n#\t-f 'path': ruta del archivo de entrada ya sea de manera relativa o absoluta"
    exit 1
}

#Verifico que la cantidad de parámetros sea correcta
if test $# -ne 2 && test "$1" != "-h" && test "$1" != "--help"
then
    echo "Error: Los parámetros son inválidos, $info"
    exit 1
fi

#Formateo los parámetros
options=$(getopt -o f:h --long 'help' -- "$@")
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
        archivo=$2
        ;;
    --)
        shift
        break;;
    esac
    shift
done

if ! [[ -r "$archivo" ]]
then
    echo "Error: no tiene permisos de lectura para el archivo enviado, $info"
    exit 1
fi

lineas=`wc -l $archivo | cut -f 1 -d " "`
if [[ $lineas -gt 0 ]]
then
    echo "Error: el archivo no debe contener más de una línea, $info"
    exit 1
fi

linea=`cat $archivo`
if [[ $linea =~ [a-zA-Z]+ ]]
then
    echo "Error: el archivo no debe contener letras, $info"
    exit 1
fi

MCD(){
    if [[ $2 -eq 0 ]]
    then
        mcd=$1
    else
        MCD $2 $(($1%$2))
    fi
}

fracciones=$(echo `cat $archivo | tr "," "\n"`)

declare -a norm
declare -a neg
declare -a mix
declare -a mix_neg

num=1
den=1

for f in ${fracciones[*]}
do
    if [[ $f =~ ^[0-9]+\/[0-9]+$ ]]
    then
        norm[${#norm[*]}]=$f
    elif [[ $f =~ ^\-[0-9]+\/[0-9]+$ ]]
    then
        neg[${#neg[*]}]=$f
    elif [[ $f =~ ^[0-9]+\:[0-9]+\/[0-9]+$ ]]
    then
        mix[${#mix[*]}]=$f
    elif [[ $f =~ ^\-[0-9]+\:[0-9]+\/[0-9]+$ ]]
    then
        mix_neg[${#mix_neg[*]}]=$f
    fi
done

for f in ${norm[*]}
do
    nums=($(echo $f | tr /\// "\n"))
    num=$(($num*${nums[1]}+$den*${nums[0]}))
    den=$(($den*${nums[1]}))

done

for f in ${neg[*]}
do
    nums=($(echo $f | tr /\// "\n"))
    num=$(($num*${nums[1]}+$den*${nums[0]}))
    den=$(($den*${nums[1]}))
done

for f in ${mix[*]}
do
    nums=($(echo $f | tr /[:/]/ "\n"))
    num=$(($num+${nums[0]}*$den))
    num=$(($num*${nums[2]}+$den*${nums[1]}))
    den=$(($den*${nums[2]}))
done

for f in ${mix_neg[*]}
do
    nums=($(echo $f | tr /[:/]/ "\n"))
    num=$(($num+${nums[0]}*$den))
    num=$(($num*${nums[2]}-$den*${nums[1]}))
    den=$(($den*${nums[2]}))
done

num=$(($num*1-$den*1))

MCD $num $den
let num=$(($num/$mcd))
let den=$(($den/$mcd))

if [[ $den -eq 1 ]]
then
    echo $num >salida.out
    echo $num
else
    echo $num/$den >salida.out
    echo $num/$den
fi


