$ErrorActionPreference = "Stop"
$PSDefaultParameterValues['*:Encoding'] = 'utf8'
$script_dir = (split-path $MyInvocation.MyCommand.Path) 

. $script_dir/config.ps1

$ErrorActionPreference = "SilentlyContinue"
Invoke-WebRequest -Uri "http://localhost:$port/quit"

Wait-Process -Name RenderService-64.exe
