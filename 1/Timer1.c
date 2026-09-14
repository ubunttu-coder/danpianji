#include "Timer1.h"

unsigned char g_key = 0;

void Timer1_Init(void) {
    TMOD |= 0x10;    // Timer1 mode 1 (16-bit)
    TH1 = 0xB1;      // 10ms @ 24MHz
    TL1 = 0xE0;
    ET1 = 1;         // enable Timer1 interrupt
    TR1 = 1;         // start Timer1
}

void Timer1_Routine(void) interrupt 3 {
    static unsigned char key_now = 0, key_prev = 0, key_locked = 0;

    TH1 = 0xB1;      // reload
    TL1 = 0xE0;

    key_now = 0;
    if (KEY_PLAY == 0) key_now = 1;   // pressed = low level
    if (KEY_NEXT == 0) key_now = 2;
    if (KEY_PREV == 0) key_now = 3;
    if (KEY_STOP == 0) key_now = 4;

    if (key_now == key_prev) {        // debounce: two samples equal
        if (!key_locked && key_now != 0) {
            g_key = key_now;          // edge trigger once
            key_locked = 1;
        }
        if (key_locked && key_now == 0) key_locked = 0;
    }
    key_prev = key_now;
}
