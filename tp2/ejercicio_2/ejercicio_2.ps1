#!/usr/bin/pwsh
# ejercicio_2.ps1 | Trabajo Práctico 2) Ejercicio 2) | Segunda reentrega
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
        $diaInicial = $vecLlamada[$_.Usuario].Split(" ")[0]
        $vecLlamada[$_.Usuario] = New-TimeSpan -Start $vecLlamada[$_.Usuario] -End $_.FechaHora 
        if ($vecLlamada[$_.Usuario] -lt 0) {
            $vecLlamada[$_.Usuario] = $vecLlamada[$_.Usuario].Negate()
        }
        #Punto 1
        $totalTiempo[$diaInicial] = $totalTiempo[$diaInicial] + $vecLlamada[$_.Usuario]
        $cantLlamadas[$diaInicial]++
        
        #Punto 2
        $tiempoUsuarioDia["$($_.Usuario)/$diaInicial"] = $tiempoUsuarioDia["$($_.Usuario)/$diaInicial"] + $vecLlamada[$_.Usuario]
        $cantUsuarioDia["$($_.Usuario)/$diaInicial"]++

        #Punto 3
        $cantLlamadasUsuario[$_.Usuario]++

        #Punto 4 - Parte 1
        $key = "$($_.Usuario)/$($cantLlamadasUsuario[$_.Usuario])/$diaInicial"
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
$prom = @()
Write-Output "PUNTO 1:"
Write-Output "`nPromedio de tiempo de las llamadas realizadas por día" 
foreach ($dia in $totalTiempo.keys.GetEnumerator() | Sort-Object ){
    $prom_value = '{0:hh\:mm\:ss}' -f $($totalTiempo[$dia]/$cantLlamadas[$dia])
    $item = New-Object System.Object
    $item | Add-Member -MemberType NoteProperty -Name "Día" -Value $dia
    $item | Add-Member -MemberType NoteProperty -Name "Promedio" -Value $prom_value
    $prom += $item
}
$prom | Format-List -Expand EnumOnly

#Punto 2
Write-Output "PUNTO 2:"
Write-Output "`nPromedio de tiempo y cantidad de llamadas por usuario y día" 
$usuarios_dia = @()
foreach ($usuarioDia in $tiempoUsuarioDia.keys.GetEnumerator() | Sort-Object){
    $usuarioDia_list = $usuarioDia.Split("/")
    $item = New-Object System.Object
    $item | Add-Member -MemberType NoteProperty -Name "Usuario" -Value $usuarioDia_list[0]
    $item | Add-Member -MemberType NoteProperty -Name "Día" -Value $usuarioDia_list[1]
    $item | Add-Member -MemberType NoteProperty -Name "Cant. llamadas" -Value $cantUsuarioDia[$usuarioDia]
    $item | Add-Member -MemberType NoteProperty -Name "Promedio" -Value $($tiempoUsuarioDia[$usuarioDia]/$cantUsuarioDia[$usuarioDia])
    $usuarios_dia += $item
}
$usuarios_dia | Format-List -Expand EnumOnly

#Punto 3
$flag = 0
$usuarios = @()
Write-Output "PUNTO 3:"
Write-Output "`nLos tres usuarios con más llamadas son:"
$cantLlamadasUsuario = $cantLlamadasUsuario.GetEnumerator() | Sort-Object -Property value -Descending -Top 3
foreach ($usr in $cantLlamadasUsuario){
    $flag++
    $item = New-Object System.Object
    $item | Add-Member -MemberType NoteProperty -Name "Puesto" -Value $flag
    $item | Add-Member -MemberType NoteProperty -Name "Usuario" -Value $usr.name
    $item | Add-Member -MemberType NoteProperty -Name "Cant. llamadas" -Value $usr.value
    $usuarios += $item
}
$usuarios | Format-List -Expand EnumOnly

#Punto 4 - Parte 1
Write-Output "PUNTO 4:"
Write-Output "`nCantidad de llamadas por debajo de la media diaria:"
$llamadas_bajo_media = @()
$callsBelowMedia = @{}
foreach ($dia in $totalTiempo.keys){
    $promedioDia = $totalTiempo[$dia]/$cantLlamadas[$dia]
    foreach ($key in $duracionLlamada.keys){
        $diaLlamada = $key.Split("/")[2]
        if ( $diaLlamada -eq $dia ){
            if( $duracionLlamada[$key] -lt $promedioDia ){
                $callsBelowMedia[$dia]++
            }
        }
    }
    $item = New-Object System.Object
    $item | Add-Member -MemberType NoteProperty -Name "Día" -Value $dia
    $item | Add-Member -MemberType NoteProperty -Name "Promedio" -Value $('{0:hh\:mm\:ss}' -f $promedioDia)
    $item | Add-Member -MemberType NoteProperty -Name "Llamadas debajo de la media" -Value $callsBelowMedia[$dia]
    $llamadas_bajo_media += $item
}
$llamadas_bajo_media | Format-List -Expand EnumOnly

#Punto 4 - Parte 2
$callsBelowMediaUser = @{}
if ($cantLlamadasSemana -ne 0){
    $promedioSem = $duracionLlamadasSemana/$cantLlamadasSemana
}
else{
    $promedioSem = 0
}
foreach ($key in $duracionLlamada.keys){
    $user = $key.Split("/")[0]
    if( $duracionLlamada[$key] -lt $promedioSem ){
        $callsBelowMediaUser[$user]++
    }
}
Write-Output "El usuario con más llamadas con duración por debajo de la media semanal es:"
$user = $callsBelowMediaUser.GetEnumerator() | Sort-Object -Property value -Descending -Top 1
if ($user){
    $item = New-Object System.Object
    $item | Add-Member -MemberType NoteProperty -Name "Usuario" -Value $user.name.Split("/")[0]
    $item | Add-Member -MemberType NoteProperty -Name "Cant. llamadas" -Value $user.value
    $item | Format-List
}

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
