Setup guide:

If you do not have msys2 installed run the following command from repo root:
```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\install-msys2.ps1
```

Options:
- `-InstallDir 'C:\tools\msys2'` — change the target install folder (default: `%USERPROFILE%\msys2`).

Post-install steps (recommended):
- Open the MSYS2 MinGW64 shell (Start Menu or run `<InstallDir>\usr\bin\bash.exe --login -i`).
- Update packages inside the MSYS2 shell:

```bash
pacman -Syu
# restart the shell if pacman updated core components, then:
pacman -Su
# install common dev tools (optional)
pacman -S --noconfirm base-devel mingw-w64-x86_64-toolchain
```

Verify installation:
- Open a new PowerShell or Command Prompt and run:

```powershell
g++ --version
where g++
```