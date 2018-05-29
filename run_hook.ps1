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

    & "./nuget.exe" install EasyHookNativePackage -version "2.7.6578.1"

	& cmake -DCMAKE_INSTALL_PREFIX="${script_dir}/hook-install" -G"$msvcpp" ../hook
	cd ..
}

if(Test-Path -Path $script_dir/hook-install) {
	Remove-Item -Path "${script_dir}/hook-install" -Confirm:$false -Force -Recurse
}
& cmake --build $script_dir/hook-build --config Release --target install

& "${script_dir}/run_renderservice.ps1" -injector "${script_dir}/hook-install/RenderServiceInjector.exe" -inject_dll "${script_dir}/hook-install/RenderServiceHook.dll" -use_injector