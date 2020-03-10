cd c:\sysmon

c:\sysmon\sysmon.exe -i C:\sysmon\ionstorm-config.xml -accepteula
sc config sysmon start= auto
c:\sysmon\wazuh-agent-3.11.1-1.msi /q WAZUH_MANAGER="XXX.XXX.XXX.XXX" WAZUH_REGISTRATION_SERVER="XXX.XXX.XXX.XXX"
net start "OssecSvc"