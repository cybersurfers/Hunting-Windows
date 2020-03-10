### before running the powershell script pushing wazuh and symon modify these files to reflect your security onion or wazuh manager server.

ossec.conf - line 11 ip address
install-sysmon.bat - line 5 ip address for wazuh_manager and wazuh_registration_server

## Ideally all the files required should be available and packaged into a folder.
## read the comments on the script for additional information and run the script from a powershell window instead of ISE since it is referencing the directory the script is ran from. ISE had issues following that.
