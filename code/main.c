
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
#define SCREEN_WIDTH 640 // 1920
#define SCREEN_HEIGHT 360 // 1080
#define SCREEN_SCALE 2
#define BYTES_PER_PIXEL 1

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

struct bitmap Win32_LoadBitmap(const char*);
void Win32_DeleteBitmap(struct bitmap);
void Win32_LockMouse(bool Lock);

#include "math.c"
#include "geom.c"
#include "audio.c"
#include "draw.c"
#include "camera.c"
#include "game.c"

global bool GlobalFocus;
global bool GlobalRunning;
global bitmap GlobalBackbuffer;

#define _AMD64_
#include <windef.h>
#include <libloaderapi.h>
#include <processthreadsapi.h>
#include <memoryapi.h>
#include <winuser.h>
#include <wingdi.h>
#include <dsound.h>

typedef struct {
    BITMAPINFOHEADER bmiHeader;
    RGBQUAD          bmiColors[256];
} BITMAPINFO_AND_PALETTE;

global HWND GlobalWindow;
global BITMAPINFO_AND_PALETTE GlobalBackbufferInfo;

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

typedef HRESULT WINAPI DIRECT_SOUND_CREATE(LPCGUID, LPDIRECTSOUND *, LPUNKNOWN);
global LPDIRECTSOUNDBUFFER GlobalAudioBuffer;

#define AUDIO_SAMPLES_PER_SECOND 48000
#define AUDIO_BYTES_PER_SAMPLE 4
#define AUDIO_BUFFER_SIZE ((AUDIO_SAMPLES_PER_SECOND * AUDIO_BYTES_PER_SAMPLE) / 15)

void Win32_ClearAudioBuffer(void)
{
    VOID *Region1; DWORD Region1Size;
    VOID *Region2; DWORD Region2Size;
    HRESULT Error = IDirectSoundBuffer_Lock(GlobalAudioBuffer, 0, 0,
                                            &Region1, &Region1Size,
                                            &Region2, &Region2Size,
                                            DSBLOCK_ENTIREBUFFER);
    if (FAILED(Error)) return;
    memset(Region1, 0, Region1Size);
    IDirectSoundBuffer_Unlock(GlobalAudioBuffer, Region1, Region1Size, Region2, Region2Size);
}

void Win32_InitDSound()
{
    HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");
    if (!DSoundLibrary) return; // TODO: Diagnostic

    DIRECT_SOUND_CREATE *DirectSoundCreate = (DIRECT_SOUND_CREATE *)
        GetProcAddress(DSoundLibrary, "DirectSoundCreate");

    LPDIRECTSOUND DirectSound;
    HRESULT Error = DirectSoundCreate(0, &DirectSound, 0);
    if (!DirectSoundCreate || FAILED(Error)) return; // TODO: Diagnostic

    WAVEFORMATEX WaveFormat = { 0 };
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = AUDIO_SAMPLES_PER_SECOND;
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
    WaveFormat.cbSize = 0;

    Error = IDirectSound_SetCooperativeLevel(DirectSound, GlobalWindow, DSSCL_PRIORITY);
    if (FAILED(Error)) return; // TODO: Diagnostic

    DSBUFFERDESC BufferDescription = {
        .dwSize = sizeof(DSBUFFERDESC),
        .dwFlags = DSBCAPS_PRIMARYBUFFER
    };

    LPDIRECTSOUNDBUFFER PrimaryBuffer;
    Error = IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription, &PrimaryBuffer, 0);
    if (FAILED(Error)) return; // TODO: Diagnostic

    Error = IDirectSoundBuffer_SetFormat(PrimaryBuffer, &WaveFormat);
    if (FAILED(Error)) return; // TODO: Diagnostic

    DSBUFFERDESC BufferDescription2 = {
        .dwSize = sizeof(DSBUFFERDESC),
        .dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS,
        .dwBufferBytes = AUDIO_BUFFER_SIZE,
        .lpwfxFormat = &WaveFormat
    };
    Error = IDirectSound_CreateSoundBuffer(DirectSound, &BufferDescription2, &GlobalAudioBuffer, 0);
    if (FAILED(Error)) return; // TODO: Diagnostic

    DSBCAPS Caps = { sizeof(DSBCAPS) };
    Error = IDirectSoundBuffer_GetCaps(GlobalAudioBuffer, &Caps);
    if(FAILED(Error)) return; // TODO: Diagnostic

    if (!(Caps.dwFlags & DSBCAPS_GETCURRENTPOSITION2)) return; // TODO: Diagnostic

    Win32_ClearAudioBuffer();
    IDirectSoundBuffer_Play(GlobalAudioBuffer, 0, 0, DSBPLAY_LOOPING);
}

void Win32_GatherSamples(DWORD AudioCursor)
{
    DWORD PlayCursor;
    DWORD WriteCursor;
    HRESULT Error = IDirectSoundBuffer_GetCurrentPosition(GlobalAudioBuffer, &PlayCursor, &WriteCursor);
    if (SUCCEEDED(Error))
    {
        if ((WriteCursor <= PlayCursor && (AudioCursor < WriteCursor || PlayCursor < AudioCursor)) ||
            (WriteCursor >= PlayCursor && (AudioCursor < WriteCursor && PlayCursor < AudioCursor)))
        {
            AudioCursor = WriteCursor;
            Win32_ClearAudioBuffer();
        }

        DWORD BytesToWrite;
        if (AudioCursor <= PlayCursor)
            BytesToWrite = PlayCursor - AudioCursor;
        else
            BytesToWrite = PlayCursor + (AUDIO_BUFFER_SIZE - AudioCursor);
        
        VOID *Region1; DWORD Region1Size;
        VOID *Region2; DWORD Region2Size;
        Error = IDirectSoundBuffer_Lock(GlobalAudioBuffer, 0, 0,
                                        &Region1, &Region1Size,
                                        &Region2, &Region2Size,
                                        DSBLOCK_ENTIREBUFFER);
        if (SUCCEEDED(Error))
        {
            memset(Region1, 0, Region1Size);
            Audio_WriteSamples((i16 *)Region1, Region1Size / AUDIO_BYTES_PER_SAMPLE);

            if (Region2 && Region2Size)
            {
                memset(Region2, 0, Region2Size);
                Audio_WriteSamples((i16 *)Region2, Region2Size / AUDIO_BYTES_PER_SAMPLE);
            }

            AudioCursor += (Region1Size + Region2Size);
            AudioCursor %= AUDIO_BUFFER_SIZE;

            IDirectSoundBuffer_Unlock(GlobalAudioBuffer, Region1, Region1Size, Region2, Region2Size);
        }
    }
}

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

void Win32_LoadPalette(const char* Name)
{
    HBITMAP hImage = LoadImageA(GetModuleHandle(0), Name, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    if (!hImage) return;

    BITMAP Image;
    GetObject(hImage, sizeof(BITMAP), &Image);

    if (Image.bmWidth == 16 && Image.bmWidth == 16)
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
        memcpy(GlobalBackbufferInfo.bmiColors, BitmapInfo.bmiColors, sizeof(RGBQUAD) * 256);
    }
    
    DeleteObject(hImage);
}

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
            AdjustWindowRect(&MinWindowDim, WS_OVERLAPPEDWINDOW, 0);
            MINMAXINFO *MinMaxInfo = (MINMAXINFO *)LParam;
            MinMaxInfo->ptMinTrackSize.x = MinWindowDim.right - MinWindowDim.left;
            MinMaxInfo->ptMinTrackSize.y = MinWindowDim.bottom - MinWindowDim.top;
        } break;

        case WM_ENTERSIZEMOVE:
        case WM_KILLFOCUS:
        {
            Win32_LockMouse(false);
            GlobalFocus = false;
            if (GlobalAudioBuffer) 
                IDirectSoundBuffer_Stop(GlobalAudioBuffer);
        } break;

        case WM_EXITSIZEMOVE:
        case WM_SETFOCUS:
        {
            Win32_LockMouse(true);
            GlobalFocus = true;
            if(GlobalAudioBuffer) 
                IDirectSoundBuffer_Play(GlobalAudioBuffer, 0, 0, DSBPLAY_LOOPING);
        } break;

        default:
        {
            Result = DefWindowProcA(Window, Message, WParam, LParam);
        } break;
    }
    
    return Result;
}

int WINAPI CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
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
    Win32_LoadPalette("PALETTE");

    // AUDIO
    DWORD AudioCursor = 0;
    Win32_InitDSound();
    
    // TIMING
    LARGE_INTEGER PerfCountFrequency;
    QueryPerformanceFrequency(&PerfCountFrequency);
    u64 TargetTimePerFrame = PerfCountFrequency.QuadPart / 60;
    u64 TargetTimePerFrameCarry = TargetTimePerFrame + PerfCountFrequency.QuadPart % 60;
    u64 LastTime = Win32_GetTime();

    // GAME STATE
    input Input = { 0 };
    game *Game = malloc(sizeof(game));
    memset(Game, 0, sizeof(game));
    Game_Init(Game);

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

                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
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
                        if (Code == VK_F4)
                            GlobalRunning = false;
                        // if (Code == VK_RETURN) FULLSCREEN
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

                            case 'E': Input.Interact = IsDown; break;
                            case VK_SPACE: Input.Jump = IsDown; break;
                            case VK_SHIFT: Input.Crouch = IsDown; break;
                            case VK_LBUTTON: Input.Punch = IsDown; break;
                            case VK_RBUTTON: Input.Place = IsDown; break;
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

        if (GlobalFocus)
        {
            // UPDATE
            Game_Update(Game, Input);

            // DRAW
            Game_Draw(Game, GlobalBackbuffer);
            
            HDC DeviceContext = GetDC(GlobalWindow);
            Win32_DisplayBitmap(DeviceContext);
            ReleaseDC(GlobalWindow, DeviceContext);

            // AUDIO
            Win32_GatherSamples(AudioCursor);

            // TIMING
            Game->Frame++;
            u64 Target = (Game->Frame % 60) ?  TargetTimePerFrame : TargetTimePerFrameCarry;
            u64 TimeElapsed = Win32_TimeSince(LastTime);
            if (TimeElapsed < Target)
            {
                // Sleep((DWORD)(1000 * (Target - TimeElapsed) / PerfCountFrequency.QuadPart));
                Sleep(1);
                while (TimeElapsed < Target) TimeElapsed = Win32_TimeSince(LastTime);
            }
            LastTime += TimeElapsed;
        }
    }

    free(Game);
    
    return 0;
}

int WinStartUp(void)
{    
    LPWSTR CmdLine = GetCommandLineW();
    int Result = WinMain(GetModuleHandle(0), 0, 0, 0);
    return Result;
}

void WinMainCRTStartup(void)
{
    ExitProcess(WinStartUp());
}

void mainCRTStartup(void)
{
    ExitProcess(WinStartUp());
}