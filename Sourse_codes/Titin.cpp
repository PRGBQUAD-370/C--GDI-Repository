#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#include <cstdlib>
#include <thread>          // ← This was missing

#pragma comment(lib, "winmm.lib")

// ================= GDI EFFECTS =================
void EffectInvert(HDC hdc, int w, int h)
{
    PatBlt(hdc, 0, 0, w, h, DSTINVERT);
}

void EffectWave(HDC hdc, int w, int h)
{
    static int offset = 0;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, hdc, 0, 0, SRCCOPY);

    for (int y = 0; y < h; y++)
    {
        int x = (int)(12 * sin((y + offset) * 0.05));
        BitBlt(hdc, x, y, w, 1, mem, 0, y, SRCCOPY);
    }
    offset += 6;

    DeleteObject(bmp);
    DeleteDC(mem);
}

void EffectStretch(HDC hdc, int w, int h)
{
    int dx = (rand() % 80) - 40;
    int dy = (rand() % 80) - 40;
   
    StretchBlt(hdc,
        dx, dy,
        w - 60 + (rand() % 50),
        h - 60 + (rand() % 50),
        hdc, 0, 0, w, h, SRCCOPY);
}

// ================= SOUND FUNCTIONS =================
VOID WINAPI sound1()
{
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);
    
    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>((9 * t & t >> 4 | 5 * t & t >> 7 | 3 * t & t >> 10) - 0);

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    Sleep(30000);
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

VOID WINAPI sound2()
{
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(t >> 5 | (t >> 2) * (t >> 3));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    Sleep(30000);
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

VOID WINAPI sound3()
{
    HWAVEOUT hWaveOut = 0;
    WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, 1, 8000, 8000, 1, 8, 0 };
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL);

    char buffer[8000 * 30] = {};
    for (DWORD t = 0; t < sizeof(buffer); ++t)
        buffer[t] = static_cast<char>(2 * (t >> 5 & t) - (t >> 5) + t * (t >> 14 & 14));

    WAVEHDR header = { buffer, sizeof(buffer), 0, 0, 0, 0, 0, 0 };
    waveOutPrepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutWrite(hWaveOut, &header, sizeof(WAVEHDR));
    Sleep(30000);
    waveOutUnprepareHeader(hWaveOut, &header, sizeof(WAVEHDR));
    waveOutClose(hWaveOut);
}

// Global variable - declared BEFORE WinMain
int currentEffect = 0;

// ================= MAIN (last thing in the file) =================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    srand(GetTickCount());

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    DWORD startTime = GetTickCount();
    DWORD lastSwitch = startTime;

    // Play sounds ONE AFTER ANOTHER in background thread (so GDI doesn't freeze)
    std::thread soundThread([]() {
        sound1();
        sound2();
        sound3();
    });
    soundThread.detach();

    while (GetTickCount() - startTime < 300000) // 5 minutes
    {
        HDC hdc = GetDC(NULL);

        if (GetTickCount() - lastSwitch > 30000) // Switch every 10 seconds
        {
            if (++currentEffect > 2) 
                currentEffect = 0;
            lastSwitch = GetTickCount();
        }

        switch (currentEffect)
        {
        case 0: EffectInvert(hdc, width, height); break;
        case 1: EffectWave(hdc, width, height); break;
        case 2: EffectStretch(hdc, width, height); break;
        }

        ReleaseDC(NULL, hdc);
        Sleep(30);
    }

    // Final screen restore
    HDC hdc = GetDC(NULL);
    PatBlt(hdc, 0, 0, width, height, DSTINVERT);
    ReleaseDC(NULL, hdc);

    return 0;
}