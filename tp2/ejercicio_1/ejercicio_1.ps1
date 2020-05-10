Param(
    [Parameter(Position = 1, Mandatory = $false)]
    [String] $pathSalida = ".\procesos.txt ",
    [int] $cantidad = 3
)
$existe = Test-Path $pathSalida
if($existe -eq $true){
    $listaproceso = Get-Process
    foreach ($proceso in $listaproceso) {
        $proceso | Format-List -Property Id,Name >> $pathSalida
    }
    for ($i = 0; $i -lt $cantidad; $i++) {
        Write-Host $listaproceso[$i].Name - $listaproceso[$i].Id
    }
}
else{
    Write-Host "El path no existe"
}

<#
Respuestas:
1) El objetivo de este script es guardar en un archivo el nombre y 
id de todos los procesos que se están ejecutando en el sistema, e
imprimir en pantalla los N primeros procesos.
2) Le agregaría la posición 2 al parámetro cantidad.
3) Si el script es ejecutado sin enviar ningún parámetro este se
ejecuta usando los valores predeterminados dentro del mismo, siendo
estos pathSalida = ".\procesos.txt" y cantidad = 3.
#>