if (!([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole] "Administrator")) 
{
	"Run as Administrator, or inject will fail"
	exit
}

$ErrorActionPreference = "Stop"
$PSDefaultParameterValues['*:Encoding'] = 'utf8'
$script_dir = (split-path $MyInvocation.MyCommand.Path) 

. $script_dir/config.ps1

& $script_dir/stop_renderservice.ps1

if(!(Test-Path -Path $script_dir/hook-build)) {
	mkdir hook-build
	cd hook-build

    if(-not (Test-Path nuget.exe))
    {
        Invoke-WebRequest -Uri "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" -OutFile nuget.exe
    }

    & "./nuget.exe" install EasyHookNativePackage

	& cmake -DCMAKE_INSTALL_PREFIX="${script_dir}/hook-install" -G"$msvcpp" ../hook
	cd ..
}

if(Test-Path -Path $script_dir/hook-install) {
	Remove-Item -Path "${script_dir}/hook-install" -Confirm:$false -Force -Recurse
}
& cmake --build $script_dir/hook-build --config Release --target install

$renderservice = Start-Job -ArgumentList $script_dir -ScriptBlock {
	& "$($args[0])/run_renderservice.ps1" 2>&1 | Write-Host
}
& hook-install/RenderServiceInjector.exe "${script_dir}/hook-install/RenderServiceHook.dll"

while ($renderservice.State -eq 'Running') {
  Receive-Job $renderservice  2>&1 | Write-Host
  Start-Sleep -Milliseconds 50
}
Receive-Job $renderservice
Stop-Job $renderservice
Remove-Job $renderservice
