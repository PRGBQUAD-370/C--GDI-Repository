#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#include <cstdlib>

#pragma comment(lib, "winmm.lib")

// ===================== GDI EFFECTS =====================
void FX_Invert(HDC hdc, int w, int h) { PatBlt(hdc, 0, 0, w, h, DSTINVERT); }
void FX_Shift(HDC hdc, int w, int h) { BitBlt(hdc, rand()%10, rand()%10, w, h, hdc, 0, 0, SRCCOPY); }
void FX_Wave(HDC hdc, int w, int h) { static int t=0; for(int y=0;y<h;y++){ int x=(int)(20*sin((y+t)*0.05)); BitBlt(hdc,x,y,w,1,hdc,0,y,SRCCOPY);} t+=3;}
void FX_Melt(HDC hdc,int w,int h){for(int x=0;x<w;x+=4) BitBlt(hdc,x,rand()%15,4,h,hdc,x,0,SRCCOPY);}
void FX_Scanlines(HDC hdc,int w,int h){for(int y=0;y<h;y+=2) BitBlt(hdc,rand()%5,y,w,1,hdc,0,y,SRCCOPY);}
void FX_RandomRects(HDC hdc,int w,int h){for(int i=0;i<20;i++) Rectangle(hdc,rand()%w,rand()%h,rand()%w,rand()%h);}
void FX_RandomLines(HDC hdc,int w,int h){for(int i=0;i<40;i++){MoveToEx(hdc,rand()%w,rand()%h,nullptr); LineTo(hdc,rand()%w,rand()%h);}}
void FX_Stretch(HDC hdc,int w,int h){StretchBlt(hdc,20,20,w-40,h-40,hdc,0,0,w,h,SRCCOPY);}
void FX_Zoom(HDC hdc,int w,int h){StretchBlt(hdc,-20,-20,w+40,h+40,hdc,0,0,w,h,SRCCOPY);}
void FX_ColorNoise(HDC hdc,int w,int h){for(int y=0;y<h;y+=5) for(int x=0;x<w;x+=5) SetPixel(hdc,x,y,RGB(rand()%256,rand()%256,rand()%256));}
void FX_XORShift(HDC hdc,int w,int h){BitBlt(hdc,5,5,w,h,hdc,0,0,SRCINVERT);}
void FX_Mirror(HDC hdc,int w,int h){StretchBlt(hdc,w,0,-w,h,hdc,0,0,w,h,SRCCOPY);}
void FX_Shear(HDC hdc,int w,int h){for(int y=0;y<h;y++) BitBlt(hdc,(y%20)-10,y,w,1,hdc,0,y,SRCCOPY);}
void FX_CircleSpam(HDC hdc,int w,int h){for(int i=0;i<25;i++) Ellipse(hdc,rand()%w,rand()%h,rand()%w,rand()%h);}
void FX_BlockShift(HDC hdc,int w,int h){for(int y=0;y<h;y+=40) BitBlt(hdc,rand()%20-10,y,w,40,hdc,0,y,SRCCOPY);}

// ===================== MAIN =====================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    srand(GetTickCount());

    // ===== FIRST MESSAGE BOX WITH CANCEL =====
    int ret1 = MessageBox(nullptr,
        "15_Sec_GDI.exe: This runs GDI for 15 seconds.\nPress Cancel to exit.",
        "15_Sec_GDI.exe",
        MB_OKCANCEL | MB_ICONWARNING);
    if (ret1 == IDCANCEL) return 0;

    // ===== SECOND MESSAGE BOX WITH CANCEL =====
    int ret2 = MessageBox(nullptr,
        "15_Sec_GDI.exe, Are you sure you want to run the GDI?.",
        "15_Sec_GDI.exe",
        MB_OKCANCEL | MB_ICONWARNING);
    if (ret2 == IDCANCEL) return 0;

    // ===== AUDIO SETUP =====
    const int sampleRate = 8000;
    const int seconds = 15; // ~15 seconds
    const int bufferSize = sampleRate * seconds;
    static unsigned char buffer[bufferSize];

    for (int t = 0; t < bufferSize; t++) {
        if (t < sampleRate * 8) buffer[t] = (unsigned char)((t*(t>>5|t>>8))^(t>>3));
        else buffer[t] = (unsigned char)((t*3 & t>>10) | (t>>4));
    }

    WAVEFORMATEX wf = {};
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.nChannels = 1;
    wf.nSamplesPerSec = sampleRate;
    wf.wBitsPerSample = 8;
    wf.nBlockAlign = 1;
    wf.nAvgBytesPerSec = sampleRate;

    HWAVEOUT hWaveOut = nullptr;
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &wf, 0, 0, CALLBACK_NULL);

    WAVEHDR header = {};
    header.lpData = (LPSTR)buffer;
    header.dwBufferLength = bufferSize;

    waveOutPrepareHeader(hWaveOut, &header, sizeof(header));
    waveOutWrite(hWaveOut, &header, sizeof(header));

    // ===== GDI LOOP =====
    DWORD start = GetTickCount();
    DWORD lastSwitch = start;
    int effect = 0;
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);

    while (GetTickCount() - start < seconds * 1000) {
        HDC hdc = GetDC(nullptr);

        if (GetTickCount() - lastSwitch > 10000) {
            effect = (effect + 1) % 15; // switch effect every 10 sec
            lastSwitch = GetTickCount();
        }

        switch (effect) {
            case 0: FX_Invert(hdc,w,h); break;
            case 1: FX_Shift(hdc,w,h); break;
            case 2: FX_Wave(hdc,w,h); break;
            case 3: FX_Melt(hdc,w,h); break;
            case 4: FX_Scanlines(hdc,w,h); break;
            case 5: FX_RandomRects(hdc,w,h); break;
            case 6: FX_RandomLines(hdc,w,h); break;
            case 7: FX_Stretch(hdc,w,h); break;
            case 8: FX_Zoom(hdc,w,h); break;
            case 9: FX_ColorNoise(hdc,w,h); break;
            case 10: FX_XORShift(hdc,w,h); break;
            case 11: FX_Mirror(hdc,w,h); break;
            case 12: FX_Shear(hdc,w,h); break;
            case 13: FX_CircleSpam(hdc,w,h); break;
            case 14: FX_BlockShift(hdc,w,h); break;
        }

        ReleaseDC(nullptr, hdc);
        Sleep(30);
    }

    waveOutUnprepareHeader(hWaveOut, &header, sizeof(header));
    waveOutClose(hWaveOut);

    return 0;
}