#!/bin/bash

# EJEMPLO BÁSICO DE USO DE GETOPT PARA PARSING DE PARÁMETROS
# Info 'getopt'
# https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash/14203146#14203146
# https://stackoverflow.com/questions/402377/using-getopts-to-process-long-and-short-command-line-options
# Documentación oficial: https://linux.die.net/man/1/getopt
OPTIONS=h
LONGOPTS=help

# '--options' define los paramétros de nombre corto que se le envían al script
# '--longoptions' define los paramétros de nombre largo que se le envían al script
# '--name' define cuál de los párametros enviados corresponde al nombre del script
# Con '-- "$@"' pasamos todos los parametros para que después puedan ser evaluados con 'eval set --'
PARSED=$(getopt --options=$OPTIONS --longoptions=$LONGOPTS --name "$0" -- "$@")
if [ $? != 0 ];
then
    echo "¡Parámetros enviados incorrectos! Ejecute $0 -h|-?|--help para obtener información acerca de la ejecución de este script" >&2 ; 
    exit 1 ;
fi
 
# Utilizamos 'eval set --' para asignar cada uno de los paramétros después de '--' a los parámetros de bash del $1 al $n
# Info sobre 'eval': https://unix.stackexchange.com/questions/23111/what-is-the-eval-command-in-bash
# Infor sobre 'eval set --': https://www.linuxjournal.com/content/bash-preserving-whitespace-using-set-and-eval
eval set -- "$PARSED"
 
 
# Recorremos las opciones con 'case' y 'shift' para analizar de un argumento a la vez.
# $1 identifica el primer argumento, y cuando usamos 'shift' descartamos el primer argumento, 
# entonces $2 se vuelve $1 entra de nuevo al 'case'.
while true;
do
    case "$1" in
        -h|--help)
            # Acá iría la información acerca de los parámetros admitidos
            echo "Info"
            shift;;
        --)
            shift
            break;;
        *)
            break;;
    esac
done
