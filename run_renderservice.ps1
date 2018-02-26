param (
  [switch]$use_injector = $false,
  [string]$injector = "",
  [string]$inject_dll = ""
)

$ErrorActionPreference = "Stop"
$PSDefaultParameterValues['*:Encoding'] = 'utf8'
$script_dir = (split-path $MyInvocation.MyCommand.Path) 

. $script_dir/config.ps1

$tmppath = "${script_dir}/run_renderservice-tmp_config"

Write "<?xml version=`"1.0`" encoding=`"UTF-8`" standalone=`"no`" ?>" | Out-File "$tmppath"
Write "<config>" | Out-File -Append "$tmppath"
Write "  <scenes path=`"$scenes_dir\`" />" | Out-File -Append "$tmppath"
Write "  <log path=`"$log_dir`" level=`"$log_level`" />" | Out-File -Append "$tmppath"
Write "  <cas $product />" | Out-File -Append "$tmppath"
Write "  <thread low-priority-count=`"1`" />" | Out-File -Append "$tmppath"
Write "  <bnet-api path=`"bnet.marlam.in/fake-bnet-api`" timeout=`"5`" max-connection=`"10`" />" | Out-File -Append "$tmppath"
Write "</config>" | Out-File -Append "$tmppath"

cat $tmppath

$cmdline = "$install_dir\RenderService-64.exe -port $port -config $tmppath -level $gxlevel -debugFrame 1"

$cmdline
& { 
  $ErrorActionPreference='continue'
  if ($use_injector) {
    & $injector $inject_dll $cmdline 2>&1
  } else {
    & $cmdline 2>&1
  }
}
