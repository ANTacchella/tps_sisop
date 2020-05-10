Param(
    [Parameter(Position = 1, Mandatory = $true)]
    [String] $path
)

#Creo el objeto de tipo FileSystemWatcher y le seteo sus propiedades
$FileSystemWatcher = New-Object System.IO.FileSystemWatcher
$FileSystemWatcher.Path = $path
$FileSystemWatcher.IncludeSubdirectories = $true
$FileSystemWatcher.EnableRaisingEvents = $true

#Guardo la ubicación original
$location = Get-Location

#Me muevo al directorio pasado por parámetro
Set-Location $path

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