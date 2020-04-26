#!/bin/bash

# ej2.sh | Trabajo Práctico 1) Ejercicio 2) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332


help() {
    echo "#### HELP $0 ####"
    echo -e "\nEl script recibe el path de un log para realizar las siguientes operaciones:\n"
    echo -e "\t1.Promedio de tiempo de las llamadas realizadas por día."
    echo -e "\t2.Promedio de tiempo y cantidad por usuario por día."
    echo -e "\t3.Los 3 usuarios con más llamadas en la semana."
    echo -e "\t4.Cuántas llamadas no superan la media de tiempo por día y el "
    echo -e "\tusuario  que  tiene  más llamadas por debajo de la media en la semana."
    echo -e "\nEl unico parametro recibido es el path del log. La sintaxis es: $0 path_del_archivo."
}


if [ "$1" = "--help" ]; then
    help
elif [ "$1" = "-?" ]; then
    help
elif [ "$1" = "-h" ]; then
    help
fi

#codigo awk                

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
        if test -r $arch && ! test -d $arch
            then cat $arch >> .archOrdenado.txt
        elif ! test -r $arch;
            then echo "no tiene permiso de lectura sobre el archivo $arch"
        fi
done    
IFS=$'\n'
archivosCreados[".archOrdenado.txt"]=".archOrdenado.txt"
chmod 777 .archOrdenado.txt


#ordeno los datos para mandarlos ar alrchivo .archOrdendo.txt y creo el archivo .usuarios.txt que contiene los usuarios
sort -t "_" -k 1 .archOrdenado.txt > .archOrdenado1.txt #lo envio a otro archivo porque si lo enviava al mismo archivo rompia
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

declare -A llamadasPorDia

for l in ${lines[*]}
do
        usr=`echo $l | awk -F" _ " ' {print  $2}' ` # me fijo en el usuario de la linea
    
    #si para el usuario de la linea no hay una llamada iniciada, entonces guardo cuando se inicio la llamada y la linea
    #la mado al archivo de la fecha de inicio de la llamada, en caso de que la llamada este iniciada, mando la linea al archivo de la fecha de inicio
    #y marco que no hay llamadas activas
    if [ -z  "${usuarios[$usr]}" ]; then
        usuarios[$usr]=`echo $l | awk -F" _ " '{print  strftime("%Y-%m-%d",$3) } '`
        t="${usuarios[$usr]}"
    else
        t="${usuarios[$usr]}"
        usuarios[$usr]=''
    fi

    nombre_arch=".$t.txt"
    archivosCreados[$nombre_arch]=$nombre_arch
    llamadasPorDia[$nombre_arch]=$nombre_arch
    echo $l >> $nombre_arch

done
#la parte del codigo de arriba me termina generando N archivos dependiendo los N dias que se iniciaron llamadas(los fines de llamadas se cuentan para el dia del inicio de la misma)
#los N archivos estan ordenados individualmente por fecha hora y usuario

for arch in ${llamadasPorDia[*]}
do
cat $arch | awk -F"_" '
BEGIN{
    totalHoy=0;
    cantLlamadasHoy=0;
    dia=1999-01-01;
}                                                                                            
{   
    if( NR==1){
        dia=strftime("%Y-%m-%d",$3)
    }
    hora=$3;
    if( $2 in tiempoLlamadas ) {
        
        tiempoLlamadas[$2]=hora - tiempoLlamadas[$2];
        
        #punto 1
        totalHoy=totalHoy + tiempoLlamadas[$2];
        cantLlamadasHoy++;
        
        #punto 2
        cantUserHoy[$2]++;
        tiempoPorUsuario[$2]+= tiempoLlamadas[$2];
        
        #punto 4.1
        key=$2"-"cantUserHoy[$2];
        duracionLlamada[key]=tiempoLlamadas[$2];
#        printf "\nUsr-Llamada-Duracion= %s--%s\n", key, duracionLlamada[key];        
#        printf "\nusuario %s--tiempo %s\n",$2,tiempoLlamadas[$2];
        
        #punto 4.2
        #key2=$2"|"cantUserHoy[$2]"|"dia; 
        #duracionLlamadaSemanal[key2]=tiempoLlamadas[$2];
        #printf "\nUsr|Llamada|dia==Duracion= %s==%s\n", key2, duracionLlamadaSemanal[key2];
        printf "%s|%d|%s|%s\n",$2,cantUserHoy[$2],dia,tiempoLlamadas[$2] >> ".Punto_4b.txt"
        
        delete tiempoLlamadas[$2];
    }
    else{
        tiempoLlamadas[$2]=hora;
    }
}
END{
    callsBelowMedia=0;
       
    #punto 1
    promedio=totalHoy/cantLlamadasHoy
    printf "%10s |%20d:%02d:%02d \n",dia,(promedio/60)/60,(promedio/60)%60,promedio%60  >> ".Punto_1.txt"
    
    #punto 2
    for( usr in cantUserHoy )
    {
        usrPromedio=tiempoPorUsuario[usr]/cantUserHoy[usr]
        printf "%10s |%40s |%13d |%4d:%02d:%02d \n",dia,usr,cantUserHoy[usr],(usrPromedio/60)/60,(usrPromedio/60)%60,usrPromedio%60 >> ".Punto_2.txt"
    
        #Punto 3
        printf "%40s |%13d \n",usr,cantUserHoy[usr] >> ".Punto_3.txt"
    }

    #Punto 4    
    for( key in duracionLlamada )
    {
        if( duracionLlamada[key] < promedio ) {
            callsBelowMedia++;
    #        printf "\ncallsnow  =  %d",callsBelowMedia;
        }
    #    printf "\nUsr-Llamada-Duracion= %s--%s\n", key, duracionLlamada[key];

    }
#    printf "\nCantidad Llamadas Por Debajo de la Media = %d", callsBelowMedia; 
    printf "%s |%d\n",dia,callsBelowMedia >> ".Punto_4.txt"
   
   
}'

done

chmod 777 .Punto_1.txt
archivosCreados[".Punto_1.txt"]=".Punto_1.txt"
#-----------------PUNTO-1--------------------"
printf "\nPROMEDIO DEL TIEMPO DE LLAMADA POR DIA"
cat .Punto_1.txt | awk 'BEGIN{printf "\n%4s |%10s |%s \n","LINE","DIA","TIEMPO DE LLAMADA PROMEDIO ";}{printf "%4d |%s \n",NR,$0}'

chmod 777 .Punto_2.txt
archivosCreados[".Punto_2.txt"]=".Punto_2.txt"
#-----------------PUNTO-2--------------------"
printf "\nPROMEDIO DE TIEMPO Y CANTIDAD DE LLAMADAS POR DIA POR USUARIO"
cat .Punto_2.txt | awk 'BEGIN{printf "\n%4s |%10s |%40s |%13s |%10s\n","LINE","DIA","USUARIO","CANT.LLAMADAS","TIEMPO";}{printf "%4d |%s \n",NR,$0}'

chmod 777 .Punto_3.txt
archivosCreados[".Punto_3.txt"]=".Punto_3.txt"
#-----------------PUNTO-3--------------------"
printf "\nUSUARIOS CON MAS CANTIDAD DE LLAMADAS EN LA SEMANA"
#cat .Punto_3.txt | sort -t "|" -k 2 -r | awk -F"|" '
cat .Punto_3.txt | awk -F"|" '
BEGIN{
    cont=0;
    printf "\n%4s |%41s |%13s \n","LINE","USUARIO","CANT.LLAMADAS";
}
{
    if( $1 in cantCallsSem)
        cantCallsSem[$1]+=$2;
    else
        cantCallsSem[$1]=$2;
}
END{
    cont=1;
    PROCINFO["sorted_in"] = "@val_num_desc"
    for(name in cantCallsSem){
        if( cont<=3 ){
            printf "%4s |%40s |%13s \n",cont,name,cantCallsSem[name];  
            cont++;
        }
    }           
}'

chmod 777 .Punto_4.txt
archivosCreados[".Punto_4.txt"]=".Punto_4.txt"
#-----------------PUNTO-4--------------------"
printf "\nCANTIDAD DE LLAMADAS POR DEBAJO DE LA MEDIA"; 
cat .Punto_4.txt | awk -F"|" 'BEGIN{printf "\n%4s |%11s |%13s\n","LINE","DIA","CANT.LLAMADAS";}{printf "%4d |%11s |%13s \n",NR,$1,$2}'

chmod 777 .Punto_4b.txt
archivosCreados[".Punto_4b.txt"]=".Punto_4b.txt"
#-----------------PUNTO-4b--------------------"
#printf "\nUSER CON MAS LLAMADAS POR ABAJO DE LA MEDIA SEMANAL"; 
cat .Punto_4b.txt | awk -F"|" '
BEGIN{
    suma=0;
    cont=0;
    promedioSem=0.0;
}
{
    suma+=$4;
    cont++;

    arrayUsers[$1"|"$2"|"$3]=$4;
}
END{
    promedioSem=suma/cont;
#    printf "\nPromedioSemanal = %s\n", promedioSem;
    
    for( usr in arrayUsers )
    {
        split(usr,t,"|")
#        printf "...........t1 = %s - t2 = %s - t3 = %s\n",t[1],t[2],t[3];
        if( arrayUsers[usr] < promedioSem ) {
            cantCallUser[t[1]]++;
#            printf "user=%s ___ cant=%d\n",t[1],cantCallUser[t[1]];
        }
    }
    printf "\nUSUARIO CON MAS LLAMADAS POR ABAJO DE LA MEDIA EN LA SEMANA"; 
    printf "\n%4s |%30s |%13s \n","LINE","USUARIO","CANT.LLAMADAS";

    cont=1;
    PROCINFO["sorted_in"] = "@val_num_desc"
    for(name in cantCallUser){
        if( cont==1 ){
            printf "%4d |%30s |%13s \n",cont,name,cantCallUser[name];  
            cont++;
        }
    }
}'

#elimino los archivos auxiliares que cree
for j in ${archivosCreados[*]}
do
        rm $j
done        
