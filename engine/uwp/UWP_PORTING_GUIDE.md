# UWP Porting Guide for FTEQW (Xbox Dev Mode)

This guide provides detailed instructions for porting FTEQW to UWP and Xbox Dev Mode, based on community best practices from worleydl and the xbdev.store community.

## Important Notes

**GDK vs UWP**: The Gaming Device Kit (GDK) and GameInput API are NOT compatible with UWP. This port uses UWP-compatible APIs only:
- XInput (not GameInput) for controller input
- D3D11 (not D3D12) for graphics
- Standard UWP APIs for system functions

## Build Configuration Requirements

### Runtime Library Settings

**CRITICAL**: All project components MUST use the Multithreaded DLL runtime library:
- Debug builds: `/MDd` (Multithreaded Debug DLL)
- Release builds: `/MD` (Multithreaded DLL)

**DO NOT USE** `/MT` (Multithreaded static) - it is incompatible with UWP.

In Visual Studio:
1. Project Properties → C/C++ → Code Generation → Runtime Library
2. Set to "Multi-threaded DLL (/MD)" for Release
3. Set to "Multi-threaded Debug DLL (/MDd)" for Debug

### CMake Configuration for UWP

If building dependencies with CMake, use:

```bash
cmake -S . -B build -G "Visual Studio 17 2022" \
  -DCMAKE_SYSTEM_NAME=WindowsStore \
  -DCMAKE_SYSTEM_VERSION=10.0
```

This ensures dependencies are built with UWP compatibility.

## DLL Dependencies

### VCRUNTIME Library Check

UWP executables and DLLs MUST depend on `VCRUNTIME140_APP.dll` (note the `_APP` suffix), not the regular `VCRUNTIME140.dll`.

**Verify with dumpbin**:
```cmd
dumpbin /DEPENDENTS FTEQW_UWP.exe
```

Look for:
- ✅ CORRECT: `VCRUNTIME140_APP.dll`
- ❌ WRONG: `VCRUNTIME140.dll` (indicates non-UWP component)

### Analyzing DLL Dependencies

Use these tools to diagnose DLL issues:

**1. dumpbin** (Visual Studio Developer Command Prompt):
```cmd
dumpbin /DEPENDENTS your_file.exe
dumpbin /DEPENDENTS your_library.dll
```

**2. gflags** (Global Flags Editor) for detailed loader debugging:
1. Run `gflags.exe`
2. Go to "Image File" tab
3. Enter executable name (e.g., `FTEQW_UWP.exe`)
4. Check "Show loader snaps"
5. Click OK

Now when debugging, Visual Studio Output will show detailed DLL loading information.

### MinGW as Alternative

If you encounter runtime library conflicts with Visual Studio-built DLLs:
- Consider using MinGW-compiled libraries
- MinGW projects include their own runtime DLLs, avoiding UWP conflicts
- This can be useful for third-party dependencies

## DLL Loading Restrictions

UWP applications on Xbox have strict DLL loading path restrictions:

**Allowed locations**:
- ✅ Application package directory (where .exe is located)
- ✅ Application LocalState folder

**NOT allowed**:
- ❌ External drives (E:\)
- ❌ Arbitrary filesystem paths
- ❌ System directories

**Note**: UWP doesn't support `LoadLibrary()` for arbitrary DLLs anyway. All DLLs must be packaged with the app or approved system libraries.

## OpenGL Support (Optional)

FTEQW uses D3D11 by default, but if OpenGL support is needed:

### Mesa-UWP

Use aerisarn's Mesa-UWP project for UWP-compatible OpenGL:
- Repository: https://github.com/aerisarn/mesa-uwp
- Follow build instructions in the Mesa-UWP README

**Required DLLs** (include with your app):
- `libgallium_wgl.dll`
- `libglapi.dll`
- `opengl32.dll`
- `z-1.dll` (zlib dependency)

**Linker settings**: Add `opengl32.lib` to linker dependencies (if not using SDL/GLFW)

### SDL2 with OpenGL

If using SDL2 with OpenGL:
- Use aerisarn's SDL-UWP-GL fork: https://github.com/aerisarn/SDL-uwp-gl
- Build SDL2 with UWP support
- Include SDL2 DLLs with Mesa-UWP DLLs

## Input Handling

### XInput for Controllers

FTEQW UWP uses XInput (NOT GameInput) for controller support:

```c
#include <xinput.h>
#pragma comment(lib, "xinput.lib")

XINPUT_STATE state;
DWORD result = XInputGetState(0, &state);  // Controller 0
if (result == ERROR_SUCCESS) {
    // Process input
}
```

**Supported controllers**:
- Xbox One controllers
- Xbox Series X|S controllers  
- Generic XInput-compatible gamepads

**Button mapping** (see `sys_uwp.c`):
- A = Jump
- B = Back
- X = Use
- Y = Weapon switch
- LT/RT = Attack
- Sticks = Movement/Look
- D-Pad = Menu navigation

## Filesystem Access

UWP apps run in a sandbox with limited filesystem access:

**Available locations**:
- Application install directory (read-only)
- LocalState folder (read/write): `Windows.Storage.ApplicationData.Current.LocalFolder`

**On PC**: `%LOCALAPPDATA%\Packages\FTEQW.UWP_<publisher>\LocalState\`

**On Xbox**: Accessible via Device Portal → File Explorer

**Deploy game data**:
1. Use Xbox Device Portal
2. Navigate to File Explorer
3. Upload PAK/PK3 files to LocalState folder
4. Or include in package during development

## Deployment to Xbox Dev Mode

### Prerequisites

1. **Activate Developer Mode** on Xbox:
   - Install "Xbox Dev Mode" app from Microsoft Store
   - Follow activation instructions
   - Restart Xbox into Dev Mode

2. **Enable Device Portal**:
   - Open "Dev Home" on Xbox
   - Go to Settings
   - Enable Device Portal
   - Note the IP address

3. **Visual Studio Setup**:
   - Right-click project → Properties → Debugging
   - Remote Machine: Enter Xbox IP address
   - Authentication: "Universal (Unencrypted Protocol)"

4. **Deploy**:
   - Press F5 (debug) or Ctrl+F5 (no debug)
   - First deployment may take a few minutes

### Troubleshooting Deployment

**Cannot connect to Xbox**:
- Verify Xbox is in Dev Mode (Dev Home should be visible)
- Check IP address matches
- Ping Xbox from PC: `ping <xbox-ip>`
- Check Windows Firewall allows Visual Studio Remote Debugger

**App crashes on start**:
- Check Output window for error messages
- Verify all DLLs are UWP-compatible (check with dumpbin)
- Check Device Portal logs on Xbox

**DLL not found errors**:
- Ensure all required DLLs are in app package
- Verify DLL dependencies with dumpbin
- Check VCRUNTIME140_APP.dll vs VCRUNTIME140.dll

## Performance Considerations

### Xbox Series S/X

- **Series X**: 8-core CPU, 12 TFLOPS GPU, 16GB RAM
- **Series S**: 8-core CPU, 4 TFLOPS GPU, 10GB RAM

### Optimization Tips

1. Use Release builds for performance testing
2. Enable LTCG (Link Time Code Generation) for release
3. Profile with Visual Studio Performance Profiler
4. Monitor frame rate on target hardware

## Debugging

### Remote Debugging

Visual Studio supports full debugging on Xbox:
- Breakpoints
- Watch variables
- Call stack inspection
- Memory viewer

### Device Portal

Access Xbox Device Portal via browser:
- URL: `http://<xbox-ip>:11443`
- Features:
  - File Explorer (upload/download files)
  - Performance Monitor
  - Process List
  - Crash Dumps
  - System Information

## Known Limitations

1. **No networking** (by design in FTEQW UWP port)
   - UWP sockets require different APIs
   - Future enhancement possible

2. **No plugin system**
   - UWP doesn't support LoadLibrary for arbitrary DLLs
   - All functionality must be compiled in

3. **No dedicated server**
   - UWP restrictions and networking limitations

4. **Sandboxed filesystem**
   - Limited to app directories
   - No arbitrary file access

## References

- [worleydl's UWP Porting Notes](https://wiki.xbdev.store/en/WorleysNotes)
- [Xbox Dev Mode Resources](https://wiki.xbdev.store/en/HelpfullLinksDev)
- [Microsoft UWP Documentation](https://docs.microsoft.com/en-us/windows/uwp/)
- [Xbox Developer Mode](https://docs.microsoft.com/en-us/windows/uwp/xbox-apps/devkit-activation)
- [aerisarn's Mesa-UWP](https://github.com/aerisarn/mesa-uwp)
- [aerisarn's SDL-UWP-GL](https://github.com/aerisarn/SDL-uwp-gl)

## Community

For help with Xbox Dev Mode development:
- xbdev.store Discord
- Reddit: r/xboxone_devmode

## Contributing

When contributing to the UWP port:
1. Test on both PC UWP and Xbox Dev Mode
2. Verify all DLLs are UWP-compatible
3. Use only UWP-compatible APIs (no GDK)
4. Document platform-specific code
5. Follow existing code style

## License

GPL v2 - Same as FTEQW. See LICENSE file.
