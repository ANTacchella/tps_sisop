#!/usr/bin/pwsh
# ejercicio_4.ps1 | Trabajo Práctico 2) Ejercicio 4) | Segunda reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

Param(
    #Validacion de Parametros: Directorio y DirectorioZip tienen que ser enviados obligatoriamente y no ser vacios.
    #Empresa en cambio puede no estar o ser vacia. Si es vacia se toma como que no estuviese.

    [Parameter( mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [string] $Directorio,

    [Parameter( mandatory = $True)]
    [ValidateNotNullOrEmpty()]
    [string] $DirectorioZip,

    [Parameter( mandatory = $False)]
    [ValidateNotNullOrEmpty()]
    [string] $Empresa

)

$existe1 = Test-Path -Path $Directorio -PathType container
$existe2 = Test-Path -Path $DirectorioZip -PathType container

if( $existe1 -ne $True ){
    Write-Error "¡El Directorio proporcionado no existe!"
    exit 1
}
if( $existe2 -ne $True ){
    Write-Error "¡El DirectorioZip proporcionado no existe!"
    exit 1
}

if( !$Empresa ){
    $regex_log='[a-zA-Z]+\-[0-9]+\.log$'
}
else{
    $regex_log="$Empresa\-[0-9]+\.log$"
}

$arch_validos = Get-ChildItem $Directorio | Where-Object Name -Match $regex_log | Select-Object -ExpandProperty "Name"

$empresas = @{}
foreach ($item in $arch_validos) {
    $nombre_emp = $item.Split('-')[0]
    $num_emp = [int]$item.Split('-')[1].Split('.')[0]
    if ( ! $empresas.Contains( $nombre_emp )) {
        $empresas.Add( $nombre_emp, $num_emp)
    } 
    if ( $num_emp -gt $empresas[$nombre_emp] )
    {
        $empresas[$nombre_emp] = $num_emp
    }
}

# Recorro las empresas
foreach ($emp in $empresas.keys)
{
    #Comprimo los archivos, agrego la opcion -Update para que si llega a existir ya el archivo.zip, no lo borre sino que lo actualize
    Get-ChildItem $Directorio | Where-Object {($_.Name -notlike "$emp-$($empresas[$emp]).log" -and $_.Name -Match "$emp\-[0-9]+\.log$") } | Compress-Archive -Update -DestinationPath "$DirectorioZip/$emp.zip"
    # Hago un rm de los archivos del directorio de origen solo si fue exitosa la compresión de los archivos
    if( $? ) {
        Get-ChildItem $Directorio | Where-Object {($_.Name -notlike "$emp-$($empresas[$emp]).log" -and $_.Name -Match "$emp\-[0-9]+\.log$") } | Remove-Item 
    }
}


<#
.Synopsis
    Este script comprime los logs en el directorio que se envía como parámetro, en un directorio especificado.
.Description
    El script ejercicio_4.ps1 recibe como parámetros un directorio, en el cual se encuentran archivos de log, 
    un directorio adonde se comprimiran y, opcionalmente el nombre de la empresa de la cual se buscaran sus logs.
.Example
    ./ejercicio_4.ps1 -Directorio logs -DirectorioZip compri
.Example
    ./ejercicio_4.ps1 -Directorio logs -DirectorioZip compri -empresa movistar
.Notes
    Parámetros:
    -Directorio: Directorio en el que se encuentran los archivos de log. Puede ser una ruta relativa o absoluta.
    -DirectorioZip: Directorio en el que se generarán los archivos comprimidos de los clientes. Puede ser una ruta relativa o absoluta.
    -Empresa: Nombre de la empresa a procesar. (opcional)
    Los nuevos archivos se agregan al ZIP existente para no perder los ya procesados.
#>
