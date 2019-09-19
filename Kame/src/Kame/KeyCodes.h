#pragma once

#ifdef KAME_PLATFORM_GLFW

// From glfw3.h
#define KAME_KEY_SPACE              32
#define KAME_KEY_APOSTROPHE         39  /* ' */
#define KAME_KEY_COMMA              44  /* , */
#define KAME_KEY_MINUS              45  /* - */
#define KAME_KEY_PERIOD             46  /* . */
#define KAME_KEY_SLASH              47  /* / */
#define KAME_KEY_0                  48
#define KAME_KEY_1                  49
#define KAME_KEY_2                  50
#define KAME_KEY_3                  51
#define KAME_KEY_4                  52
#define KAME_KEY_5                  53
#define KAME_KEY_6                  54
#define KAME_KEY_7                  55
#define KAME_KEY_8                  56
#define KAME_KEY_9                  57
#define KAME_KEY_SEMICOLON          59  /* ; */
#define KAME_KEY_EQUAL              61  /* = */
#define KAME_KEY_A                  65
#define KAME_KEY_B                  66
#define KAME_KEY_C                  67
#define KAME_KEY_D                  68
#define KAME_KEY_E                  69
#define KAME_KEY_F                  70
#define KAME_KEY_G                  71
#define KAME_KEY_H                  72
#define KAME_KEY_I                  73
#define KAME_KEY_J                  74
#define KAME_KEY_K                  75
#define KAME_KEY_L                  76
#define KAME_KEY_M                  77
#define KAME_KEY_N                  78
#define KAME_KEY_O                  79
#define KAME_KEY_P                  80
#define KAME_KEY_Q                  81
#define KAME_KEY_R                  82
#define KAME_KEY_S                  83
#define KAME_KEY_T                  84
#define KAME_KEY_U                  85
#define KAME_KEY_V                  86
#define KAME_KEY_W                  87
#define KAME_KEY_X                  88
#define KAME_KEY_Y                  89
#define KAME_KEY_Z                  90
#define KAME_KEY_LEFT_BRACKET       91  /* [ */
#define KAME_KEY_BACKSLASH          92  /* \ */
#define KAME_KEY_RIGHT_BRACKET      93  /* ] */
#define KAME_KEY_GRAVE_ACCENT       96  /* ` */
#define KAME_KEY_WORLD_1            161 /* non-US #1 */
#define KAME_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define KAME_KEY_ESCAPE             256
#define KAME_KEY_ENTER              257
#define KAME_KEY_TAB                258
#define KAME_KEY_BACKSPACE          259
#define KAME_KEY_INSERT             260
#define KAME_KEY_DELETE             261
#define KAME_KEY_RIGHT              262
#define KAME_KEY_LEFT               263
#define KAME_KEY_DOWN               264
#define KAME_KEY_UP                 265
#define KAME_KEY_PAGE_UP            266
#define KAME_KEY_PAGE_DOWN          267
#define KAME_KEY_HOME               268
#define KAME_KEY_END                269
#define KAME_KEY_CAPS_LOCK          280
#define KAME_KEY_SCROLL_LOCK        281
#define KAME_KEY_NUM_LOCK           282
#define KAME_KEY_PRINT_SCREEN       283
#define KAME_KEY_PAUSE              284
#define KAME_KEY_F1                 290
#define KAME_KEY_F2                 291
#define KAME_KEY_F3                 292
#define KAME_KEY_F4                 293
#define KAME_KEY_F5                 294
#define KAME_KEY_F6                 295
#define KAME_KEY_F7                 296
#define KAME_KEY_F8                 297
#define KAME_KEY_F9                 298
#define KAME_KEY_F10                299
#define KAME_KEY_F11                300
#define KAME_KEY_F12                301
#define KAME_KEY_F13                302
#define KAME_KEY_F14                303
#define KAME_KEY_F15                304
#define KAME_KEY_F16                305
#define KAME_KEY_F17                306
#define KAME_KEY_F18                307
#define KAME_KEY_F19                308
#define KAME_KEY_F20                309
#define KAME_KEY_F21                310
#define KAME_KEY_F22                311
#define KAME_KEY_F23                312
#define KAME_KEY_F24                313
#define KAME_KEY_F25                314
#define KAME_KEY_KP_0               320
#define KAME_KEY_KP_1               321
#define KAME_KEY_KP_2               322
#define KAME_KEY_KP_3               323
#define KAME_KEY_KP_4               324
#define KAME_KEY_KP_5               325
#define KAME_KEY_KP_6               326
#define KAME_KEY_KP_7               327
#define KAME_KEY_KP_8               328
#define KAME_KEY_KP_9               329
#define KAME_KEY_KP_DECIMAL         330
#define KAME_KEY_KP_DIVIDE          331
#define KAME_KEY_KP_MULTIPLY        332
#define KAME_KEY_KP_SUBTRACT        333
#define KAME_KEY_KP_ADD             334
#define KAME_KEY_KP_ENTER           335
#define KAME_KEY_KP_EQUAL           336
#define KAME_KEY_LEFT_SHIFT         340
#define KAME_KEY_LEFT_CONTROL       341
#define KAME_KEY_LEFT_ALT           342
#define KAME_KEY_LEFT_SUPER         343
#define KAME_KEY_RIGHT_SHIFT        344
#define KAME_KEY_RIGHT_CONTROL      345
#define KAME_KEY_RIGHT_ALT          346
#define KAME_KEY_RIGHT_SUPER        347
#define KAME_KEY_MENU               348

#else

#define KAME_KEY_ABNT_C1	0xC1
#define KAME_KEY_ABNT_C2	0xC2
#define KAME_KEY_ADD	0x6B
#define KAME_KEY_ATTN	0xF6
#define KAME_KEY_BACK	0x08
#define KAME_KEY_CANCEL	0x03
#define KAME_KEY_CLEAR	0x0C
#define KAME_KEY_CRSEL	0xF7
#define KAME_KEY_DECIMAL	0x6E
#define KAME_KEY_DIVIDE	0x6F
#define KAME_KEY_EREOF	0xF9
#define KAME_KEY_ESCAPE	0x1B
#define KAME_KEY_EXECUTE	0x2B
#define KAME_KEY_EXSEL	0xF8
#define KAME_KEY_ICO_CLEAR	0xE6
#define KAME_KEY_ICO_HELP	0xE3
#define KAME_KEY_0	0x30
#define KAME_KEY_1	0x31
#define KAME_KEY_2	0x32
#define KAME_KEY_3	0x33
#define KAME_KEY_4	0x34
#define KAME_KEY_5	0x35
#define KAME_KEY_6	0x36
#define KAME_KEY_7	0x37
#define KAME_KEY_8	0x38
#define KAME_KEY_9	0x39
#define KAME_KEY_A	0x41
#define KAME_KEY_B	0x42
#define KAME_KEY_C	0x43
#define KAME_KEY_D	0x44
#define KAME_KEY_E	0x45
#define KAME_KEY_F	0x46
#define KAME_KEY_G	0x47
#define KAME_KEY_H	0x48
#define KAME_KEY_I	0x49
#define KAME_KEY_J	0x4A
#define KAME_KEY_K	0x4B
#define KAME_KEY_L	0x4C
#define KAME_KEY_M	0x4D
#define KAME_KEY_N	0x4E
#define KAME_KEY_O	0x4F
#define KAME_KEY_P	0x50
#define KAME_KEY_Q	0x51
#define KAME_KEY_R	0x52
#define KAME_KEY_S	0x53
#define KAME_KEY_T	0x54
#define KAME_KEY_U	0x55
#define KAME_KEY_V	0x56
#define KAME_KEY_W	0x57
#define KAME_KEY_X	0x58
#define KAME_KEY_Y	0x59
#define KAME_KEY_Z	0x5A
#define KAME_KEY_MULTIPLY	0x6A
#define KAME_KEY_NONAME	0xFC
#define KAME_KEY_NUMPAD0	0x60
#define KAME_KEY_NUMPAD1	0x61
#define KAME_KEY_NUMPAD2	0x62
#define KAME_KEY_NUMPAD3	0x63
#define KAME_KEY_NUMPAD4	0x64
#define KAME_KEY_NUMPAD5	0x65
#define KAME_KEY_NUMPAD6	0x66
#define KAME_KEY_NUMPAD7	0x67
#define KAME_KEY_NUMPAD8	0x68
#define KAME_KEY_NUMPAD9	0x69
#define KAME_KEY_OEM_1	0xBA
#define KAME_KEY_OEM_102	0xE2
#define KAME_KEY_OEM_2	0xBF
#define KAME_KEY_OEM_3	0xC0
#define KAME_KEY_OEM_4	0xDB
#define KAME_KEY_OEM_5	0xDC
#define KAME_KEY_OEM_6	0xDD
#define KAME_KEY_OEM_7	0xDE
#define KAME_KEY_OEM_8	0xDF
#define KAME_KEY_OEM_ATTN	0xF0
#define KAME_KEY_OEM_AUTO	0xF3
#define KAME_KEY_OEM_AX	0xE1
#define KAME_KEY_OEM_BACKTAB	0xF5
#define KAME_KEY_OEM_CLEAR	0xFE
#define KAME_KEY_OEM_COMMA	0xBC
#define KAME_KEY_OEM_COPY	0xF2
#define KAME_KEY_OEM_CUSEL	0xEF
#define KAME_KEY_OEM_ENLW	0xF4
#define KAME_KEY_OEM_FINISH	0xF1
#define KAME_KEY_OEM_FJ_LOYA	0x95
#define KAME_KEY_OEM_FJ_MASSHOU	0x93
#define KAME_KEY_OEM_FJ_ROYA	0x96
#define KAME_KEY_OEM_FJ_TOUROKU	0x94
#define KAME_KEY_OEM_JUMP	0xEA
#define KAME_KEY_OEM_MINUS	0xBD
#define KAME_KEY_OEM_PA1	0xEB
#define KAME_KEY_OEM_PA2	0xEC
#define KAME_KEY_OEM_PA3	0xED
#define KAME_KEY_OEM_PERIOD	0xBE
#define KAME_KEY_OEM_PLUS	0xBB
#define KAME_KEY_OEM_RESET	0xE9
#define KAME_KEY_OEM_WSCTRL	0xEE
#define KAME_KEY_PA1	0xFD
#define KAME_KEY_PACKET	0xE7
#define KAME_KEY_PLAY	0xFA
#define KAME_KEY_PROCESSKEY	0xE5
#define KAME_KEY_RETURN	0x0D	Enter
#define KAME_KEY_SELECT	0x29
#define KAME_KEY_SEPARATOR	0x6C
#define KAME_KEY_SPACE	0x20
#define KAME_KEY_SUBTRACT	0x6D
#define KAME_KEY_TAB	0x09
#define KAME_KEY_ZOOM	0xFB
 
#define KAME_KEY__none_	0xFF
#define KAME_KEY_ACCEPT	0x1E
#define KAME_KEY_APPS	0x5D
#define KAME_KEY_BROWSER_BACK	0xA6
#define KAME_KEY_BROWSER_FAVORITES	0xAB
#define KAME_KEY_BROWSER_FORWARD	0xA7
#define KAME_KEY_BROWSER_HOME	0xAC
#define KAME_KEY_BROWSER_REFRESH	0xA8
#define KAME_KEY_BROWSER_SEARCH	0xAA
#define KAME_KEY_BROWSER_STOP	0xA9
#define KAME_KEY_CAPITAL	0x14
#define KAME_KEY_CONVERT	0x1C
#define KAME_KEY_DELETE	0x2E
#define KAME_KEY_DOWN	0x28
#define KAME_KEY_END	0x23
#define KAME_KEY_F1	0x70
#define KAME_KEY_F10	0x79
#define KAME_KEY_F11	0x7A
#define KAME_KEY_F12	0x7B
#define KAME_KEY_F13	0x7C
#define KAME_KEY_F14	0x7D
#define KAME_KEY_F15	0x7E
#define KAME_KEY_F16	0x7F
#define KAME_KEY_F17	0x80
#define KAME_KEY_F18	0x81
#define KAME_KEY_F19	0x82
#define KAME_KEY_F2	0x71
#define KAME_KEY_F20	0x83
#define KAME_KEY_F21	0x84
#define KAME_KEY_F22	0x85
#define KAME_KEY_F23	0x86
#define KAME_KEY_F24	0x87
#define KAME_KEY_F3	0x72




#define KAME_KEY_F4	0x73
#define KAME_KEY_F5	0x74
#define KAME_KEY_F6	0x75
#define KAME_KEY_F7	0x76
#define KAME_KEY_F8	0x77
#define KAME_KEY_F9	0x78
#define KAME_KEY_FINAL	0x18
#define KAME_KEY_HELP	0x2F
#define KAME_KEY_HOME	0x24
#define KAME_KEY_ICO_00	0xE4
#define KAME_KEY_INSERT	0x2D
#define KAME_KEY_JUNJA	0x17
#define KAME_KEY_KANA	0x15
#define KAME_KEY_KANJI	0x19
#define KAME_KEY_LAUNCH_APP1	0xB6
#define KAME_KEY_LAUNCH_APP2	0xB7
#define KAME_KEY_LAUNCH_MAIL	0xB4
#define KAME_KEY_LAUNCH_MEDIA_SELECT	0xB5
#define KAME_KEY_LBUTTON	0x01
#define KAME_KEY_LCONTROL	0xA2
#define KAME_KEY_LEFT	0x25
#define KAME_KEY_LMENU	0xA4
#define KAME_KEY_LSHIFT	0xA0
#define KAME_KEY_LWIN	0x5B
#define KAME_KEY_MBUTTON	0x04


#define KAME_KEY_MEDIA_NEXT_TRACK	0xB0
#define KAME_KEY_MEDIA_PLAY_PAUSE	0xB3
#define KAME_KEY_MEDIA_PREV_TRACK	0xB1
#define KAME_KEY_MEDIA_STOP	0xB2
#define KAME_KEY_MODECHANGE	0x1F
#define KAME_KEY_NEXT	0x22
#define KAME_KEY_NONCONVERT	0x1D
#define KAME_KEY_NUMLOCK	0x90
#define KAME_KEY_OEM_FJ_JISHO	0x92
#define KAME_KEY_PAUSE	0x13
#define KAME_KEY_PRINT	0x2A
#define KAME_KEY_PRIOR	0x21
#define KAME_KEY_RBUTTON	0x02
#define KAME_KEY_RCONTROL	0xA3
#define KAME_KEY_RIGHT	0x27
#define KAME_KEY_RMENU	0xA5
#define KAME_KEY_RSHIFT	0xA1
#define KAME_KEY_RWIN	0x5C
#define KAME_KEY_SCROLL	0x91
#define KAME_KEY_SLEEP	0x5F
#define KAME_KEY_SNAPSHOT	0x2C
#define KAME_KEY_UP	0x26
#define KAME_KEY_VOLUME_DOWN	0xAE
#define KAME_KEY_VOLUME_MUTE	0xAD
#define KAME_KEY_VOLUME_UP	0xAF
#define KAME_KEY_XBUTTON1	0x05
#define KAME_KEY_XBUTTON2	0x06


#endif