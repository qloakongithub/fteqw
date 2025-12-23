# FTEQW UWP/Xbox Dev Mode Port - Implementation Summary

## Overview

This document summarizes the implementation of UWP (Universal Windows Platform) support for FTEQW, specifically targeting Xbox Series X|S and Xbox One family consoles running in Developer Mode.

## What Was Implemented

### 1. Project Structure
- **Location**: `engine/uwp/`
- **Files Created**:
  - `FTEQW_UWP.sln` - Visual Studio solution
  - `FTEQW_UWP.vcxproj` - Project configuration
  - `Package.appxmanifest` - UWP application manifest
  - `README.md` - Build and deployment instructions
  - `UWP_PORTING_GUIDE.md` - Detailed porting guide
  - `Assets/README.md` - Asset requirements

### 2. Platform Implementation
- **File**: `engine/client/sys_uwp.c`
- **Features**:
  - High-precision timing via QueryPerformanceCounter
  - XInput-based Xbox controller support (up to 4 controllers)
  - UWP-compliant system functions
  - Debug output via OutputDebugString
  - File system stubs (sandboxed UWP storage)
  - Main loop implementation

### 3. Build Configuration
- **Platforms**: x64 (PC, Xbox One X|S, Series X|S) and ARM64 (Series S|X native)
- **Configurations**: Debug and Release
- **Runtime Library**: Multithreaded DLL (/MD, /MDd) - **Required for UWP**
- **Dependencies**: d3d11.lib, dxgi.lib, xinput.lib

### 4. Engine Configuration
- **File**: `engine/common/bothdefs.h`
- **Changes**:
  - Enhanced WINRT section with clear comments
  - Disabled networking (HAVE_TCP, HAVE_PACKET)
  - Disabled plugins (UWP LoadLibrary restriction)
  - Disabled text editor (console limitation)
  - Force D3D11QUAKE for WINRT builds
  - Disabled multithreading (UWP model differences)

## Key Technical Decisions

### Why XInput Instead of GameInput?
**GameInput is part of GDK (Gaming Device Kit), which is NOT compatible with UWP.**

UWP apps must use the older XInput API, which is still fully supported on Xbox consoles in Dev Mode.

### Why D3D11 Instead of D3D12?
The codebase already has partial WINRT support in the D3D11 renderer. D3D12 would require more extensive porting work.

### Why No Networking?
UWP uses different socket APIs (Windows.Networking.Sockets) compared to traditional Winsock. This can be added in future work but was excluded from the initial port to keep changes minimal.

### Why No Plugins?
UWP security model doesn't support LoadLibrary() for arbitrary DLLs. All functionality must be compiled into the main executable.

## Based on Community Best Practices

This implementation follows guidance from:

1. **worleydl's UWP Porting Notes** (wiki.xbdev.store/en/WorleysNotes)
   - Runtime library must be /MD (not /MT)
   - Check for VCRUNTIME140_APP.dll dependency
   - Use dumpbin and gflags for troubleshooting
   - DLL loading restrictions

2. **xbdev.store Community Resources** (wiki.xbdev.store/en/HelpfulLinksDev)
   - Xbox Dev Mode activation
   - Deployment best practices
   - Controller input handling

3. **MinGW Alternative Path**
   - Documented for cases where Visual Studio DLLs have conflicts
   - MinGW-built libraries can avoid UWP runtime issues

4. **OpenGL Support Path**
   - aerisarn's Mesa-UWP for OpenGL on UWP
   - SDL-UWP-GL for SDL integration
   - Optional, as FTEQW uses D3D11 by default

## How to Use

### For Developers

1. **Requirements**:
   - Windows 10/11 version 1809 or later
   - Visual Studio 2019 or 2022
   - Universal Windows Platform development workload
   - Windows 10 SDK (10.0.17763.0 or later)

2. **Build**:
   ```
   cd engine/uwp
   # Open FTEQW_UWP.sln in Visual Studio
   # Select platform (x64 or ARM64) and configuration (Debug or Release)
   # Build (Ctrl+Shift+B)
   ```

3. **Deploy to Xbox**:
   - Put Xbox in Developer Mode
   - Enable Device Portal on Xbox
   - Configure Visual Studio remote debugging
   - Press F5 to deploy and debug

See `engine/uwp/README.md` for detailed instructions.

### For Xbox Dev Mode Users

1. **Activate Developer Mode** on your Xbox
2. **Install FTEQW** via Visual Studio deployment
3. **Upload game data** via Device Portal to LocalState folder
4. **Launch** from Dev Home

## What's NOT Included

- **Networking**: Requires UWP socket API implementation
- **Multiplayer**: Depends on networking
- **Plugins**: Not supported by UWP
- **Dedicated Server**: Requires networking + not suitable for console
- **OpenGL**: Possible via Mesa-UWP but not included by default

## Testing Status

This implementation has been:
- ✅ Code reviewed for correctness
- ✅ Documentation verified
- ⚠️ Not yet compile-tested (requires Windows + Visual Studio)
- ⚠️ Not yet tested on Xbox (requires Xbox in Dev Mode)

## Future Enhancements

Potential future improvements:

1. **Networking Support**
   - Implement UWP socket APIs (Windows.Networking.Sockets)
   - Enable multiplayer functionality

2. **Asset Integration**
   - Create proper logo and splash screen assets
   - Package default game data

3. **Storage Integration**
   - Improve file system integration with UWP storage APIs
   - Game save management

4. **Xbox Live Integration**
   - Achievements
   - Cloud saves
   - Social features

5. **Performance Optimization**
   - Profile on actual Xbox hardware
   - Optimize for Series S (lower spec)
   - ARM64 native optimizations

## Files Modified/Added

### New Files
- `engine/uwp/FTEQW_UWP.sln`
- `engine/uwp/FTEQW_UWP.vcxproj`
- `engine/uwp/Package.appxmanifest`
- `engine/uwp/README.md`
- `engine/uwp/UWP_PORTING_GUIDE.md`
- `engine/uwp/Assets/README.md`
- `engine/client/sys_uwp.c`

### Modified Files
- `engine/common/bothdefs.h` - Enhanced WINRT section
- `.gitignore` - Added UWP build artifacts

### Total Changes
- ~1,200 lines of new code/documentation
- Minimal changes to existing codebase
- No breaking changes to other platforms

## Maintenance Notes

### When Adding New Features

If adding new features to FTEQW:

1. Check if they work under UWP restrictions
2. Guard UWP-incompatible code with `#ifndef WINRT`
3. Update `bothdefs.h` if needed
4. Test on Xbox Dev Mode if possible

### Common Issues

**Build fails with VCRUNTIME errors**:
- Check Runtime Library is set to /MD (not /MT)
- Verify all dependencies are UWP-compatible

**App crashes on Xbox**:
- Use dumpbin to check DLL dependencies
- Look for VCRUNTIME140.dll instead of VCRUNTIME140_APP.dll
- Enable loader snaps with gflags for detailed logs

**Controller not detected**:
- Verify XInput is being polled in Sys_SendKeyEvents
- Check Device Portal to see if controller is recognized
- Try different USB ports on Xbox

## License

Same as FTEQW: GPL v2

## Credits

- FTEQW Team - Original engine
- worleydl - UWP porting guidance
- xbdev.store community - Xbox Dev Mode resources
- aerisarn - Mesa-UWP and SDL-UWP-GL projects

## Contact

For UWP/Xbox-specific issues:
- Check xbdev.store wiki
- Join xbdev.store Discord community
- Refer to Microsoft UWP documentation

For FTEQW general issues:
- See main FTEQW documentation
- Use FTEQW's established support channels
