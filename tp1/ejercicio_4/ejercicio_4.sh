#!/bin/bash

# ejercicio_4.sh | Trabajo Práctico 1) Ejercicio4) | Segunda reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

info="Ejecute $0 -h|--help para obtener información acerca de la ejecución de este script"

Help(){
    echo "############################### HELP $0 ###############################"
    echo -e "El script recibe el path de un directorio con archivos de log,"
    echo -e "y genera un .tar.gz por cada empresa del conjunto de archivos de log"
    echo -e "en un directorio de destino proporcionado como parámetro. Si, además,"
    echo -e "se indica una empresa solo se generará un .tar.gz para esa empresa."
    echo -e "Aclaraciones:"
    echo -e "\t-Luego de creados los .tar.gz correspondientes los archivos de log son borrados."
    echo -e "\t-Si ya existiera un .tar.gz para alguna empresa los nuevos archivos se agregarán a este."
    echo -e "\nEjecución: $0 -f {path_archivos_log} -z {path_destino_comprimidos} -e {empresa}"
}

ErrorP(){
    echo "¡Parámetros enviados incorrectos! $info" >&2 ; 
}

ErrorL(){
    echo "La ruta especificada no existe o no se tienen los permisos requeridos. $info"
    exit 1;
}

ErrorD(){
    echo "La ruta especificada no es un directorio. $info"
}

ErrorW(){
    echo "Necesita permisos de escritura sobre los directorios proporcionados. $info"
}

OPTIONS=f:z:e:h
LONGOPTS=help

PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@")
if [ $? != 0 ]
then
    ErrorP
    exit 1;
fi

eval set -- "$PARSED"

SRC_PATH=""
DEST_PATH=""
EMPRESA=""

while true;
do
    case "$1" in
        -h|--help)
            Help
            exit 1;;
        -f)
            SRC_PATH=$2
            shift 2;;
        -z)
            DEST_PATH=$2
            shift 2;;
        -e)
            EMPRESA=$2
            shift 2;;        
        --)
            shift;
            break;;
        *)
            break;;
    esac
done

# Verifico que se haya dado un path de dónde tomar los archivos de log
if [ -z "$SRC_PATH" ]
then
    echo "Es necesario proporcionar un path de donde leer los archivos de log. $info"
    exit 1;
fi

# Verifico que se haya dado un path a dónde enviar los archivos de log comprimidos
if [ -z "$DEST_PATH" ]
then
    echo "Es necesario proporcionar un path donde crear los archivos zip. $info"
    exit 1;
fi

# Verifico que el usuario haya pasado dos path correspondientes a directorios
if ! [[ -d "$SRC_PATH" && -d "$DEST_PATH" ]]
then
    ErrorD
    exit 1;
fi

# Verifico que el usuario tenga permisos de escritura sobre los directorios enviados
# De otra forma no podrá eliminar los logs en el directorio de origen ni crear los 
# comprimidos en el directorio de destino
if ! [[ -w "$SRC_PATH" && -w "$DEST_PATH" ]]
then
    ErrorW
    exit 1;
fi

# Expresión regular para filtrar solo los archivos de log según el formato 'empresa-int.log'
# Si se me pasara por parámetro la empresa la expresión regular limitará solo a los archivos de esa empresa
if [ -z "$EMPRESA" ]
then
    regex_log="[a-zA-Z]+\-[0-9]+\.log$"
else
    regex_log="$EMPRESA\-[0-9]+\.log$"
fi

SRC_PATH=`echo "$SRC_PATH" | sed 's/\/*$//'`
DEST_PATH=`echo "$DEST_PATH" | sed 's/\/*$//'`

declare -A empresas
declare -A mayor_empresas

# Calculo el número de semana más alto entre los archivos válidos
for arch in $(ls "$SRC_PATH/" | grep -E $regex_log)
do
    emp=$(echo $arch | cut -f 1 -d "-")
    numero=$(echo $arch | cut -f 2 -d "-" | cut -f 1 -d ".")
    if [[ ${mayor_empresas[$emp]} ]]
    then
        if [ $numero -gt ${mayor_empresas[$emp]} ]
        then
            mayor_empresas[$emp]=$numero
        fi
    else
        mayor_empresas[$emp]=$numero
    fi
done

# Guardo en este array el nombre de la compañía de cada archivo válido y los archivos de esa empresa
for emp in ${!mayor_empresas[*]}
do
    # Excluyo al archivo con número de semana más alto entre los que voy a comprimir
    archivos=$(ls -d "$SRC_PATH/"* | grep -E "$emp\-[0-9]+\.log$" | grep -vE "[a-zA-Z]+\/$emp\-${mayor_empresas[$emp]}\.log$")
    # Compruebo que archivos tenga algo por el caso de tener un solo log válido
    if [[ ! -z $archivos ]]
    then
        empresas[$emp]=$archivos
    fi
done

# Recorro las empresas
for emp in ${!empresas[*]}
do  
    # Si no tengo un .tar.gz para la empresa dada en el directorio de destino creo uno nuevo
    if [ ! -f "$DEST_PATH/$emp.tar.gz" ]
    then
        tar -czf "$DEST_PATH/$emp.tar.gz" ${empresas[$emp]}
    # Si tengo un .tar.gz para la empresa dada en el directorio de destino añado los archivos a ese .tar.gz
    else
        gunzip "$DEST_PATH/$emp.tar.gz"
        tar -rf "$DEST_PATH/$emp.tar" ${empresas[$emp]}
        gzip "$DEST_PATH/$emp.tar"
    fi
    # Hago un rm de los archivos del directorio de origen solo si fue exitosa la compresión de los archivos
    if [ $? == 0 ]
    then
        echo ${empresas[$emp]} | xargs rm
    fi
done
