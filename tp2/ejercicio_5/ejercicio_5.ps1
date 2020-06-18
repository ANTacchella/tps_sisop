#!/usr/bin/pwsh

# ejercicio_5.ps1 | Trabajo Práctico 2) Ejercicio 5) | Segunda reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332



Param(
 
    [Parameter(Position = 1, mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [string] $Nomina

)

# Fijarse de evaluar permisos
$existe = Test-Path -Path $Nomina -PathType Leaf

if( $existe -ne $True ){
    Write-Error "¡El archivo proporcionado no existe!"
    exit 1
}

$archivo = Import-Csv -Path $Nomina -Delimiter '|'
$punto1 = @{}
$punto2 = @{}
$punto3 = @{}
$punto4 = @{}

$alu_materia = @{}
$materias = @{}

$bandera=0
foreach($item in $archivo){

    if(!$item.Equals("") -and $bandera -ne "1" ){
        
        $dni = $item.DNI
        $idMateria = $item.IdMateria

        $clave= $dni+"_"+$idMateria
        if(!$materias.contains($idMateria)){

            $materias[$idMateria] = $idMateria 
            $punto1[$idMateria] = 0
            $punto2[$idMateria] = 0
            $punto3[$idMateria] = 0
            $punto4[$idMateria] = 0

        }     
    
        if(!$alu_materia.contains($clave)){

            $alu_materia[$clave] = $clave
            
            [float]$p1 =  [float]$item.PrimerParcial
            [float]$p2 =  [float]$item.SegundoParcial
            [float]$recu =  [float]$item.RecuParcial
            [float]$nRecu =  [float]$item.RecuNota
            [float]$final =  [float]$item.Final

            #punto 1
            if( ((4 -le $p1 -and $p1 -le 6 -and $recu -ne "1" ) -or ($recu -eq "1" -and 4 -le $nRecu -and $nRecu -le 6) -or ($recu -eq "1" -and 6 -lt $nRecu -and  4 -le $p2 -and $p2 -le 6)) -and (( 4 -le $p2 -and $p2 -le 6 -and $recu -ne "2") -or ($recu -eq "2" -and 4 -le $nRecu -and $nRecu -le 6) -or ($recu -eq "2" -and 6 -lt $nRecu -and 4 -le $p1 -and $p1 -le 6)) -and $final -eq ""  ){
                $punto1[$idMateria]+=1
               
            }
            else{
                #punto 2
                if( ($final -lt 4 -and $final -ne "") -or ( (($p1 -lt 4 -and $recu -ne "1" ) -or ($recu -eq "1" -and $nRecu -lt 4 )) -and (($p2 -lt 4 -and $recu -ne "2" ) -or ($recu -eq "2" -and $nRecu  -lt 4 )) )){
                    $punto2[$idMateria]+=1
                    
                }else{
                    #punto 3
                    if($nRecu -eq "" -and ( ($p1 -lt 7 -and $p1 -ne "" ) -or ($p2 -lt 7 -and $p2 -ne "") ) ){
                        $punto3[$idMateria]+=1
            
                    }
                    else{
                        #punto 4
                        if( ($p1 -eq "" -and $recu -eq "1" -and $nRecu -eq "") -or ($p1 -eq "" -and $recu -ne "1") -or ($p2 -eq "" -and $recu -eq "2" -and $nRecu -eq "") -or ($p2 -eq "" -and $recu -ne 2) ){
                            $punto4[$idMateria]+=1
                        
                        }

                    }

                }

            }
            
        }
    }
    else{
        if(!$item.Equals("")){
            $bandera=1
        }
    }
 

}


$salida = @()

foreach($elem in $materias.GetEnumerator()){
    $value = $elem.Value
    
    $line = [Ordered]@{
        Materia = $value
        Final = $punto1[$value] 
        Recursan = $punto2[$value]
        Recuperan = $punto3[$value]
        Abandonaron = $punto4[$value]   
    }

    $salida += New-Object PSObject -Property $line
}

$salida | export-csv -Path .\res_materias.txt -NoTypeInformation

<#
.Synopsis
    Este script recibe un archivo de notas de alumnos y lo analiza para indicar por cada materia, cuantos alumnos recursan,cuantos pueden recuperar, cuantos tienen que ir a final y cuantos abandonaron.
.Description
    El script ejercicio_5.ps1 recibe como parámetros el archivo de alumnos para analizar, la primera linea del archivo no se toma encuenta porque se supone que son los titulos de cada columna.
.Example
    ./ejercicio_5.ps1 -Nomina ./alumnos.csv o ./alumnos.txt
.Notes
    Parámetros:
    -Nomina: archivo que se va a usar para sacar las estadisticas por materia.
#>