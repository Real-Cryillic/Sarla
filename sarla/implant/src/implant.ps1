# Credit to https://github.com/CodeXTF2/PyHmmm/blob/master/agent.py 
        # Register info:
        #   - AgentID           : int [needed]
        #   - Hostname          : CHAR [needed]
        #   - Username          : CHAR [needed]
        #   - Domain            : str [optional]
        #   - InternalIP        : str [needed]
        #   - Process Path      : str [needed]
        #   - Process Name      : str [needed]
        #   - Process ID        : DWORD [needed]
        #   - Process Parent ID : int [optional]
        #   - Process Arch      : str [needed]
        #   - OS Build          : str [needed]
        #   - OS Version        : str [needed]
        #   - OS Arch           : str [optional]


        $Hostname = $env:COMPUTERNAME
        $Username = $env:USERNAME
        $Domain = $env:USERDOMAIN
        $ProcessArch = $env:PROCESSOR_ARCHITECTURE
        $IP = (Get-NetIPAddress -AddressFamily IPv4 -InterfaceAlias Ethernet).IPAddress
        $PIDName = Get-Process | Where-Object Id -eq $PID | Select-Object ProcessName -ExpandProperty ProcessName
        $PPID = (gwmi win32_process -Filter "processid='$PID'").ParentProcessId
        $PPIDName = Get-Process | Where-Object Id -eq $PPID | Select-Object ProcessName -ExpandProperty ProcessName
        $Version = Get-WmiObject -Class win32_OperatingSystem | Select Version -ExpandProperty Version 
        $Build = Get-WmiObject -Class win32_OperatingSystem | Select BuildNumber -ExpandProperty BuildNumber
        $is64Bit = Test-Path 'Env:ProgramFiles(x86)'
        if($is64Bit -eq $true){
            $OSArch = 64
        } else {
            $OSArch = 32
        }