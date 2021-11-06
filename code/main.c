
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

#include <stddef.h>

typedef uintptr_t ptr;
typedef size_t size;
typedef size_t index;

#define global static

#include <stdbool.h>

#define alignas _Alignas

#define static_assert _Static_assert

#if defined(_DEBUG)
  #define assert(x) do{if(!(x)){__debugbreak();}}while(0)
#else
  #define assert(x) do{(void)sizeof(x);}while(0)
#endif

#define GAME_NAME "Bergbaukunst"
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SCALE 1
#define BYTES_PER_PIXEL 1
#define SERVER_UPDATES_PER_SECOND 20
#define CLIENT_UPDATES_PER_SECOND 100

int _fltused = 0;

#pragma function(memset)
void *memset(void *dest, int c, size_t count)
{
    char *bytes = (char *)dest;
    while (count--) { *bytes++ = (char)c; }
    return dest;
}

#pragma function(memcpy)
void *memcpy(void *dest, const void *src, size_t count)
{
    char *dest8 = (char *)dest;
    const char *src8 = (const char *)src;
    while (count--) { *dest8++ = *src8++; }
    return dest;
}

#pragma function(memcmp)
int memcmp(const void *s1, const void *s2, size_t count)
{
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    for (; count--; p1++, p2++){ if (*p1 != *p2) return *p1 - *p2; }
    return 0;
}

#pragma function(strcmp)
int strcmp(const char* s1, const char* s2)
{
    while(*s1 && (*s1 == *s2)) { s1++; s2++; }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

__declspec(restrict) void * malloc(size_t);
__declspec(restrict) void * realloc(void *, size_t);
void free(void *);

//
// SYNC
//

typedef struct event
{
    void *Handle;
} event;

event Event_Create(void);
void Event_Destroy(event *);
void Event_Wait(event);
void Event_Signal(event);
void Event_WaitAll(const event *, u32);
u32 Event_WaitOne(const event *, u32);

//
// SERVER
//

#include "string.c"
#include "math.c"
#include "hash.c"
#include "rng.c"
#include "noise.c"
#include "box.c"
#include "block.c"
#include "chunk.c"
#include "entity.c"
#include "phys.c"
#include "message.c"
#include "network.c"
#include "server.c"

//
// CLIENT
//

struct bitmap Win32_LoadBitmap(const char*);
void Win32_DestroyBitmap(struct bitmap);
struct palette Win32_LoadPalette(const char *);
void Win32_SetPalette(const struct palette *);

global volatile bool GlobalRunning;
global volatile bool GlobalFocus;

#include "audio.c"
#include "draw.c"
#include "camera.c"
#include "mesh.c"
#include "view.c"
#include "client.c"

//
// WINDOWS HEADERS
//

#define _AMD64_
#include <windef.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <memoryapi.h>
#include <winuser.h>
#include <wingdi.h>
#include <dsound.h>

global bitmap GlobalBackbuffer;
global WINDOWPLACEMENT GlobalWindowPosition;

//
// C STD LIB
//
__declspec(restrict)
void *malloc(size_t size)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

__declspec(restrict)
void *realloc(void *ptr, size_t size)
{
    if (!ptr) return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    return HeapReAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ptr, size);
}

void free(void *ptr)
{
    if (!ptr) return;
    HeapFree(GetProcessHeap(), 0, ptr);
}

//
// SYNC
//

event Event_Create(void)
{
    return (event) {
        .Handle = CreateEventA(0, false, false, 0),
    };
}

void Event_Destroy(event *Event)
{
    CloseHandle(Event->Handle);
    Event->Handle = 0;
}

void Event_Wait(event Event)
{
    WaitForSingleObject(Event.Handle, INFINITE);
}

void Event_Signal(event Event)
{
    SetEvent(Event.Handle);
}

void Event_WaitAll(const event *Events, u32 Count)
{
    WaitForMultipleObjects(Count, (const HANDLE *)Events, true, INFINITE);
}

u32 Event_WaitOne(const event *Events, u32 Count)
{
    return WaitForMultipleObjects(Count, (const HANDLE *)Events, false, INFINITE);
}

//
// GUI
//

typedef struct {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFO_AND_PALETTE;

global HWND GlobalWindow;
global BITMAPINFO_AND_PALETTE GlobalBackbufferInfo;

ivec2 Win32_GetWindowDimension(void)
{
    RECT ClientRect;
    GetClientRect(GlobalWindow, &ClientRect);
    return (ivec2) { ClientRect.right - ClientRect.left, ClientRect.bottom - ClientRect.top };
}

void Win32_DisplayBitmap(HDC DeviceContext)
{
    ivec2 Dim = Win32_GetWindowDimension();
    StretchDIBits(DeviceContext,
                  0, 0, Dim.x, Dim.y,
                  0, 0, GlobalBackbuffer.Width, GlobalBackbuffer.Height,
                  GlobalBackbuffer.Pixels,
                  (BITMAPINFO *)&GlobalBackbufferInfo,
                  DIB_RGB_COLORS, SRCCOPY);
}

bitmap Win32_LoadBitmap(const char* Name)
{
    HBITMAP hImage = LoadImageA(GetModuleHandle(0), Name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (!hImage) return (bitmap){ 0 };

    BITMAP Image;
    GetObject(hImage, sizeof(BITMAP), &Image);
    bitmap Bitmap = Bitmap_Create(Image.bmWidth, Image.bmHeight);

    BITMAPINFO_AND_PALETTE BitmapInfo = {
        .bmiHeader = {
            .biSize = sizeof(BITMAPINFOHEADER),
            .biWidth = Image.bmWidth,
            .biHeight = Image.bmHeight,
            .biPlanes = 1,
            .biBitCount = BYTES_PER_PIXEL << 3,
            .biCompression = BI_RGB
        }
    };

    HDC DeviceContext = GetDC(GlobalWindow);
    GetDIBits(DeviceContext, hImage, 0, Bitmap.Height, Bitmap.Pixels, (BITMAPINFO *)&BitmapInfo, DIB_RGB_COLORS);
    ReleaseDC(GlobalWindow, DeviceContext);
    DeleteObject(hImage);

    return Bitmap;
}

void Win32_SetPalette(const palette *Palette)
{
    memcpy(GlobalBackbufferInfo.bmiColors, Palette->Colors, sizeof(RGBQUAD) * 256);
}

palette Win32_LoadPalette(const char* Name)
{
    palette Palette = { 0 };

    HBITMAP hImage = LoadImageA(GetModuleHandle(0), Name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (!hImage) return Palette;

    BITMAP Image;
    GetObject(hImage, sizeof(BITMAP), &Image);

    if (Image.bmWidth == 16 && Image.bmHeight == 16)
    {
        BITMAPINFO_AND_PALETTE BitmapInfo = {
            .bmiHeader = {
                .biSize = sizeof(BITMAPINFOHEADER),
                .biWidth = 16,
                .biHeight = 16,
                .biPlanes = 1,
                .biBitCount = BYTES_PER_PIXEL << 3,
                .biCompression = BI_RGB
            }
        };

        u8 Bitmap[256];
        HDC DeviceContext = GetDC(GlobalWindow);
        GetDIBits(DeviceContext, hImage, 0, 16, Bitmap, (BITMAPINFO*)&BitmapInfo, DIB_RGB_COLORS);
        ReleaseDC(GlobalWindow, DeviceContext);
        memcpy(Palette.Colors, BitmapInfo.bmiColors, sizeof(RGBQUAD) * 256);
    }
    
    DeleteObject(hImage);

    return Palette;
}

static void
Win32_ToggleFullscreen(HWND Window)
{
    //see https://devblogs.microsoft.com/oldnewthing/20100412-00/?p=14353
    DWORD Style = GetWindowLong(Window, GWL_STYLE);
    if (Style & WS_OVERLAPPEDWINDOW)
    {
        MONITORINFO MonitorInfo = {sizeof(MonitorInfo)};
        if (GetWindowPlacement(Window, &GlobalWindowPosition) && 
            GetMonitorInfo(MonitorFromWindow(Window, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
        {
            SetWindowLong(Window, GWL_STYLE, Style & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(Window, HWND_TOP,
                         MonitorInfo.rcMonitor.left, MonitorInfo.rcMonitor.top,
                         MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left,
                         MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top,
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(Window, GWL_STYLE, Style | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(Window, &GlobalWindowPosition);
        SetWindowPos(Window, 0, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

//
// AUDIO
//

#define COBJMACROS
#include <mmdeviceapi.h>
#include <audioclient.h>
global HANDLE GlobalAudioEvent;
global IMMDeviceEnumerator *GlobalIMMDeviceEnumerator;
global IAudioClient *GlobalAudioClient;
global IAudioRenderClient *GlobalAudioRenderClient;

void Win32_AudioPlay(void)
{
    if (GlobalAudioClient)
        IAudioClient_Start(GlobalAudioClient);
}

void Win32_AudioPause(void)
{
    if (GlobalAudioClient)
        IAudioClient_Stop(GlobalAudioClient);
}

void Win32_InitAudio(void)
{
    const CLSID CLSID_MMDeviceEnumerator_ = { 0xbcde0395, 0xe52f, 0x467c,{ 0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e } };
    const IID IID_IMMDeviceEnumerator_ = { 0xa95664d2, 0x9614, 0x4f35,{ 0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6 } };
    const IID IID_IAudioClient_ = { 0x1cb9ad4c, 0xdbfa, 0x4c32,{ 0xb1, 0x78, 0xc2, 0xf5, 0x68, 0xa7, 0x03, 0xb2 } };
    const IID IID_IAudioRenderClient_ = { 0xf294acfc, 0x3146, 0x4483,{ 0xa7, 0xbf, 0xad, 0xdc, 0xa7, 0xc2, 0x60, 0xe2 } };

    HRESULT Result = CoInitialize(0);
    if (FAILED(Result)) return; // TODO: Diagnostic

    Result = CoCreateInstance(&CLSID_MMDeviceEnumerator_, 0, CLSCTX_INPROC_SERVER, &IID_IMMDeviceEnumerator_, (LPVOID *)&GlobalIMMDeviceEnumerator);
    if (FAILED(Result)) return; // TODO: Diagnostic

    IMMDevice *Device;
    Result = IMMDeviceEnumerator_GetDefaultAudioEndpoint(GlobalIMMDeviceEnumerator, eRender, eMultimedia, &Device);
    if (FAILED(Result)) return; // TODO: Diagnostic

    Result = IMMDevice_Activate(Device, &IID_IAudioClient_, CLSCTX_ALL, 0, (LPVOID *)&GlobalAudioClient);
    IMMDevice_Release(Device);
    if (FAILED(Result)) return; // TODO: Diagnostic

    WAVEFORMATEX *WaveFormat;
    Result = IAudioClient_GetMixFormat(GlobalAudioClient, &WaveFormat);
    if (FAILED(Result)) return; // TODO: Diagnostic

    REFERENCE_TIME DefaultPeriod;
    Result = IAudioClient_GetDevicePeriod(GlobalAudioClient, &DefaultPeriod, 0);
    if (FAILED(Result)) return; // TODO: Diagnostic

    DWORD StreamFlags = AUDCLNT_STREAMFLAGS_EVENTCALLBACK;
    if ((WaveFormat->wFormatTag != WAVE_FORMAT_PCM) ||
        (WaveFormat->nChannels != 2) ||
        (WaveFormat->nSamplesPerSec != AUDIO_SAMPLES_PER_SECOND) ||
        (WaveFormat->wBitsPerSample != 16) ||
        (WaveFormat->cbSize != 0))
    {
        StreamFlags |= (AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM | AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);

        WaveFormat->wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat->nChannels = 2;
        WaveFormat->nSamplesPerSec = AUDIO_SAMPLES_PER_SECOND;
        WaveFormat->wBitsPerSample = 16;
        WaveFormat->nBlockAlign = (WaveFormat->nChannels * WaveFormat->wBitsPerSample) / 8;
        WaveFormat->nAvgBytesPerSec = WaveFormat->nSamplesPerSec * WaveFormat->nBlockAlign;
        WaveFormat->cbSize = 0;
    }

    Result = IAudioClient_Initialize(GlobalAudioClient, AUDCLNT_SHAREMODE_SHARED, StreamFlags, 0, 0, WaveFormat, 0);
    if (FAILED(Result)) return; // TODO: Diagnostic

    GlobalAudioEvent = CreateEventA(0, false, false, 0);
    Result = IAudioClient_SetEventHandle(GlobalAudioClient, GlobalAudioEvent);
    if (FAILED(Result)) return; // TODO: Diagnostic

    Result = IAudioClient_GetService(GlobalAudioClient, &IID_IAudioRenderClient_, (LPVOID *)&GlobalAudioRenderClient);
    if (FAILED(Result)) return; // TODO: Diagnostic

    Result = IAudioClient_Start(GlobalAudioClient);
    if (FAILED(Result)) return; // TODO: Diagnostic
}

void Win32_DestroyAudio(void)
{
    IAudioRenderClient_Release(GlobalAudioRenderClient);
    GlobalAudioRenderClient = 0;

    IAudioClient_Release(GlobalAudioClient);
    GlobalAudioClient = 0;

    CloseHandle(GlobalAudioEvent);
    GlobalAudioEvent = 0;

    IMMDeviceEnumerator_Release(GlobalIMMDeviceEnumerator);
    GlobalIMMDeviceEnumerator = 0;

    CoUninitialize();
}

typedef HANDLE (*av_set_mm_thread_characteristics_a)(LPCSTR,LPDWORD);
typedef BOOL (*av_revert_mm_thread_characteristics)(HANDLE);

DWORD Win32_AudioThreadProc(LPVOID Parameter)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    HMODULE avrtdll = LoadLibraryA("avrt.dll");
    av_set_mm_thread_characteristics_a AvSetMmThreadCharacteristicsA = 0;
    av_revert_mm_thread_characteristics AvRevertMmThreadCharacteristics = 0;
    if (avrtdll) {
        AvSetMmThreadCharacteristicsA = (av_set_mm_thread_characteristics_a)GetProcAddress(avrtdll, "AvSetMmThreadCharacteristicsA");
        AvRevertMmThreadCharacteristics = (av_revert_mm_thread_characteristics)GetProcAddress(avrtdll, "AvRevertMmThreadCharacteristics");
    }

    DWORD Index = 0;
    HANDLE Task = 0;
    if (AvSetMmThreadCharacteristicsA)
        Task = AvSetMmThreadCharacteristicsA("Pro Audio", &Index);

    HRESULT Result = CoInitialize(0);
    if (FAILED(Result)) return 1; // TODO: Diagnostic

    u32 BufferSize;
    Result = IAudioClient_GetBufferSize(GlobalAudioClient, &BufferSize);
    if (FAILED(Result)) return 1; // TODO: Diagnostic

    while (GlobalRunning)
    {
        DWORD WaitResult = WaitForSingleObject(GlobalAudioEvent, 200);
        if (WaitResult != WAIT_OBJECT_0) continue;

        u32 Padding;
        Result = IAudioClient_GetCurrentPadding(GlobalAudioClient, &Padding);
        if (FAILED(Result)) break; // TODO: Diagnostic

        u8 *SampleBuffer;
        u32 SampleCount = BufferSize - Padding;
        IAudioRenderClient_GetBuffer(GlobalAudioRenderClient, SampleCount, &SampleBuffer);
        Audio_WriteSamples((i16 *)SampleBuffer, SampleCount);
        IAudioRenderClient_ReleaseBuffer(GlobalAudioRenderClient, SampleCount, 0);
    }

    CoUninitialize();

    if (avrtdll)
    {
        if (Task && AvRevertMmThreadCharacteristics)
            AvRevertMmThreadCharacteristics(Task);
        FreeLibrary(avrtdll);
    }

    return 0;
}

//
// TIME
//

u64 Win32_GetTime(void)
{    
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);
    return Result.QuadPart;
}

u64 Win32_TimeSince(u64 Counter)
{    
    u64 Result = Win32_GetTime() - Counter;
    return Result;
}

#include <winternl.h>
typedef NTSYSAPI NTSTATUS (NTAPI *nt_query_timer_resolution) (PULONG, PULONG, PULONG);
typedef NTSYSAPI NTSTATUS (NTAPI *nt_set_timer_resolution) (ULONG, BOOLEAN, PULONG);

void Win32_HighResolutionTimer(bool Set)
{
    HMODULE ntdll = LoadLibraryA("ntdll.dll");
    if (!ntdll) return;

    nt_query_timer_resolution NtQueryTimerResolution = (nt_query_timer_resolution)GetProcAddress(ntdll, "NtQueryTimerResolution");
    nt_set_timer_resolution NtSetTimerResolution = (nt_set_timer_resolution)GetProcAddress(ntdll, "NtSetTimerResolution");

    ULONG Minimum, Maximum, Current;
    NtQueryTimerResolution(&Minimum, &Maximum, &Current);
    NtSetTimerResolution(Maximum, Set, &Current);

    FreeLibrary(ntdll);
}

//
// INPUT
//

void Win32_LockMouse(bool Lock)
{
    if (Lock)
    {
        RECT ClientRect;
        GetClientRect(GlobalWindow, &ClientRect);
        MapWindowPoints(GlobalWindow, HWND_DESKTOP, (POINT *)&ClientRect, 2);
        ClipCursor(&ClientRect);
        ShowCursor(false);
    }
    else
    {
        ClipCursor(0);
        ShowCursor(true);
    }
}

LRESULT CALLBACK Win32_WindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            Win32_DisplayBitmap(DeviceContext);
            EndPaint(Window, &Paint);
        } break;

        case WM_GETMINMAXINFO:
        {
            RECT MinWindowDim = {
                .left = 0,
                .top = 0,
                .right = SCREEN_WIDTH,
                .bottom = SCREEN_HEIGHT
            };
            DWORD Style = GetWindowLong(Window, GWL_STYLE);
            AdjustWindowRect(&MinWindowDim, Style, 0);
            MINMAXINFO *MinMaxInfo = (MINMAXINFO *)LParam;
            MinMaxInfo->ptMinTrackSize.x = MinWindowDim.right - MinWindowDim.left;
            MinMaxInfo->ptMinTrackSize.y = MinWindowDim.bottom - MinWindowDim.top;
        } break;

        case WM_ENTERSIZEMOVE:
        case WM_KILLFOCUS:
        {
            Win32_LockMouse(false);
            GlobalFocus = false;
            Win32_AudioPause();
        } break;

        case WM_EXITSIZEMOVE:
        case WM_SETFOCUS:
        {
            Win32_LockMouse(true);
            GlobalFocus = true;
            Win32_AudioPlay();
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

//
//
//

int Win32_ClientMain(const char *Ip)
{
    HANDLE hInstance = GetModuleHandle(0);
    const char *ClassName = GAME_NAME "Class";
    if(!RegisterClassA(&(WNDCLASSA) {
            .style         = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc   = Win32_WindowCallback,
            .hInstance     = hInstance,
            .hCursor       = LoadCursor(0, IDC_ARROW),
            .hIcon         = LoadIcon(hInstance, "APP_ICON"),
            .lpszClassName = ClassName }))
        return 1;

    // WINDOW
    RECT WindowDim = { .right = SCREEN_WIDTH * SCREEN_SCALE,
                       .bottom = SCREEN_HEIGHT * SCREEN_SCALE };
    AdjustWindowRect(&WindowDim, WS_OVERLAPPEDWINDOW, 0);
    GlobalWindow = CreateWindowExA(0, ClassName,
                                   GAME_NAME,
                                   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                   CW_USEDEFAULT, CW_USEDEFAULT,
                                   WindowDim.right - WindowDim.left,
                                   WindowDim.bottom - WindowDim.top,
                                   0, 0, hInstance, 0);
    if(!GlobalWindow) return 1;

    // BACKBUFFER
    GlobalBackbuffer = Bitmap_Create(SCREEN_WIDTH, SCREEN_HEIGHT);
    GlobalBackbufferInfo = (BITMAPINFO_AND_PALETTE){
        .bmiHeader = {
            .biSize = sizeof(BITMAPINFOHEADER),
            .biWidth = GlobalBackbuffer.Width,
            .biHeight = GlobalBackbuffer.Height,
            .biPlanes = 1,
            .biBitCount = BYTES_PER_PIXEL << 3,
            .biCompression = BI_RGB
        }
    };

    // AUDIO
    Audio_Init();
    Win32_InitAudio();
    HANDLE AudioThread = CreateThread(0, 0,  Win32_AudioThreadProc, 0, 0, 0);
    
    // GRAPHICS
#if (RASTERIZER_TILE_COUNT > 1)
    Rasterizer_Init();
    DWORD RasterizerThreadIDs[RASTERIZER_TILE_COUNT-1];
    HANDLE RasterizerThreads[RASTERIZER_TILE_COUNT-1];
    for (u64 i = 1; i < RASTERIZER_TILE_COUNT; ++i)
    {
        RasterizerThreads[i] = CreateThread(0, 0,  Rasterizer_TileThreadProc, (void*)i, 0, &RasterizerThreadIDs[i-1]);
        SetThreadPriority(RasterizerThreads[i-1], THREAD_PRIORITY_TIME_CRITICAL);
    }
#endif

    // TIMING
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    Win32_HighResolutionTimer(true);
    LARGE_INTEGER PerfCountFrequency;
    QueryPerformanceFrequency(&PerfCountFrequency);
    u64 TimePerSecond = PerfCountFrequency.QuadPart;
    u64 TimePerUpdate = TimePerSecond / CLIENT_UPDATES_PER_SECOND;
    u64 LastTime = Win32_GetTime();
    u64 LastInputTime = LastTime;

    // GAME STATE
    input Input = { 0 };
    client *Client = malloc(sizeof(client));
    Client_Init(Client, Ip);

    // RAW INPUT
    Win32_LockMouse(true);
    RAWINPUTDEVICE Rid[1];
    Rid[0].usUsagePage = 1;
    Rid[0].usUsage = 2; 
    Rid[0].dwFlags = 0;//RIDEV_INPUTSINK;
    Rid[0].hwndTarget = GlobalWindow;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    // GAME LOOP
    GlobalRunning = true;
    while (GlobalRunning)
    {
        Input.Look = (vec2){ 0 };

        MSG Message;
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            switch (Message.message)
            {
                case WM_QUIT:
                {
                    GlobalRunning = false;
                } break;

                case WM_LBUTTONDOWN: { Input.Punch = true; } break;
                case WM_RBUTTONDOWN: { Input.Use = true; } break;
                case WM_MBUTTONDOWN: break;

                case WM_LBUTTONUP: { Input.Punch = false; } break;
                case WM_RBUTTONUP: { Input.Use = false; } break;
                case WM_MBUTTONUP: break;

                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    WPARAM Code = Message.wParam;
                    bool WasDown = (((Message.lParam >> 30) & 1) != 0);
                    bool IsDown = (((Message.lParam >> 31) & 1) == 0);
                    bool AltDown = (Message.lParam & (1 << 29)) != 0;
                    
                    if (IsDown && AltDown)
                    {
                        if (Code == VK_F4) GlobalRunning = false;
                        if (Code == VK_RETURN) Win32_ToggleFullscreen(GlobalWindow);
                    }

                    if (IsDown && !WasDown)
                    {
                        if (Code == VK_F1) Client->NoClip = !Client->NoClip;
                        if (Code == VK_F2) Client->Hitboxes = !Client->Hitboxes;
                    }

                    if (WasDown != IsDown)
                    {
                        switch (Code)
                        {
                            case 'W': Input.MoveForward = IsDown; break;
                            case 'A': Input.MoveLeft = IsDown; break;
                            case 'S': Input.MoveBack = IsDown; break;
                            case 'D': Input.MoveRight = IsDown; break;

                            case VK_UP:    Input.LookUp = IsDown; break;
                            case VK_LEFT:  Input.LookLeft = IsDown; break;
                            case VK_DOWN:  Input.LookDown = IsDown; break;
                            case VK_RIGHT: Input.LookRight = IsDown; break;

                            case VK_SPACE: Input.Jump = IsDown; break;
                            case VK_SHIFT: Input.Crouch = IsDown; break;
                        }
                    }
                } break;

                case WM_MOUSEMOVE:
                {
                    ivec2 Dim = Win32_GetWindowDimension();
                    i16 xx = Message.lParam & 0xFFFF;
                    i16 yy = (Message.lParam >> 16) & 0xFFFF;
                    i32 x = (xx * GlobalBackbuffer.Width / Dim.x);
                    i32 y = (yy * GlobalBackbuffer.Height / Dim.y);
                    y = GlobalBackbuffer.Height - y - 1;
                    Input.Mouse = (ivec2){ x, y };
                } break;

                case WM_INPUT:
                {
                    UINT dwSize = 48;
                    u8 lpb[48];

                    GetRawInputData((HRAWINPUT)Message.lParam, RID_INPUT, 
                                    lpb, &dwSize, sizeof(RAWINPUTHEADER));
                    RAWINPUT* raw = (RAWINPUT*)lpb;
                    if (raw->header.dwType == RIM_TYPEMOUSE) 
                    {
                        Input.Look.x += raw->data.mouse.lLastX;
                        Input.Look.y -= raw->data.mouse.lLastY;
                    }
                } break;

                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                } break;
            }
        }

        // if (GlobalFocus)
        {
            // INPUT
            u64 InputTimeElapsed = Win32_TimeSince(LastInputTime);
            LastInputTime += InputTimeElapsed;
            Client_Input(Client, Input, (f32)InputTimeElapsed / TimePerSecond);

            // UPDATE
            u64 TimeElapsed = Win32_TimeSince(LastTime);
            while (TimeElapsed >= TimePerUpdate)
            {
                TimeElapsed -= TimePerUpdate;
                LastTime += TimePerUpdate;
                Client_Update(Client, Input, (f32)TimePerUpdate / TimePerSecond);
            }

            // DRAW
            Client_Draw(Client, GlobalBackbuffer, (f32)TimeElapsed / TimePerSecond);
            
            // BLIT
            HDC DeviceContext = GetDC(GlobalWindow);
            Win32_DisplayBitmap(DeviceContext);
            ReleaseDC(GlobalWindow, DeviceContext);

            // SLEEP
            // Sleep(1);
        }
    }


    Win32_DestroyAudio();
    free(Client);

    return 0;
}

int Win32_ServerMain(void)
{
    // TIMING
    LARGE_INTEGER PerfCountFrequency;
    QueryPerformanceFrequency(&PerfCountFrequency);
    u64 TimePerSecond = PerfCountFrequency.QuadPart;
    u64 TimePerUpdate = TimePerSecond / SERVER_UPDATES_PER_SECOND;
    u64 LastTime = Win32_GetTime();
    u64 LastInputTime = LastTime;

    server Server = { 0 };
    Server_Init(&Server);

    while (GlobalRunning)
    {
        Server_Update(&Server);

        // TIMING
        u64 TimeElapsed = Win32_TimeSince(LastTime);
        if (TimeElapsed < TimePerUpdate)
        {
            // Sleep((DWORD)(1000 * (Target - TimeElapsed) / PerfCountFrequency.QuadPart));
            Sleep(1);
            while (TimeElapsed < TimePerUpdate)
                TimeElapsed = Win32_TimeSince(LastTime);
        }
        LastTime += TimeElapsed;
    }

    return 0;
}

DWORD Win32_ServerMainThreadProc(LPVOID Parameter)
{
    return Win32_ServerMain();
}

int WinStartUp(void)
{
    int ExitCode = 0;
    GlobalRunning = true;
    Network_Init();

    string CmdLine = String_FromCString(GetCommandLineA());
    /* string Executable = */ String_ExtractToken(&CmdLine);
    string Option = String_ExtractToken(&CmdLine);

    if (String_Equal(Option, STRING("-client")))
    {
        // string Ip = String_ExtractToken(&CmdLine);
        // string Port = String_ExtractToken(&CmdLine);
        ExitCode = Win32_ClientMain(CmdLine.Data);
    }
    else if (String_Equal(Option, STRING("-server")))
    {
        // string Port = String_ExtractToken(&CmdLine);
        ExitCode = Win32_ServerMain();
    }
    else
    {
        GlobalRunning = true;

        // STARTUPINFO StartupInfo = { 0 };
        // PROCESS_INFORMATION ProcessInformation = { 0 };
        // CreateProcessA("bergbaukunst.exe", "-server", 0, 0, FALSE, 0, 0, 0, &StartupInfo, &ProcessInformation);
        // WaitForInputIdle(ProcessInformation.hProcess, INFINITE);
        // CloseHandle(ProcessInformation.hThread);
        
        DWORD ServerThreadID;
        HANDLE ServerThread = CreateThread(0, 0,  Win32_ServerMainThreadProc, 0, 0, &ServerThreadID);

        ExitCode = Win32_ClientMain("localhost");

        // TerminateProcess(ProcessInformation.hProcess, 0);
        // WaitForSingleObject(ProcessInformation.hProcess, INFINITE);
    }

    Network_Destroy();
    return ExitCode;
}

void WinMainCRTStartup(void)
{
    ExitProcess(WinStartUp());
}

void mainCRTStartup(void)
{
    ExitProcess(WinStartUp());
}