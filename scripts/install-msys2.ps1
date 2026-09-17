param(
    [string]$InstallDir
)

function Write-Log {
    param($msg)
    Write-Host "[install-msys2] $msg"
}

# Check for existing g++
$gpp = Get-Command g++ -ErrorAction SilentlyContinue
if ($gpp) {
    Write-Log "g++ already found in PATH; msys2 is already installed"
    exit 0
}

# Ensure install directory parent exists
if (-not $InstallDir) { $InstallDir = $args[0] }
if (-not $InstallDir) { $InstallDir = "$env:USERPROFILE\msys2" }

Write-Log "Target install directory: $InstallDir"

# Attempt to discover a recent MSYS2 installer on the official repo
$baseUrl = 'https://repo.msys2.org/distrib/x86_64/'
try {
    Write-Log "Fetching available installers from $baseUrl"
    $resp = Invoke-WebRequest -Uri $baseUrl -UseBasicParsing -ErrorAction Stop
    $content = $resp.Content
    $matches = [regex]::Matches($content, 'msys2-x86_64-[0-9]{8}\.exe') | ForEach-Object { $_.Value } | Sort-Object
    if ($matches.Count -gt 0) {
        $installerName = $matches[-1]
        $installerUrl = $baseUrl + $installerName
        Write-Log "Discovered installer: $installerName"
    } else {
        Write-Log "No dated installer found on index; falling back to known filename 'msys2-x86_64-latest.exe'"
        $installerName = 'msys2-x86_64-latest.exe'
        $installerUrl = $baseUrl + $installerName
    }
} catch {
    Write-Log "Failed to fetch index page: $_. Exception.Message"
    Write-Log "Falling back to default installer URL: msys2-x86_64-latest.exe"
    $installerName = 'msys2-x86_64-latest.exe'
    $installerUrl = $baseUrl + $installerName
}

$confirmation = Read-Host -Prompt "Proceed with installation [y/n]"

if(-not ($confirmation -eq "y" -or $confirmation -eq "Y")) {
    Exit 0
}

$tmpInstaller = Join-Path $env:TEMP "msys2-installer-$([System.Guid]::NewGuid().ToString()).exe"

Write-Log "Downloading $installerUrl to $tmpInstaller"
try {
    Invoke-WebRequest -Uri $installerUrl -OutFile $tmpInstaller -UseBasicParsing -ErrorAction Stop
} catch {
    Write-Log "Download failed: $_.Exception.Message"
    Exit 1
}

# Run the installer silently. NSIS installers accept /S for silent and /D=path as the last arg.
# The installer requires the /D argument to be the last argument and with no trailing slash.
$target = $InstallDir.TrimEnd('\')
$arg = "/S"
$argD = "/D=$target"

Write-Log "Preparing target directory and launching interactive installer."
try {
    if (-not (Test-Path $target)) {
        New-Item -ItemType Directory -Path $target -Force | Out-Null
        Write-Log "Created directory: $target"
    } else {
        Write-Log "Directory already exists: $target"
    }

    Write-Log "Please select the above directory in the installer when prompted."
    Write-Log "Launching the installer interactively now."
    $proc = Start-Process -FilePath $tmpInstaller -PassThru
    Write-Log "Installer launched (PID $($proc.Id)). Waiting for it to exit..."
    $proc.WaitForExit()
    Write-Log "Installer exited with code $($proc.ExitCode). Continuing script."
} catch {
    Write-Log "Failed to launch installer interactively: $_.Exception.Message"
    Remove-Item -LiteralPath $tmpInstaller -ErrorAction SilentlyContinue
    Exit 1
}

Remove-Item -LiteralPath $tmpInstaller -ErrorAction SilentlyContinue

# Compute the mingw64 bin path (common MSYS2 layout: <InstallDir>\mingw64\bin)
$mingwBin = Join-Path $target 'mingw64\bin'
if (-not (Test-Path $mingwBin)) {
    # Some installers create msys64 root even when you pass msys2; check for msys64
    $alt = Join-Path $target 'msys64\mingw64\bin'
    if (Test-Path $alt) { $mingwBin = $alt }
}

if (-not (Test-Path $mingwBin)) {
    Write-Log "Could not find mingw64 bin at expected location: $mingwBin"
    Write-Log "Installation may have a different layout. Inspect $target manually."
    Exit 1
}

Write-Log "Found mingw64 bin: $mingwBin"

# Add to current session PATH
if (-not ($env:Path -split ';' | Where-Object { $_ -eq $mingwBin })) {
    $env:Path = "$mingwBin;$env:Path"
    Write-Log "Added to current session PATH."
} else {
    Write-Log "Already present in current PATH."
}

# Persist to user PATH environment variable
try {
    $userPath = [Environment]::GetEnvironmentVariable('Path','User')
    if (-not $userPath) { $userPath = '' }
    $parts = $userPath -split ';' | Where-Object { $_ -and ($_ -ne $mingwBin) }
    $parts = $parts + $mingwBin
    $newUserPath = ($parts -join ';')
    [Environment]::SetEnvironmentVariable('Path',$newUserPath,'User')
    Write-Log "Persisted $mingwBin to user PATH. New user PATH will apply to new shells."
} catch {
    Write-Log "Failed to update user PATH: $_.Exception.Message"
    Write-Log "Unable to persist $mingwBin to user PATH automatically."
    # Prepare manual instructions and open a helper PowerShell window so the user can perform the change and close it when done.
    $manualCmd = "[Environment]::SetEnvironmentVariable('Path', '{0}', 'User')" -f ($newUserPath -replace "'","''")
    $instr = @"
Manual step required: add the following value to your User PATH and then CLOSE THIS WINDOW when finished.

Path to add:
$mingwBin

PowerShell command to run (paste into a PowerShell window):
$manualCmd

Alternatively: Open 'Edit environment variables for your account' from Start, add the path above to Path, save, and close the dialog.

This helper window will stay open until you close it.
"@
    Write-Log "Opening helper PowerShell window with manual instructions. Close that window after you've updated PATH."
    $psArgs = @('-NoExit', '-Command', "Write-Host '$instr' -ForegroundColor Yellow")
    $helper = Start-Process -FilePath powershell.exe -ArgumentList $psArgs -PassThru
    $helper.WaitForExit()
    Write-Log "Helper window closed. Assuming manual PATH update completed."
}

Write-Log "MSYS2 installation complete."

# Locate bash.exe inside the installation
$bashExe = Join-Path $target 'usr\bin\bash.exe'
if (-not (Test-Path $bashExe)) {
    $alt = Join-Path $target 'msys64\usr\bin\bash.exe'
    if (Test-Path $alt) { $bashExe = $alt }
}

if (-not (Test-Path $bashExe)) {
    Write-Log "Could not locate MSYS2 bash at expected location: $bashExe"
    Write-Log "Please open the MSYS2 MinGW64 shell and run the recommended updates and toolchain installation manually."
} else {
    $steps = @(
        'pacman -Syu --noconfirm',
        'pacman -Su --noconfirm',
        'pacman -S --noconfirm mingw-w64-x86_64-toolchain'
    )

    Write-Log "Launching MSYS2 shell(s) to run the update/install steps. The shell may close after each update and will be reopened automatically."
    foreach ($step in $steps) {
        try {
            $proc = Start-Process -FilePath $bashExe -ArgumentList '--login','-i','-c',$step -PassThru
            Write-Log "Running MSYS2 step: $step"
            $proc.WaitForExit()
            Write-Log "MSYS2 step finished with exit code $($proc.ExitCode): $step"
            if ($proc.ExitCode -ne 0) {
                Write-Log "This step failed. The script will continue to the next step anyway so the user can inspect the result."
            }
        } catch {
            Write-Log "Failed to launch MSYS2 shell for: $step"
            Write-Log "Please reopen the MSYS2 shell and run this command manually: $step"
        }
    }
}
