##Prompts for Privileged User credentials for remote clients
##initial information gathering for script.
$user = Get-Credential
$dir1 = pwd
$dir = $dir1.ToString();
$diredit = $dir.Substring(3)
$ip = Read-Host -p "Your IP Address"
$pass = Read-Host -p "Domain Admin Password"
$domain = Read-Host -p "Domain"

##Prompts for Client IP list and location of executable scripts***must create ips.txt file with ipaddress for all hosts in network that you want to push too.
$clients = "$dir1\ips.txt"
$psexec = "\\$ip\c$\$diredit\Sysmon\PsExec.exe" #(Note: Ensure psexec.exe is Unblocked)
$bat = "\\$ip\c$\$diredit\Sysmon\install-sysmon.bat" #(Note: This file must be accessible to the target)

#SMB Share - making an smb share of the current directory so workstations will have access to copy files from.
New-SmbShare -Name "Sysmon" -Path "$dir1\Sysmon" -FullAccess "users" -ErrorAction SilentlyContinue

##For every IP in the Client IP list, psexec will make a remote connection and execute the .bat script.
##Execution will complete in order according to the Client IP list.
ForEach ($client in Get-Content $clients) {
    New-Item -Path \\$client\C$\sysmon -type directory -Force;
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\sysmon.exe -Destination \\$client\c$\sysmon\Sysmon.exe;
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\ionstorm-config.xml -Destination \\$client\c$\sysmon\ionstorm-config.xml;
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\wazuh-agent-3.11.1-1.msi -Destination \\$client\c$\sysmon\wazuh-agent-3.11.1-1.msi;
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\install-sysmon.bat -Destination \\$client\c$\sysmon\install-sysmon.bat;
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\uninstall-sysmon.bat -Destination \\$client\c$\sysmon\uninstall-sysmon.bat;
    Write-Host "$client copy of files completed successfully";
}
##psexec install wazzuh
Start-Process -filepath $psexec "@$dir1\ips.txt -u $domain\Administrator -p $pass c:\sysmon\install-sysmon.bat -accepteula" -wait;

##psexec delete wazzuh - uncomment the line below and run the line individually
##Start-Process -filepath $psexec "@$dir1\ips.txt -u $domain\Administrator -p $pass c:\sysmon\uninstall-sysmon.bat -accepteula" -wait;

#push config for ossecon.conf then restart the service.
ForEach ($client in Get-Content $clients) {
    Copy-Item -Path \\$ip\c$\$diredit\Sysmon\ossec.conf -Destination "\\$client\c$\Program Files (x86)\ossec-agent\ossec.conf"
    Get-Service -ComputerName $client -Name OssecSvc | Restart-Service
    Write-Host "$client osssec.conf copied  completed successfully";
}

echo "Please unshare $dir1\Sysmon on your local mahcine"