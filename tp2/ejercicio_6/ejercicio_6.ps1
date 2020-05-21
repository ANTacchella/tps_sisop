#!/usr/bin/pwsh

# ejercicio_6.ps1 | Trabajo Práctico 2) Ejercicio 6) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

Param(
    [Parameter(Position = 1, Mandatory = $true)]
    [String] $Path
)

# Función para calcular em Máximo Común Divisor
function mcd(){
    Param(
        [Parameter(Position = 1, Mandatory = $true)]
        [int] $x,
        [Parameter(Position = 2, Mandatory = $true)]
        [int] $y
    )

    if($y -eq 0){
        $x
    }else{
        mcd $y $($x%$y)
    }
}

#Chequeo que exista el archivo pasado pro parámetro
$existe = Test-Path $Path
if($existe -ne $true){
    Write-Error "El archivo proporcionado no existe!"
    exit 1
}

#Chequeo que el archivo no contenga más de una línea
$lineas = Get-Content $Path | Measure-Object -Line
if($lineas.Lines -gt 1){
    Write-Error "El archivo no debe contenter más de una linea!"
    exit 1
}

#Chequeo que el archivo no contenga letras
$contenido = Get-Content $Path
if($contenido -Match "[a-zA-Z]"){
    Write-Error "El archivo no debe contener letras!"
    exit 1
}
#Caso de archivo vacío, retorno Cero
elseif($null -eq $contenido){
    Write-Host "0"
    "0" 1> salida.out
    exit 1
}

#Lleno el array con las fracciones contenidas por el archivo
$fracciones = (Get-Content $Path).Split(",")

$simples = @() #Array de fracciones simples
$mixtas = @()  #Array de fracciones mixtas

$re_simple = "^\-?[0-9]+\/\-?[0-9]+$" #Regex de fracciones simples
$re_mixta = "^\-?[0-9]+\:\-?[0-9]+\/\-?[0-9]+$" #Regex de fracciones mixtas

#Recorro todas las fracciones y las separo entre simples o mixtas
foreach ($item in $fracciones) {
    if($item -Match $re_simple){
        $simples += $item
    }
    elseif($item -Match $re_mixta){
        $mixtas += $item
    }
}

#Inicializo el resultado en 1
$num = 1
$den = 1

#Realizo la suma de las fracciones simples
foreach ($item in $simples) {
    $nums = $item -split '/'
    $num = ($num * $nums[1]) + ($den * $nums[0])
    $den = $den * $nums[1] 
}

#Realizo la suma de las fracciones mixtas
foreach ($item in $mixtas) {
    $nums = $item -split '[:/]'
    $num = $num + $den * $nums[0]

    if($nums[0] -lt 0){
        $num = ($num * $nums[2]) - ($den * $nums[1])    
    }else{
        $num = ($num * $nums[2]) + ($den * $nums[1])
    }

    $den = $den * $nums[2]
}

#Le resto el 1 sobrante al resultado
$num = $num - $den

#Simplifico el resultado
$mcd = mcd $num $den
$num = $num / $mcd
$den = $den / $mcd

#Invierto los signos si el denominador es negativo
if($den -lt 0){
    $num = $num * -1
    $den = $den * -1
}

#Muestro el resultado por pantalla y lo imprimo en el archivo salida.out
if($den -eq 1 || $num -eq 0){
    $num > salida.out
    $num
}else{
    "$num/$den" > salida.out
    "$num/$den"
}

<#
.Synopsis
    Realiza la suma de las fracciones contenidas en el archivo que se envía como parámetro.
.Description
    El script ejercicio_6.ps1 recibe como parámetro un archivo que contiene una serie de fracciones separas por coma, las cuales pueden ser simples o mixtas y positivas o negativas, y realiza la suma de todas esas fracciones imprimiendo el resultado en pantalla y en el archivo "salida.out" ubicado en el mismo directorio del script.
.Example
    Formato del archivo de fracciones: una sola línea con las fracciones separadas por coma
    Ej: fracciones.txt = {
        16/8,8/9,7/5,9/2,1:5/8
    }

    Ejecución del script:

    ./ejercicio_6.ps1 -Path fracciones.txt
    
.Notes
    Parámetros:
    -Path: Ruta del archivo de entrada, ya sea de manera relativa o absoluta

    El archivo de fracciones no debe contener más de una línea con las fracciones, ni letras.
#>