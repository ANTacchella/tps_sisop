#!/bin/bash 

# ejercicio_1.sh | Trabajo Práctico 1) Ejercicio1) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

ErrorS()  {  
 
	echo "Error. La sintaxis del script es la siguiente:"    
	echo "El comando para saber la cantidad de líneas del archivo es: $0 nombre_archivo L."    
	echo "El comando para saber el número de caracteres del archivo es: $0 nombre_archivo C." 
	echo "El comando para saber la longitud de la línea más larga del archivo es: $0 nombre_archivo M." 

}  
 
ErrorP()  {
	echo "Error. $1 no existe o no tiene permiso de lectura."
}  
if test $# -lt 2; then  
	ErrorS  
fi  
if ! test -r $1; then    
	ErrorP  
elif test -f $1 && (test $2 = "L" || test $2 = "C" || test $2 = "M"); then    
	if test $2 = "L"; then      
		res=`wc –l $1`  
		echo "La cantidad de líneas del archivo es: $res." 
	elif test $2 = "C"; then      
		res=`wc –m $1`  
		echo "El número de caracteres del archivo es: $res." 
	elif test $2 = "M"; then      
		res=`wc –L $1`  
		echo "La longitud de la línea más larga del archivo es: $res." 
	fi  
else    
	ErrorS  
fi   

#RESPUESTAS:  

#a)	¿Cuál es el objetivo de este script?  
#El objetivo del script es mostrar por pantalla el número de líneas, el número de caracteres o el número de caracteres de la línea más larga del archivo que se le mande (si se tiene permiso de lectura, sino devuelve el error correspondiente), dependiendo del segundo parámetro ingresado.

#b)	¿Qué parámetros recibe?  
#Recibe dos parámetros:
#-El primero es el archivo sobre el que se quiere consultar.
#-El segundo parámetro indica que es lo que se quiere consultar del archivo:
#	L = número de líneas.
#	C = número de caracteres.
#	M = número de caracteres de la línea más larga.

#c)	Comentar el código según la funcionalidad (no describa los comandos, indique la lógica)  
#Verifica la cantidad de parámetros, y si no cumple con tener mínimo dos, tira ErrorS.
#Verifica la existencia del archivo indicado y que tenga los permisos de lectura, en caso de no tenerlos arroja ErrorP.
#Verifica la existencia del archivo indicado, si es un archivo regular y si el segundo parámetro es L, C o M. En el caso verdadero, imprime el dato solicitado que corresponda según el segundo parámetro ingresado, en el caso falso, lanza el ErrorS.

#d)	Completar los “echo” con el mensaje correspondiente.  
#Realizado en el script.

#e)	¿Qué información brinda la variable “$#”? ¿Qué otras variables similares conocen? Explíquelas.
# $#:	Número de parámetros pasados al script o función.
# $* / $@:	Parámetros de la función / script (argumentos).
#	"$*" es lo mismo que "$1 $2 ..." 1
#	"$@" es lo mismo que "$1" "$2" ...
# $!	:	El Pid del último proceso hijo ejecutado en segundo plano.
# $$:	Pid del proceso actual.
# $?:	Valor de ejecución del último comando.
# $n:	Parámetros enviados, donde n = 1, 2, 3, ..., 9.
# ${n}	Parámetros enviados (igual que arriba), pero n puede ser mayor que 9.
# $0	En los scripts, ruta con la que se invocó el script.
# $_	Último parámetro del último comando.
# $IFS	Separador de campos.
# $PATH	Variable de entorno con los path a los ejecutables del sistema.

#f)	Explique las diferencias entre los distintos tipos de comillas que se pueden utilizar en Shell scripts.  
#Comillas Dobles ("): Se utilizan para definir textos y "se expanden". Es decir, las variables dentro de las comillas dobles son interpretadas (y no se muestran como el nombre de la variable).
#Comillas Simples ('): Se utilizan para definir textos y "no se expanden", el contenido se interpreta de forma literal. Es decir, las variables dentro de las comillas simples se muestran como el nombre de la variable (y no se muestran como su valor).
#Acento Grave (`): Se utilizan para indicar a bash que interprete el comando que hay entre los acentos, por ejemplo, permite usar la salida de una orden para asignarla a una variable.
