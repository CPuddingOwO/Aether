$sourceDir = "shaders"
$destDir = "res/shaders"

if (-not (Test-Path $destDir)) {
    New-Item -ItemType Directory -Path $destDir
    Write-Host "Directory '$destDir' created."
}

$vertFiles = Get-ChildItem -Path $sourceDir -Filter "*.vert.glsl" -Recurse
$fragFiles = Get-ChildItem -Path $sourceDir -Filter "*.frag.glsl" -Recurse

Write-Host $vertFiles
Write-Host $fragFiles

$glslc = "glslc"

foreach ($file in $vertFiles) {

    $outputFile = Join-Path $destDir ($file.BaseName + ".spv")

    Write-Host "[Vertex] Compiling: $($file.FullName) -> $outputFile"
    & $glslc -fshader-stage=vert $file.FullName -o $outputFile

    if ($?) {
        Write-Host "[Vertex] Compilation successful: $outputFile"
    } else {
        Write-Host "[Vertex] Compilation failed: $file.FullName"
    }
}

foreach ($file in $fragFiles) {
    $outputFile = Join-Path $destDir ($file.BaseName + ".spv")

    Write-Host "[Fragment] Compiling: $($file.FullName) -> $outputFile"
    & $glslc -fshader-stage=frag $file.FullName -o $outputFile

    if ($?) {
        Write-Host "[Fragment] Compilation successful: $outputFile"
    } else {
        Write-Host "[Fragment] Compilation failed: $file.FullName"
    }
}
