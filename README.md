# Protocol+ Auto Clicker

A simple native Windows CPS-based auto clicker.

## Features

- CPS input instead of milliseconds
- 1–1000 CPS
- Left / Right / Middle mouse buttons
- F6–F9 global toggle hotkey
- Native Win32 GUI
- Windows installer with Start Menu/Desktop shortcuts
- Uninstaller
- No Python required
- GitHub Actions builds the Windows installer automatically

## Build on GitHub

1. Create a GitHub repository.
2. Upload all files from this project.
3. Open **Actions**.
4. Run **Build Windows Installer**.
5. Download the `ProtocolPlus-AutoClicker-Installer` artifact.
6. The artifact contains `ProtocolPlus_Auto_Clicker_Setup.exe`.

## Local Windows build

Install Visual Studio 2022 Build Tools with the Desktop C++ workload and Inno Setup 6.

Then run:

```bat
build.bat
```

The installer will be created in:

```text
release\ProtocolPlus_Auto_Clicker_Setup.exe
```
