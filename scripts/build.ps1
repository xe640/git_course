Set-StrictMode -Version Latest

$RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
$CppDir = Join-Path $RepoRoot "cpp"
$IncludeDir = Join-Path $CppDir "include"
$ImguiDir = Join-Path $CppDir "include/imgui"
$LibDir = Join-Path $CppDir "lib"
$BuildDir = Join-Path $RepoRoot "build"

New-Item -ItemType Directory -Path $BuildDir -Force | Out-Null

Write-Host "Using repo root:" $RepoRoot
Write-Host "Include dir:" $IncludeDir
Write-Host "Lib dir:" $LibDir
Write-Host "Build dir:" $BuildDir

# Gather source files: top-level main and any sources under cpp/src
$src = @()
$main = Join-Path $CppDir "main.cpp"
if (Test-Path $main) { $src += $main }
$src += Get-ChildItem -Path (Join-Path $CppDir "src") -Include *.c,*.cpp -Recurse -File -ErrorAction SilentlyContinue | ForEach-Object { $_.FullName }
$src += Get-ChildItem -Path ($ImguiDir) -Include *.c,*.cpp -Recurse -File -ErrorAction SilentlyContinue | ForEach-Object { $_.FullName }
$src = $src | Select-Object -Unique

if ($src.Count -eq 0) {
	Write-Error "No source files found in $CppDir (expected main.cpp and cpp/src/*)."
	exit 1
}

# Detect available compiler
$gpp = Get-Command g++ -ErrorAction SilentlyContinue

if (-not $gpp) {
	Write-Error "g++ not found on PATH. Please install MinGW/MSYS2 or add g++ to PATH."
	exit 2
}

$OutName = Read-Host -Prompt "bin name"
if ([string]::IsNullOrWhiteSpace($OutName)) { $OutName = "test" }

$outExe = Join-Path $BuildDir ($OutName + ".exe")

# Minimal g++ build: hardcoded libraries (-lglfw3 -lopengl32)
$compileArgs = @(
	"-std=c++17",
	"-I$IncludeDir",
	"-L$LibDir",
	@($src),
	"-lglfw3",
	"-lopengl32",
	"-luser32",
	"-lgdi32",
	"-lkernel32",
	"-lshell32",
	"-o",
	$outExe
)

Write-Host "Using g++ at:" $gpp.Path
Write-Host "Building with args:" $compileArgs
Write-Host "Sources:" ($src -join ' ')

& $gpp.Path @compileArgs
if ($LASTEXITCODE -ne 0) { Write-Error "g++ failed with exit code $LASTEXITCODE"; exit $LASTEXITCODE }
Write-Host "Built:" $outExe
exit 0

