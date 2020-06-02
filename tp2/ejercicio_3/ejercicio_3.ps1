#!/usr/bin/pwsh

# ejercicio_3.ps1 | Trabajo Práctico 2) Ejercicio 3) | Primera reentrega
# Maximiliano José Bernardo | 41912800
# Nicolás Agustín Fábregues | 41893896
# Joela Belén Kachanosqui | 41917556
# Alejandro Nicolás Tacchella | 41893930
# Tomás Martín Vera | 41988332

Param(
    [Parameter(Position = 1, Mandatory = $true)]
    [String] $Directorio
)

$existe = Test-Path $Directorio
if($existe -ne $true){
    Write-Error "El directorio proporcionado no existe!"
    exit 1
}

$absoluta = $Directorio -Match "^/home*"
if($absoluta -eq $false){
    $Directorio = "$(Get-Location)/$Directorio"
}

#Creo el objeto de tipo FileSystemWatcher y le seteo sus propiedades
$FileSystemWatcher = New-Object System.IO.FileSystemWatcher
$FileSystemWatcher.Path = $Directorio
$FileSystemWatcher.IncludeSubdirectories = $true
$FileSystemWatcher.EnableRaisingEvents = $true

#Guardo la ubicación original
$location = Get-Location

#Me muevo al directorio pasado por parámetro
Set-Location $Directorio

#Acción que le paso al evento "Created" para que ejecute
$Action = {
    
    #Obtengo todos los archivos del directorio que cumplen con el patrón nombreEmpresa-numeroSemana.log
    $arch_validos = Get-ChildItem | Where-Object Name -Match '[a-zA-Z]+\-[0-9]+\.log$' | Select-Object -ExpandProperty "Name"

    $empresas = @()

    #Obtengo el campo nombreEmpresa de cada uno de los archivos
    foreach ($item in $arch_validos) {
        $empresas += $item.Split('-')[0]
    }

    $apariciones = @{}

    #Cuento la cantidad de archivos .log de cada empresa
    foreach ($item in $empresas) {
        $apariciones["$item"]++
    }

    #Recorro cada empresa y si tiene más de dos archivos en el directorio borro todos menos el más reciente
    foreach ($item in $apariciones.Keys) {
        if($apariciones["$item"] -ge 2){
            $archivos = $arch_validos | Where-Object {$_ -like "$item*"} 
            
            for($i = 0; $i -lt $archivos.Length-1; $i++){
                Remove-Item -Force $archivos[$i]
            }
        }
    }
}

#Creo el handler para el evento Created disparado por el objeto FileSystemWatcher
Register-ObjectEvent $FileSystemWatcher "Created" -Action $Action -SourceIdentifier FWCreated > /dev/null

try{
    do{
        sleep 1
    }while($true)
}
finally{ #Se ejecuta cuando apretas ctrl+c
    #Borro el handler del evento y elimino el objeto FileSystemWatcher
    Unregister-Event -SourceIdentifier FWCreated
    $FileSystemWatcher.EnableRaisingEvents = $false
    $FileSystemWatcher.Dispose()

    #Vuelvo a la ubicación original
    Set-Location $location
}

<#
.Synopsis
    Registra el directorio proporcionado como parámetro y elimina todos los archivos de log antiguos cada vez que se crea uno nuevo.
.Description
    El script ejercicio_3.ps1 se encarga de encarga de establecer un manejador de eventos en el directorio pasado como parámetro para que cada vez que se dispara el evento "Created" al crear un archivo nuevo se analice el directorio y se elimine los archivos de log antiguos y dejando únicamente el log más reciente de cada empresa.
.Example
    Ejecución del script:
    ./ejercicio_3.ps1 -Directorio logs
.Notes
    Parámetros:
    -Directorio: Directorio en el que se encuentran los archivos de log. Puede ser una ruta relativa o absoluta.

    Para detener la ejecución y el manejador de eventos solo hace falta presionar la combinación de teclas ctrl+C o cerrar la terminal desde la cual se está ejecutando.
#>