![CI status](https://github.com/lclutz/computer-vision/actions/workflows/ci.yml/badge.svg)

# Computer Vision Projekt

Projekt für das Labor der Vorlesung _Computer Vision_ an der
[Hochschule Karlsruhe](https://www.h-ka.de/).

## Kurzanleitung

```shell
mkdir build && cd build          # Build-Verzeichnis erstellen
cmake -G "Ninja Multi-Config" .. # Build Dateien erzeugen
cmake --build .                  # Programm bauen
```

Wer unter Windows entwickelt muss die `x64 Native Tools Command Prompt`
verwenden um die Kommandos auszuführen.

Der Release-Build hat bessere Performanz insbesondere für ältere Computer:

```shell
cmake --build . --config Release # Programm in Release-Mode bauen
```

## Voraussetzungen

### Windows

- [git](https://git-scm.com/)
- [Visual Studio](https://visualstudio.microsoft.com) mit der Workload
  `Desktopentwicklung mit C++`.

### Linux

Getestet mit Ubuntu 22.04 LTS:

```shell
sudo apt install build-essential git git-lfs cmake ninja-build libsdl2-dev libopencv-dev
```

### MacOS

TODO
