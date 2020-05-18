#!/usr/bin/pwsh
# ejercicio_2.ps1 | Trabajo Práctico 2) Ejercicio 2) | Primera entrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

Param(

    [Parameter(mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [string] $Path

)

# Fijarse de evaluar permisos
$existe = Test-Path -Path $Path -PathType Leaf

if( $existe -ne $True ){
    Write-Error "¡El archivo proporcionado no existe!"
    exit 1
}

$file_data = Import-Csv -Path $Path -Delimiter '_' -Header 'FechaHora','Usuario'
$file_data = $file_data | Sort-Object -Property FechaHora

#Declaraciones
$vecLlamada = @{}
$totalTiempo = @{}
$cantLlamadas = @{}
$tiempoUsuarioDia = @{}
$cantUsuarioDia = @{}
$cantLlamadasUsuario = @{}
$duracionLlamada = @{}
$cantLlamadasSemana = 0
$duracionLlamadasSemana = 0

$file_data | ForEach-Object {
    if ( $vecLlamada.Contains($_.Usuario) ){
        $diaInicial = $vecLlamada[$_.Usuario].Split(" ")[0].Split("-")[2]
        $vecLlamada[$_.Usuario] = New-TimeSpan -Start $vecLlamada[$_.Usuario] -End $_.FechaHora 
        
        #Punto 1
        $totalTiempo[$diaInicial] = $totalTiempo[$diaInicial] + $vecLlamada[$_.Usuario]
        $cantLlamadas[$diaInicial]++
        
        #Punto 2
        $tiempoUsuarioDia["$($_.Usuario)-$diaInicial"] = $tiempoUsuarioDia["$($_.Usuario)-$diaInicial"] + $vecLlamada[$_.Usuario]
        $cantUsuarioDia["$($_.Usuario)-$diaInicial"]++

        #Punto 3
        $cantLlamadasUsuario[$_.Usuario]++

        #Punto 4 - Parte 1
        $key = "$($_.Usuario)-$($cantLlamadasUsuario[$_.Usuario])-$diaInicial"
        $duracionLlamada[$key] = $vecLlamada[$_.Usuario]

        #Punto 4 - Parte 2
        $cantLlamadasSemana++
        $duracionLlamadasSemana = $duracionLlamadasSemana + $vecLlamada[$_.Usuario];


        $vecLlamada.Remove($_.Usuario)
    }
    else{
        $vecLlamada[$_.Usuario] = $_.FechaHora
    }

}

 #Punto 1
 Write-Output "`nPUNTO 1:"
 Write-Output "`Promedio de tiempo de las llamadas realizadas por día`n" 
 foreach ($dia in $totalTiempo.keys.GetEnumerator() | Sort-Object ){
     $prom = $totalTiempo[$dia]/$cantLlamadas[$dia]
     $prom = '{0:hh\:mm\:ss}' -f $prom
    "`t$($dia): `t$($prom)"| Format-List  
    #Write-Output "`tPromedio de tiempo del dia $dia son: $($totalTiempo[$dia]/$cantLlamadas[$dia])"
 }

 #Punto 2
 Write-Output "`nPUNTO 2:"
 Write-Output "Promedio de tiempo y cantidad por usuario por día`n" 
 foreach ($usuarioDia in $tiempoUsuarioDia.keys.GetEnumerator() | Sort-Object){
    $usuarioDia_list = $usuarioDia.Split("-")
    "`tUsuario: $($usuarioDia_list[0]) Dia: $($usuarioDia_list[1]) Llamadas: $($cantUsuarioDia[$usuarioDia]) Promedio de Tiempo: $($tiempoUsuarioDia[$usuarioDia]/$cantUsuarioDia[$usuarioDia])" | Format-List -GroupBy $usuarioDia_list[0]
  # Write-Output "`tLa cantidad de llamadas del dia $($usuarioDia_list[1]) del usuario $($usuarioDia_list[0]) son: $($cantUsuarioDia[$usuarioDia])"
   # Write-Output "`tY su promedio de tiempo es $($tiempoUsuarioDia[$usuarioDia]/$cantUsuarioDia[$usuarioDia])`n"
}

#Punto 3
$flag = 0
Write-Output "`nPUNTO 3:"
Write-Output "Los tres usuarios con más llamadas son:`n"
foreach ($item in $cantLlamadasUsuario.GetEnumerator() | Sort-Object -Property value -Descending){
    if ( $flag -lt 3 ){
        $flag++
        Write-Output "`t$($flag)-El usuario $($item.name) con $($item.value)"
    }
    else{
        break
    }
}

#Punto 4 - Parte 1
Write-Output "`nPUNTO 4:"
Write-Output "Cantidad de llamadas por debajo de la media diaria:`n"
$callsBelowMedia = @{}
foreach ($dia in $totalTiempo.keys){
    $promedioDia = $totalTiempo[$dia]/$cantLlamadas[$dia]
    foreach ($key in $duracionLlamada.keys){
        $diaLlamada = $key.Split("-")[2]
        if ( $diaLlamada -eq $dia ){
            if( $duracionLlamada[$key] -lt $promedioDia ){
                $callsBelowMedia[$dia]++
            }
        }
    }
    Write-Output "`tPara el dia $dia fueron $($callsBelowMedia[$dia]) llamadas por debajo del promedio del día."
}

#Punto 4 - Parte 2
$callsBelowMediaUser = @{}
$promedioSem = $duracionLlamadasSemana/$cantLlamadasSemana
foreach ($key in $duracionLlamada.keys){
    $user = $key.Split("-")[0]
    if( $duracionLlamada[$key] -lt $promedioSem ){
        $callsBelowMediaUser[$user]++
    }
}
$user = $callsBelowMediaUser.GetEnumerator() | Sort-Object -Property value -Descending -Top 1
Write-Output "`nEl usuario con más llamadas con duración por debajo de la media semanal es: $($user.name) con $($user.value)`n`n"

<#
.Synopsis
    Este script lee el log proporcionado a traves de un path enviado y realiza distintas operaciones.
.Description
    El script ejercicio_2.ps1 recibe como parametro un el path de un log.
    Se pide obtienen y muestran por pantalla los siguientes datos uno debajo del otro:
        ▪ Promedio de tiempo de las llamadas realizadas por día.
        ▪ Promedio de tiempo y cantidad por usuario por día.
        ▪ Los 3 usuarios con más llamadas en la semana.
        ▪ Cuántas llamadas no superan la media de tiempo por día y el usuario que tiene más llamadas por debajo de la media en la semana.
    Parámetros:
        • -Path: Directorio en el que se encuentran los archivos de log. Puede ser una ruta relativa o absoluta.
    Formato del registro de llamada (log):
        • Fecha y Hora: Formato YYYY-MM-DD HH:mm:ss
        • Usuario que realiza la llamada.
        • Separador: “ _ “ (espacio, guión bajo, espacio)
.Example
    ./ejercicio_2.ps1 -Path logs/telefonica-1.log
#>
