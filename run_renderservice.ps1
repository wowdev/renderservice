$script_dir = (split-path $MyInvocation.MyCommand.Path) 
. $script_dir/config.ps1

$PSDefaultParameterValues['*:Encoding'] = 'utf8'

$tmp = New-TemporaryFile
$tmppath = $tmp.FullName

Write "<?xml version=`"1.0`" encoding=`"UTF-8`" standalone=`"no`" ?>" | Out-File "$tmppath"
Write "<config>" | Out-File -Append "$tmppath"
Write "  <scenes path=`"$scenes_dir`" />" | Out-File -Append "$tmppath"
Write "  <log path=`"$log_dir`" level=`"$log_level`" />" | Out-File -Append "$tmppath"
Write "  <cas $product />" | Out-File -Append "$tmppath"
Write "  <thread low-priority-count=`"1`" />" | Out-File -Append "$tmppath"
Write "  <bnet-api path=`"bnet.marlam.in/fake-bnet-api`" timeout=`"5`" max-connection=`"10`" />" | Out-File -Append "$tmppath"
Write "</config>" | Out-File -Append "$tmppath"

cat $tmppath

& "$install_dir\RenderService-64.exe" -port $port -config $tmppath
