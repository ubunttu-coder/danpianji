/**************************************************************
 * Bad Apple player on AT89C52 + LCD12864 + SD card
 * Original: bilibili UP "yalexing" (UID 181818866, GitHub: yalexing)
 * For study only, not for commercial use.
 * Added: independent key control (play/pause, next/prev frame, stop)
 **************************************************************/
#include <lcd12864.c>
#include "Delayms.h"
#include "Timer0.h"
#include "Timer1.h"

sbit BEEP = P2^5;

#define P	0
#define L1	1
#define L1_	2
#define L2	3
#define L2_	4
#define L3	5
#define L4	6
#define L4_	7
#define L5	8
#define L5_	9
#define L6	10
#define L6_	11
#define L7	12
#define M1	13
#define M1_	14
#define M2	15
#define M2_	16
#define M3	17
#define M4	18
#define M4_	19
#define M5	20
#define M5_	21
#define M6	22
#define M6_	23
#define M7	24
#define H1	25
#define H1_	26
#define H2	27
#define H2_	28
#define H3	29
#define H4	30
#define H4_	31
#define H5	32
#define H5_	33
#define H6	34
#define H6_	35
#define H7	36
#define STOP 37

unsigned int xdata FreqTable[] = {
	0,
	63777,63872,63969,64054,64140,64216,64291,64360,64426,64489,64547,64603,
	64655,64704,64751,64795,64837,64876,64913,64948,64981,65012,65042,65070,
	65095,65120,65144,65166,65186,65206,65225,65242,65259,65274,65289,65303
};

unsigned char xdata Music[][2] = {
	{M3,4},{M2,2},{M1,4},{M2,2},{M3,3},{M4,1},{M3,2},{M2,4},{P,2},

	{M3,4},{M2,2},{M1,4},{M2,2},{M3,3},{M4,1},{M3,2},{M2,4},{P,2},

	{M3,4},{M2,2},{M1,4},{M2,2},{M3,3},{M4,1},{M3,2},{M2,4},{P,2},

	{M3,4},{M2,2},{M1,4},{M2,2},{M3,3},{M4,1},{M3,2},{M2,4},

	{M1,1},{M2,1},{M3,2},{M3,2},{M2,2},{M4,2},{M3,2},{M2,2},{M2,2},{M2,2},{M1,1},{M1,1},{M4,2},{M3,2},{M2,2},{M2,4},{M1,1},{M2,1},{M3,6},{P,6},

	{M3,2},{M5,2},{H1,1},{H1,1},{M7,4},{H1,2},{M7,4},{H1,2},{M7,1},{M6,1},{M5,2},{P,2},

	{M5,2},{M2,2},{M4,2},{M4,4},{M3,2},{M3,2},{P,2},{L5,2},{M4,2},{M3,2},{M2,2},{M3,4},{M5,2},{M1,6},

	{P,4},{M1,2},{M2,2},{M1,3},{M1,1},{M1,2},{M5,2},{M1,2},{M4,4},{M3,2},{M2,4},{M1,2},{M1,6},

	{P,4},{M1,1},{M2,1},{M3,2},{M3,2},{M2,2},{M4,2},{M3,2},{M2,2},{M2,2},{M2,2},{M1,1},{M1,1},{M4,2},{M3,2},{M2,2},{M2,4},{M1,1},{M2,1},{M3,6},{P,6},

	{M3,2},{M5,2},{H1,2},{M7,4},{H1,2},{M7,4},{H1,2},{M7,1},{M6,1},{M5,2},{P,2},

	{M5,2},{M2,2},{M4,2},{M4,4},{M3,2},{M3,2},{P,2},{L5,2},{M4,2},{M3,2},{M2,2},{M3,4},{M5,2},{M1,10},{M1,1},{M1,1},{M2,2},{M1,2},{P,2},

	{M1,2},{M5,2},{M1,2},{M4,2},{M4,1},{M4,1},{M3,1},{M2,1},{M2,4},{M1,2},{M1,6},{P,6},

	{M6,2},{M5,2},{M5,2},{M5,2},{M4,2},{M4,2},{M3,2},{M2,2},{M2,2},{M2,4},

	{M5,2},{M5,2},{M4,1},{M4,1},{M4,2},{M4,2},{M3,2},{M2,2},{M2,4},{M1,1},{L7,1},{M1,4},{P,2},

// {M6,2},{M5,2},{M5,2},{M5,2},{M4,2},{M4,2},{M3,2},{M2,2},{M2,2},{M2,4},

//	{M2,2},{M3,2},{M3,1},{M3,1},{M3,1},{M3,1},{M3,2},{M2,2},{M3,2},{H2,4},{H1,2},{H1,2},{P,2},

//	{H1,2},{M7,4},{M6,2},{M6,6},{P,4},{M6,2},{M6,2},{M5,2},{M4,1},{M4,1},{M4,6},{M3,1},{M4,1},{M5,10},{P,6},

//	{M3,1},{M2,1},{M3,1},{M2,1},{M3,1},{M4,1},{M5,4},{M4,1},{M5,1},{M6,4},{M6,1},{M7,1},{H1,2},{P,2},

//	{H2,1},{H1,1},{M5,4},{M1,2},{M5,2},{M4,2},{M3,2},{M3,4},{M3,1},{M3,1},{M5,4},{P,2},

//	{M3,1},{M2,1},{M3,1},{M2,1},{M3,1},{M4,1},{M5,4},{M4,1},{M5,1},{M6,4},{M6,1},{M7,1},{H1,2},{P,2},

//	{H1,1},{H3,2},{H3,2},{P,1},{H2,1},{H4,2},{H3,2},{H2,2},{H2,3},{H1,1},{H1,1},{M7,1},{H1,2},{P,2},

//	{M5,1},{H1,1},{H2,4},{H1,2},{H1,2},{P,2},{M5,2},{H2,4},{H1,2},{H1,2},{P,2},

//	{M5,1},{H1,1},{H2,4},{H1,2},{H1,2},{P,2},{M5,1},{H1,1},{H2,3},{H3,1},{H2,2},{H1,2},{P,2},

	{H1,2},{H1,4},{M6,2},{M6,4},{M5,2},{M5,4},{M4,2},{M4,2},{M3,2},{M2,2},{M3,6},{P,6},{M3,2},{M4,2},{M3,2},{M4,2},{M3,2},{M2,2},{M1,6},{STOP,1}};

unsigned char FreqSelect, MusicSelect;

// ===== key control state =====
bit g_playing = 1;                     // 1=playing, 0=paused
unsigned long g_address = 0;           // current frame address in SD
unsigned char xdata frame_buf[4][176]; // 4 blocks frame buffer

// read one frame (704 bytes) from SD and show it on LCD
void readAndShowOneFrame(void) {
    SdReadBlock(frame_buf[0], g_address, 176); g_address += 176;
    SdReadBlock(frame_buf[1], g_address, 176); g_address += 176;
    SdReadBlock(frame_buf[2], g_address, 176); g_address += 176;
    SdReadBlock(frame_buf[3], g_address, 176); g_address += 176;

    dprintf_hanzi_string_1(frame_buf[0], 0, 0, 16, 0);
    dprintf_hanzi_string_1(frame_buf[1], 0, 16, 16, 0);
    dprintf_hanzi_string_1(frame_buf[2], 0, 32, 16, 0);
    dprintf_hanzi_string_1(frame_buf[3], 0, 48, 16, 0);
}

// play one note of the music
void playOneNote(void) {
    if (FreqSelect == STOP) {
        TR0 = 0;
        g_playing = 0;               // music ends -> stop (keys still work)
    } else {
        FreqSelect = Music[MusicSelect][0];
        Delayms(140 * Music[MusicSelect][1]);
        MusicSelect++;
        TR0 = 0;
        Delayms(5);
        TR0 = 1;
    }
}

void main(void) {
    F12864_init();
    F12864_clear();
    write_commond(0x98);
    while (!SdInit());    // init SD card (must be done before reading)
    Timer0_Init();
    Timer1_Init();

    while (1) {
        // handle key command
        if (g_key) {
            switch (g_key) {
                case 1: g_playing = !g_playing; break;          // play/pause
                case 2: readAndShowOneFrame(); break;           // next frame
                case 3: if (g_address >= 704) {                 // prev frame
                            g_address -= 704;
                            readAndShowOneFrame();
                        }
                        break;
                case 4: g_address = 0; g_playing = 0; break;    // stop
            }
            g_key = 0;
        }

        // auto play
        if (g_playing) {
            readAndShowOneFrame();
            playOneNote();
        }
    }
}

void Timer0_Routine() interrupt 1 {
    if (FreqTable[FreqSelect]) {
        TH0 = FreqTable[FreqSelect] / 256;
        TL0 = FreqTable[FreqSelect] % 256;
        BEEP = !BEEP;
    }
}
