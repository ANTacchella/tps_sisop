# ejercicio_2.ps1 | Trabajo Práctico 2) Ejercicio 2) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332



Param(
    <# Defino las condiciones que tiene que cumplir el parametro,
    en este caso el 
   parametro $file es obrigatorio(mandatory=$true)
   y sera el primer parametro de los que se
   mandan por consola(position=1). el parametro file 
   se lo puedo manda simplemente haciendo 
   mi_programa mi_file o haciendo
   mi_programa -file'mi_file',
   tambien valido que el parametro no sea nulo o 
   vacio.
   tambien valido que el parametro tenga una terminacion
   del tipo .log
    #>

    [Parameter(Position = 1, mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    #[ValidatePattern('^*.log')]
    [string] $file

)


[string] $info = "para mas informacion ejecute $0 -h"
#buscar como seria el $0 de bash en powershell

function Error_1() {

    write-ouput "la ruta especificada no existe o no se tiene los permisos requeridos, $info " 
    exit 1

}