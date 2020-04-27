#!/bin/bash

# ejercicio_5.sh | Trabajo Práctico 1) Ejercicio 5) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

info="para más información ejecute $0 -h o $0 --help"

#Ayuda del script
Help(){
    echo "######    HELP $0     ######"
    echo -e "\nEste script se encargará de procesar los datos proporcionados por la universidad y devolver un resumen de resultados por materia."
    echo -e "Aclaración:\n-Si un mismo alumno cumple las condiciones puede tener más de un estado para la misma materia.\n-Un ausente equivale a una nota menor que 4."
    echo -e "\nEl script recibe un único parámetro:\n-f 'path': ruta del archivo de entrada ya sea de manera relativa o absoluta"
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
        arch=$2
        ;;
    --)
        shift
        break;;
    esac
    shift
done

#valido que el archivo que me mandan sea un archivo y que tenga permiso de lectura sobre el
#falta validar que la cantidad de parametros sea igual a 1
if test -r $arch && ! test -d $arch
    then cat $arch >> .archOrdenado.txt
elif test -d $arch;
    then echo "el parametro $arch enviado corresponde con un directorio, no con un archivo"
else
    echo "no tiene permiso de lectura sobre el archivo $arch"
fi

IFS=$'\n'

cat $arch | awk -F"|" '
{
    if($0!=""){
        clave=$1"_"$2
        materias[$2]=$2
        if(clave in alu_materia)
        {}
        else{
            alu_materia[clave]=clave
            #punto 1
            if( ((4<=$3 && $3<=6 && $5!="1" ) || ($5=="1" && 4<=$6 && $6<=6) || ($5=="1" && 6<$6 &&  4<=$4 && $4<=6)) && (( 4<=$4 && $4<= 6 && $5!="2") || ($5=="2" && 4<=$6 && $6<=6) || ($5=="2" && 6<$6 && 4<=$3 && $3<=6)) && $7==""  ){
                punto1[$2]++;
            }
            #punto 2
            if( ($7 <4 &&  $7 != "") || (($3 < 4 && $5 != "1") || ($5=="1" && $6 < 4)) || (($4 < 4 && $5 != "2") || ($5=="2" && $6 < 4)) ){
                punto2[$2]++;
            }
            #punto 3
            if($6=="" && ($3<7 || $4<7) ){
                punto3[$2]++;
            }
            #punto 4
            if( ($3=="" && $5=="1" && $6=="") || ($3=="" && $5!="1") || ($4=="" && $5=="2" && $6=="") || ($4=="" && $5!=2) ){
                punto4[$2]++;
            }
        }
    }
}
END{
    printf "\n%4s |%10s |%8s |%10s |%10s |%12s \n","LINE","MATERIA","FINAL","RECURSAN","RECUPERAN","ABANDONARON";
    line=1
    for( materia in materias){
        printf "%4d |%10s |%8d |%10d |%10d |%12d \n",line,materia,punto1[materia],punto2[materia],punto3[materia],punto4[materia];
        line++;
    }
}
'

