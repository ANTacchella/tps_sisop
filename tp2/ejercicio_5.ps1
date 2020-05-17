# ejercicio_5.ps1 | Trabajo Práctico 2) Ejercicio 5) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332



Param(
    <# con Parameter() defino lascondiciones que 
   tiene que cumplr el parametro,en este caso el 
   parametro $file es obrigatorio(mandatory=$true)
   y sera el primer parametro de los que se
   mandan por consola(position=1). el parametro file 
   se lo puedo manda simplemente haciendo 
   mi_programa mi_file o haciendo
   mi_programa -file'mi_file',
   tambien valido que el parametro no sea nulo o 
   vacio.
    
    #>
    [Parameter(Position = 1, mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [string] $file

)


[string] $info = "para mas informacion ejecute $0 -h"
#buscar como seria el $0 de bash en powershell

function Error_1(){

 write-ouput "la ruta especificada no existe o no se tiene los permisos requeridos, $info " 
 exit 1

}

$archivo = get-content $file 
$punto1 = @{}
$punto2 = @{}
$punto3 = @{}
$punto4 = @{}

$alu_materia = @{}
$materias = @{}

$bandera=0
foreach($item in $archivo){

    if(!$item.Equals("") -and $bandera -ne "1" ){

        $linea = $item.Split("|")
        $clave= $linea[0]+"_"+$linea[1]
        $materias[$linea[1]] = $linea[1]      
    
        if($clave -notin $alu_materia){

            $alu_materia[$clave] = $clave
            
            #punto 1
            if( ((4 -le $linea[2] -and $linea[2] -le 6 -and $linea[4] -ne "1" ) -or ($linea[4] -eq "1" -and 4 -le $linea[5] -and $linea[5] -le 6) -or ($linea[4] -eq "1" -and 6 -lt $linea[5] -and  4 -le $linea[3] -and $linea[3] -le 6)) -and (( 4 -le $linea[3] -and $linea[3] -le  6 -and $linea[4] -ne "2") -or ($linea[4] -eq "2" -and 4 -le $linea[5] -and $linea[5] -le 6) -or ($linea[4] -eq "2" -and 6 -lt $linea[5] -and 4 -le $linea[2] -and $linea[2] -le 6)) -and $linea[6] -eq ""  ){
                $punto1[$linea[1]]+=1
               
            }
            #punto 2
            if( ($linea[6]  -lt 4 -and  $linea[6]  -ne  "") -or (($linea[2]  -lt  4 -and $linea[4]  -ne  "1" ) -or ($linea[4] -eq "1" -and $linea[5]  -lt  4 )) -or (($linea[3]  -lt  4 -and $linea[4]  -ne  "2" ) -or ($linea[4] -eq "2" -and $linea[5]  -lt  4 )) ){
                $punto2[$linea[1]]+=1
               
            }
            #punto 3
            if($linea[5] -eq "" -and ( ($linea[2] -lt 7 -and $linea[2] -ne "" ) -or ($linea[3] -lt 7 -and $linea[3] -ne "") ) ){
                $punto3[$linea[1]]+=1
       
            }
            #punto 4
            if( ($linea[2] -eq "" -and $linea[4] -eq "1" -and $linea[5] -eq "") -or ($linea[2] -eq "" -and $linea[4] -ne "1") -or ($linea[3] -eq "" -and $linea[4] -eq "2" -and $linea[5] -eq "") -or ($linea[3] -eq "" -and $linea[4] -ne 2) ){
                $punto4[$linea[1]]+=1
               
            }

        }
    }
    $bandera=1

}

foreach($elem in $materias.GetEnumerator()){
    $value = $elem.Value
    write-output "materia $value"
    write-output $punto1[$value] 
    write-output $punto2[$value] 
    write-output $punto3[$value] 
    write-output $punto4[$value] 

}
