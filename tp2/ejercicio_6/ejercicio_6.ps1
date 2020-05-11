Param(
    [Parameter(Position = 1, Mandatory = $true)]
    [String] $Path
)

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

$existe = Test-Path $Path
if($existe -ne $true){
    Write-Error "El archivo proporcionado no existe!"
    exit 1
}

$lineas = Get-Content $Path | Measure-Object -Line
if($lineas.Lines -gt 1){
    Write-Error "El archivo no debe contenter más de una linea!"
    exit 1
}

$contenido = Get-Content $Path
if($contenido -Match "[a-zA-Z]"){
    Write-Error "El archivo no debe contener letras!"
    exit 1
}
elseif($contenido -eq $null){
    Write-Host "0"
    "0" 1> salida.out
    exit 1
}

$fracciones = (Get-Content $Path).Split(",")

$simples = @()
$mixtas = @()

$re_simple = "^\-?[0-9]+\/\-?[0-9]+$"
$re_mixta = "^\-?[0-9]+\:\-?[0-9]+\/\-?[0-9]+$"

foreach ($item in $fracciones) {
    if($item -Match $re_simple){
        $simples += $item
    }
    elseif($item -Match $re_mixta){
        $mixtas += $item
    }
}

$num = 1
$den = 1

foreach ($item in $simples) {
    $nums = $item -split '/'
    $num = ($num * $nums[1]) + ($den * $nums[0])
    $den = $den * $nums[1] 
}

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

$num = $num - $den

$mcd = mcd $num $den
$num = $num / $mcd
$den = $den / $mcd

if($den -lt 0){
    $num = $num * -1
    $den = $den * -1
}

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