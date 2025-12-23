#include "quakedef.h"
#ifdef WINRT

#include <windows.h>
#include <xinput.h>

#define MAXPRINTMSG 1024
qboolean isDedicated = false;

// XInput for Xbox controller support (UWP-compatible)
#define MAX_XINPUT_CONTROLLERS 4
static XINPUT_STATE g_xinputStates[MAX_XINPUT_CONTROLLERS];
static qboolean g_xinputConnected[MAX_XINPUT_CONTROLLERS];
static DWORD g_xinputPacketNumbers[MAX_XINPUT_CONTROLLERS];

/*Timers - UWP supports QueryPerformanceCounter*/
double Sys_DoubleTime(void)
{
	static int first = 1;
	static LARGE_INTEGER qpcfreq;
	LARGE_INTEGER PerformanceCount;
	static LONGLONG oldcall;
	static LONGLONG firsttime;
	LONGLONG diff;

	QueryPerformanceCounter(&PerformanceCount);
	if (first)
	{
		first = 0;
		QueryPerformanceFrequency(&qpcfreq);
		firsttime = PerformanceCount.QuadPart;
		diff = 0;
	}
	else
		diff = PerformanceCount.QuadPart - oldcall;
	if (diff >= 0)
		oldcall = PerformanceCount.QuadPart;
	return (oldcall - firsttime) / (double)qpcfreq.QuadPart;
}

unsigned int Sys_Milliseconds(void)
{
	return Sys_DoubleTime() * 1000;
}

NORETURN void VARGS Sys_Error(const char *error, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start(argptr, error);
	vsnprintf(msg, sizeof(msg) - 1, error, argptr);
	msg[sizeof(msg) - 1] = 0;
	va_end(argptr);

	COM_WorkerAbort(msg);

	// On UWP, we should terminate cleanly
	OutputDebugStringA("FTEQW Error: ");
	OutputDebugStringA(msg);
	OutputDebugStringA("\n");

	// Infinite loop for debugging
	for (;;)
		;
}

void Sys_Sleep(double seconds)
{
	// Yields to other processes/threads
	Sleep((DWORD)(seconds * 1000));
}

void Sys_Quit(void)
{
	Host_Shutdown();
	// On UWP, apps should use CoreApplication::Exit() but we can't call it from C
	// The main loop should handle exiting gracefully
}

void Sys_Shutdown(void)
{
	// Clean up XInput (no special cleanup needed)
	int i;
	for (i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		g_xinputConnected[i] = false;
	}
}

void Sys_Init(void)
{
	// Register system-specific cvars here
	// Initialize XInput state tracking
	int i;
	for (i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		g_xinputConnected[i] = false;
		g_xinputPacketNumbers[i] = 0;
		memset(&g_xinputStates[i], 0, sizeof(XINPUT_STATE));
	}
}

/*prints to debug output*/
void VARGS Sys_Printf(char *fmt, ...)
{
	va_list argptr;
	char msg[MAXPRINTMSG];

	va_start(argptr, fmt);
	vsnprintf(msg, sizeof(msg) - 1, fmt, argptr);
	msg[sizeof(msg) - 1] = 0;
	va_end(argptr);

	// Output to debug console
	OutputDebugStringA(msg);
}

/*returns the system video mode settings*/
qboolean Sys_GetDesktopParameters(int *width, int *height, int *bpp, int *refreshrate)
{
	// UWP apps typically run in a window/view controlled by the system
	// For Xbox, we should query the display capabilities
	*width = 1920;  // Default to 1080p
	*height = 1080;
	*bpp = 32;
	*refreshrate = 60;
	return true;
}

/*dedicated server type stuff - not applicable for Xbox*/
qboolean Sys_InitTerminal(void)
{
	return false;  // failure
}

void Sys_CloseTerminal(void)
{
}

char *Sys_ConsoleInput(void)
{
	// returns any text typed on stdin, when acting as a dedicated server
	return NULL;
}

/*various system notifications*/
void Sys_ServerActivity(void)
{
	// player joined the server or said something
	// Not much we can do on Xbox
}

void Sys_RecentServer(char *command, char *target, char *title, char *desc)
{
	// Could potentially use Xbox Live features here
}

/*check any system message queues here*/
void Sys_SendKeyEvents(void)
{
	// Process XInput for controller input
	int i;
	for (i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);
		
		if (result == ERROR_SUCCESS)
		{
			if (!g_xinputConnected[i])
			{
				// Controller just connected
				g_xinputConnected[i] = true;
				Con_Printf("XInput controller %d connected\n", i);
			}
			
			// Only process if packet number changed (new input)
			if (state.dwPacketNumber != g_xinputPacketNumbers[i])
			{
				g_xinputPacketNumbers[i] = state.dwPacketNumber;
				// Store state for INS_Move to process
				g_xinputStates[i] = state;
			}
		}
		else if (result == ERROR_DEVICE_NOT_CONNECTED)
		{
			if (g_xinputConnected[i])
			{
				// Controller just disconnected
				g_xinputConnected[i] = false;
				Con_Printf("XInput controller %d disconnected\n", i);
			}
		}
	}
}

qboolean Sys_RandomBytes(qbyte *string, int len)
{
	// UWP supports BCryptGenRandom for cryptographically strong random data
	// For now, return false to use fallback
	return false;
}

/*filesystem stuff - UWP has restricted filesystem access*/
void Sys_mkdir(const char *path)
{
	// UWP apps should use Windows::Storage::StorageFolder
	// For now, stub it out
}

qboolean Sys_rmdir(const char *path)
{
	// UWP filesystem restriction
	return false;
}

qboolean Sys_remove(const char *path)
{
	return false;  // false for failure
}

qboolean Sys_Rename(const char *oldfname, const char *newfname)
{
	return false;  // false for failure
}

int Sys_EnumerateFiles(const char *gpath, const char *match, int (QDECL *func)(const char *fname, qofs_t fsize, void *parm, searchpathfuncs_t *spath), void *parm, searchpathfuncs_t *spath)
{
	// UWP filesystem enumeration would use Windows::Storage APIs
	// For now, return true (no files found)
	return true;
}

/*clipboard support*/
#define SYS_CLIPBOARD_SIZE 256
static char clipboard_buffer[SYS_CLIPBOARD_SIZE] = {0};

void Sys_Clipboard_PasteText(clipboardtype_t cbt, void (*callback)(void *cb, char *utf8), void *ctx)
{
	// UWP clipboard access requires Windows::ApplicationModel::DataTransfer
	// For now, use internal buffer
	callback(ctx, clipboard_buffer);
}

void Sys_SaveClipboard(clipboardtype_t cbt, const char *text)
{
	Q_strncpyz(clipboard_buffer, text, SYS_CLIPBOARD_SIZE);
}

/*dynamic library stubs - UWP doesn't support LoadLibrary for arbitrary DLLs*/
dllhandle_t *Sys_LoadLibrary(const char *name, dllfunction_t *funcs)
{
	Con_Printf("Sys_LoadLibrary: %s (not supported on UWP)\n", name);
	return NULL;
}

void Sys_CloseLibrary(dllhandle_t *lib)
{
}

void *Sys_GetAddressForName(dllhandle_t *module, const char *exportname)
{
	return NULL;  // unsupported
}

char *Sys_GetNameForAddress(dllhandle_t *module, void *address)
{
	return NULL;  // unsupported
}

// Main entry point - Note: UWP apps typically use CoreApplication
// but for a C-based engine, we can use a traditional main
int main(int argc, char **argv)
{
	float time, lasttime;
	quakeparms_t parms;

	memset(&parms, 0, sizeof(parms));

	parms.argc = argc;
	parms.argv = argv;
#ifdef CONFIG_MANIFEST_TEXT
	parms.manifest = CONFIG_MANIFEST_TEXT;
#endif

	COM_InitArgv(parms.argc, parms.argv);
	TL_InitLanguages(parms.basedir);
	Host_Init(&parms);

	// main loop
	lasttime = Sys_DoubleTime();

	while (1)
	{
		time = Sys_DoubleTime();
		Host_Frame(time - lasttime);
		lasttime = time;
	}

	return 0;
}

/*input stubs - Xbox controller support via XInput*/
void INS_Shutdown(void)
{
	// XInput doesn't require explicit shutdown
}

void INS_ReInit(void)
{
	INS_Shutdown();
	INS_Init();
}

void INS_Move(void)
{
	// Poll XInput and update player movement
	// This would integrate with the engine's input system
	// The actual input mapping would be done here using g_xinputStates[]
}

void INS_Init(void)
{
	// XInput doesn't require explicit initialization
	Con_Printf("XInput initialized for UWP\n");
}

void INS_Accumulate(void)  // input polling
{
	// XInput state is polled in Sys_SendKeyEvents
}

void INS_Commands(void)  // joystick button events
{
	// Process button presses from XInput and send to Cbuf_AddText
	int i;
	for (i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		if (!g_xinputConnected[i])
			continue;
			
		// TODO: Map XInput buttons to console commands
		// Example: if (g_xinputStates[i].Gamepad.wButtons & XINPUT_GAMEPAD_A)
		//     Cbuf_AddText("+jump\n", RESTRICT_LOCAL);
	}
}

void INS_EnumerateDevices(void *ctx, void(*callback)(void *ctx, const char *type, const char *devicename, unsigned int *qdevid))
{
	// Enumerate XInput controllers
	int i;
	for (i = 0; i < MAX_XINPUT_CONTROLLERS; i++)
	{
		if (g_xinputConnected[i])
		{
			char devicename[64];
			unsigned int qdevid = i;
			snprintf(devicename, sizeof(devicename), "XInput Controller %d", i);
			callback(ctx, "xinput", devicename, &qdevid);
		}
	}
}

void INS_SetupControllerAudioDevices(qboolean enabled)
{
	// Xbox controllers can have audio (headsets)
	// Not implemented yet
}

void INS_UpdateGrabs(void)
{
	// Not used on console platforms
}

#endif // WINRT
