Setup guide:

If you do not have msys2 installed run the following command from repo root:
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install-msys2.ps1
```

Options:
- `-InstallDir 'C:\tools\msys2'` — change the target install folder (default: `%USERPROFILE%\msys2`).

Verify installation:
- Open a new PowerShell or Command Prompt and run:

```powershell
g++ --version
where g++
```