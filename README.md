# SA5RA STUDIO — Mobile-to-Windows Build Starter

**Current version: 0.2.0 (early audio-engine prototype)**

This repository is prepared so the source can be edited from an Android phone and a Windows installer can be built by GitHub Actions in the cloud.

## What this version actually contains

- Native JUCE desktop application shell.
- Four audio playback tracks.
- Load supported audio files (WAV/AIFF/FLAC/OGG as supported by the JUCE build).
- Play/Stop transport.
- Independent track gain controls.
- SA5RA STUDIO application branding and installer icon.

## What is NOT implemented yet

This is not a finished Pro Tools competitor. Recording, waveform timeline editing, MIDI, VST3 hosting, automation, professional routing, and all six AI modules are future milestones. The installer packages only the current prototype. Do not describe unfinished features as working.

## Build a Windows installer from a phone

1. Create a GitHub account and a new repository named `sa5ra-studio`.
2. For the simplest free GitHub Actions workflow, make the repository **Public**. Public source code is visible to everyone. If you need private source, check your GitHub Actions usage limits before choosing a private repository.
3. Upload the contents of this project ZIP to the repository root. Do not upload the ZIP as a single file; extract it first and upload the folders/files so `.github/workflows/windows-installer.yml` is at the repository root.
4. Ensure the default branch is named `main`.
5. Open the repository's **Actions** tab. Select **Build SA5RA STUDIO Windows Installer**, then tap **Run workflow** if it has not started automatically.
6. Wait for the workflow to finish successfully.
7. Open the completed workflow run and download the artifact named `SA5RA-STUDIO-Windows-Setup`.
8. Extract the downloaded artifact ZIP. It contains `SA5RAStudioSetup.exe`.
9. Transfer that EXE to the Windows 11 computer and run it.

Every push to `main` also triggers a new build. Each successful run creates a downloadable installer artifact.

## Build architecture

- C++17
- JUCE 8.0.6 (fetched by CMake during the cloud build)
- CMake
- MSVC on the GitHub-hosted Windows runner
- Inno Setup to package `SA5RAStudioSetup.exe`

## Important limitations

- The first cloud build needs internet to download JUCE and Inno Setup.
- The installed application is intended to run offline; this prototype does not include downloadable AI models.
- The current app is a starter prototype, not yet optimized for professional real-time use. Its simple audio callback synchronization and in-memory file loading must be upgraded before large sessions or critical live use.
- The 4 GB RAM computer may struggle with the development tools, which is why compilation is delegated to GitHub Actions. The resulting app's practical performance still depends on the computer and audio interface.
- macOS builds are not included in this Windows installer workflow yet.
