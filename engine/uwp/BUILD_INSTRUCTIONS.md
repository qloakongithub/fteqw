# Building FTEQW for UWP/Xbox Dev Mode

## Quick Start

### Option 1: Using Visual Studio (Recommended)

1. Open `FTEQW_UWP.sln` in Visual Studio 2019 or 2022
2. The project is currently set up with minimal file references
3. **Add source files manually** via Visual Studio:
   - Right-click project → Add → Existing Item
   - Navigate to `../ client/`, `../common/`, `../d3d/`, `../gl/`, `../qclib/`, `../server/`
   - Select `.c` files (excluding platform-specific files like sys_win.c, sys_linux.c, etc.)
   - **Important**: Make sure to include `sys_uwp.c` from `../client/`

4. Or use the reference from `../xdk/FTEQW_XDK.vcproj` to see which files are needed

5. Select configuration:
   - **x64 | Debug** - For debugging on PC or Xbox
   - **x64 | Release** - For release builds on PC/Xbox One X/Series
   - **ARM64 | Release** - For native Xbox Series builds (recommended for Series S/X)

6. Build (Ctrl+Shift+B)

### Option 2: Using a Script to Populate Project

We've provided a helper script to auto-populate the project with source files:

#### Windows PowerShell:
```powershell
cd engine\uwp
.\populate_project.ps1
```

#### Linux/macOS (for cross-compilation setup):
```bash
cd engine/uwp
./populate_project.sh
```

These scripts will:
- Scan the required directories
- Generate a complete list of source files
- Update the .vcxproj file
- Exclude platform-specific files (sys_win.c, sys_linux.c, etc.)

### Files to Include

Based on the XDK project, you need files from these directories:

**Client** (`../client/`):
- sys_uwp.c (UWP platform implementation)
- cl_*.c (client logic)
- console.c, keys.c, menu.c, etc.
- snd_*.c (sound system)
- r_*.c (renderer interface)
- pr_*.c (QuakeC VM)
- in_generic.c (input - NOT in_win.c)
- cd_null.c (CD audio stub)

**Common** (`../common/`):
- ALL .c files except:
  - net_ssl_*.c (platform-specific SSL, may need adjustments)
  - fs_win32.c (use generic filesystem)

**D3D** (`../d3d/`):
- d3d11_*.c (D3D11 renderer)
- vid_d3d11.c (video driver)
- NOT d3d8*.c or d3d9*.c

**GL** (`../gl/`):
- gl_*.c (renderer backend)

**HTTP** (`../http/`):
- httpclient.c (if not disabled)

**QCLib** (`../qclib/`):
- ALL .c files (QuakeC compiler/VM)

**Server** (`../server/`):
- ALL .c files

### Excluded Platform Files

**DO NOT include these platform-specific files:**
- sys_win.c, sys_linux.c, sys_sdl.c, sys_xdk.c, sys_axfte.cpp, sys_droid.c, etc.
- Use **sys_uwp.c** instead
- in_win.c (use in_generic.c)
- cd_win.c, cd_linux.c (use cd_null.c)
- snd_win.c, snd_linux.c (use generic sound or UWP-specific if needed)

## Build Requirements

### Software
- Windows 10/11 SDK (10.0.17763.0 or later)
- Visual Studio 2019 or 2022 with:
  - Universal Windows Platform development workload
  - C++ build tools
  - Windows 10/11 SDK

### Target Platforms
- **x64**: Windows 10/11 PC, Xbox One, Xbox One S, Xbox One X, Xbox Series S, Xbox Series X
- **ARM64**: Xbox Series S, Xbox Series X (native), Windows on ARM

## Configuration Details

### Preprocessor Defines
- `WINRT` - Enables UWP/Xbox code paths
- `D3D11QUAKE` - Uses D3D11 renderer
- `_DEBUG` or `NDEBUG` - Debug/Release mode

### Runtime Library
- **CRITICAL**: Must use `/MD` (Release) or `/MDd` (Debug)
- This is **required** for UWP
- `/MT` will cause runtime errors

### Include Directories
```
..\qclib
..\server
..\gl
..\common
..\client
..\d3d
```

### Libraries
```
d3d11.lib
dxgi.lib
xinput.lib
```

## Deployment

### To Xbox Dev Mode

1. Enable Developer Mode on Xbox
2. Configure Visual Studio remote debugging:
   - Project Properties → Debugging
   - Remote Machine: `<Xbox IP>`
   - Authentication: Universal (Unencrypted Protocol)
3. Press F5 to build, deploy, and debug

### To PC (Local UWP)

1. Enable Developer Mode in Windows Settings
2. Press F5 to build and run locally

## Troubleshooting

### "Cannot find source files"
- The project needs source files added
- Use Visual Studio to add files manually
- Or run the populate_project script

### "VCRUNTIME140.dll not found"
- Check Runtime Library setting is `/MD` or `/MDd`
- Should be linking against VCRUNTIME140_APP.dll (UWP version)
- Use `dumpbin /DEPENDENTS FTEQW_UWP.exe` to verify

### "D3D11 errors"
- Ensure D3D11QUAKE is defined
- Check that d3d11.lib and dxgi.lib are linked
- Verify Windows SDK is installed

### "XInput not found"
- Ensure xinput.lib is linked
- Check that xinput.h is available in SDK

## Next Steps

After building:

1. **Test on PC** first (UWP mode)
2. **Deploy to Xbox** via Visual Studio
3. **Upload game data** via Device Portal to LocalState folder
4. **Test controllers** using XInput
5. **Check logs** via Device Portal

## Additional Resources

- See `README.md` for usage instructions
- See `UWP_PORTING_GUIDE.md` for detailed porting info
- See `IMPLEMENTATION_SUMMARY.md` for technical details
- Reference `../xdk/FTEQW_XDK.vcproj` for complete file list

## CMake Alternative

For CMake users, configure with:

```bash
cmake -S ../.. -B build -G "Visual Studio 17 2022" \
  -DCMAKE_SYSTEM_NAME=WindowsStore \
  -DCMAKE_SYSTEM_VERSION=10.0 \
  -DWINRT=ON \
  -DD3D11QUAKE=ON
```

Then build:
```bash
cmake --build build --config Release
```

Note: CMake support for this port is experimental. Visual Studio solution is recommended.
