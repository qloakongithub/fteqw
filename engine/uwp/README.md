# FTEQW for UWP and Xbox Dev Mode

This directory contains the Visual Studio project files for building FTEQW for UWP (Universal Windows Platform) and Xbox consoles running in Developer Mode.

## Requirements

### For Development

- Windows 10/11 (version 1809 or later)
- Visual Studio 2019 or 2022 with the following workloads:
  - Universal Windows Platform development
  - Desktop development with C++
  - Game development with C++ (optional, for GDK support)
- Windows 10 SDK (10.0.17763.0 or later)

### For Xbox Dev Mode

- Xbox One, Xbox One S, Xbox One X, Xbox Series S, or Xbox Series X console
- Xbox in Developer Mode (see [Xbox Dev Mode activation](https://docs.microsoft.com/en-us/windows/uwp/xbox-apps/devkit-activation))
- Xbox Device Portal enabled on your console

## Building

1. Open `FTEQW_UWP.sln` in Visual Studio
2. Select your target platform:
   - **x64**: For Windows 10/11 PCs and Xbox One/Xbox Series X|S (Remote deployment)
   - **ARM64**: For Windows on ARM devices and Xbox Series consoles (native)
3. Select configuration:
   - **Debug**: For development with debugging symbols
   - **Release**: For optimized builds
4. Build the solution (Ctrl+Shift+B)

## Deploying to Xbox

### Prerequisites

1. Put your Xbox console in Developer Mode:
   - Install the "Xbox Dev Mode" app from the Microsoft Store on your Xbox
   - Follow the activation process
   - Restart your Xbox into Dev Mode

2. Enable Device Portal on your Xbox:
   - Go to Dev Home on Xbox
   - Navigate to Settings
   - Enable Device Portal and note the IP address

3. In Visual Studio:
   - Right-click the FTEQW_UWP project
   - Select Properties → Debugging
   - Set Remote Machine to your Xbox's IP address
   - Set Authentication Type to "Universal (Unencrypted Protocol)"

4. Deploy:
   - Press F5 to build and deploy with debugging
   - Or Ctrl+F5 to build and deploy without debugging

## Features

### Supported

- D3D11 rendering
- Xbox controller input via XInput API
- Local file storage in app data folder
- Basic audio output
- Xbox Live integration (via system)

### Not Yet Supported

- Networking (requires UWP socket APIs)
- Plugin system (UWP doesn't support dynamic library loading)
- Server hosting (network restrictions)
- CD audio

## File Storage

UWP apps have restricted filesystem access. FTEQW will store files in:

- App local storage: `%LOCALAPPDATA%\Packages\FTEQW.UWP_*\LocalState\`
- On Xbox: Accessible via Device Portal

To add game data (PAK files):

1. Use Visual Studio Device Portal or Xbox Device Portal
2. Navigate to File Explorer
3. Upload files to the app's LocalState folder
4. Or include files in the deployment package

## Known Limitations

- No dedicated server mode (UWP restrictions)
- No direct file system access (sandboxed)
- No arbitrary DLL loading (UWP security)
- Limited networking capabilities
- Must use UWP-compliant APIs

## Input

Xbox controllers are automatically detected via XInput API. The following buttons are mapped:

- Left Stick: Movement
- Right Stick: Look
- A: Jump
- B: Back/Cancel
- X: Use
- Y: Weapon switch
- LT: Attack
- RT: Alt Attack
- D-Pad: Menu navigation
- Menu button: Console
- View button: Scores

## Troubleshooting

### Build Errors

**Error: Cannot find Windows 10 SDK**
- Install Windows 10 SDK via Visual Studio Installer
- Ensure version 10.0.17763.0 or later is installed

**Error: WINRT not defined**
- This is expected. The WINRT preprocessor definition is set in the project properties.
- Ensure you're building the UWP project, not another configuration.

### Deployment Issues

**Cannot deploy to Xbox**
- Verify Xbox is in Dev Mode (should see Dev Home)
- Check IP address is correct
- Ensure firewall allows Visual Studio Remote Debugger
- Try pinging the Xbox from your PC

**App crashes on start**
- Check Output window in Visual Studio for error messages
- Verify all required assets are included
- Check Device Portal logs on Xbox

## References

- [UWP App Development](https://docs.microsoft.com/en-us/windows/uwp/)
- [Xbox Development](https://docs.microsoft.com/en-us/windows/uwp/xbox-apps/)
- [XInput API](https://docs.microsoft.com/en-us/windows/win32/xinput/xinput-game-controller-apis-portal)
- [Xbox Dev Mode](https://www.xbdev.store) - Community resources
- [Worley's Notes](https://wiki.xbdev.store/en/WorleysNotes) - Xbox Dev Mode development tips
- [Helpful Links](https://wiki.xbdev.store/en/HelpfulLinksDev) - Additional Xbox development resources

## Contributing

When contributing to the UWP/Xbox port:

1. Test on both PC (UWP) and Xbox Dev Mode if possible
2. Maintain compatibility with the WINRT preprocessor define
3. Use only UWP-compliant APIs
4. Document any Xbox-specific features or limitations
5. Follow the existing code style

## License

Same as FTEQW - GPL v2. See LICENSE file in the root directory.
