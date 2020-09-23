// translation.h
// Copyright (c) 2013 - 2016, zhiayang
// Licensed under the Apache License Version 2.0.

#include <stdint.h>

// copied from mx.
// https://github.com/zhiayang/mx/blob/develop/source/Kernel/HardwareAbstraction/Devices/Keyboard/Translation.hpp
namespace tables
{
	// PS/2 Scancodes
#if 0
	static const uint32_t ScanCode2_US[2 * 132] =
	{
		 0,    0,           //          0
		 0,    0,           // f9       1
		 0,    0,           //          2
		 0,    0,           // f5       3
		 0,    0,           // f3       4
		 0,    0,           // f1       5
		 0,    0,           // f2       6
		 0,    0,           // f12      7
		 0,    0,           //          8
		 0,    0,           // f10      9
		 0,    0,           // f8       10
		 0,    0,           // f6       11
		 0,    0,           // f4       12
		'\t', '\t',         // \t       13
		'`',  '`',          // `        14
		 0,    0,           //          15
		 0,    0,           //          16
		 0,    0,           // lalt     17
		 0,    0,           // lshft    18
		 0,    0,           //          19
		 0,    0,           // lctrl    20
		'q',  'Q',          // q        21
		'1',  '!',          // 1        22
		 0,    0,           //          23
		 0,    0,           //          24
		 0,    0,           //          25
		'z',  'Z',          // z        26
		's',  'S',          // s        27
		'a',  'A',          // a        28
		'w',  'W',          // w        29
		'2',  '@',          // 2        30
		 0,    0,           //          31
		 0,    0,           //          32
		'c',  'C',          // c        33
		'x',  'X',          // x        34
		'd',  'D',          // d        35
		'e',  'E',          // e        36
		'4',  '$',          // 4        37
		'3',  '#',          // 3        38
		 0,    0,           //          39
		 0,    0,           //          40
		' ',  ' ',          // spce     41
		'v',  'V',          // v        42
		'f',  'F',          // f        43
		't',  'T',          // t        44
		'r',  'R',          // r        45
		'5',  '%',          // 5        46
		 0,    0,           //          47
		 0,    0,           //          48
		'n',  'N',          // n        49
		'b',  'B',          // b        50
		'h',  'H',          // h        51
		'g',  'G',          // g        52
		'y',  'Y',          // y        53
		'6',  '^',          // 6        54
		 0,    0,           //          55
		 0,    0,           //          56
		 0,    0,           //          57
		'm',  'M',          // m        58
		'j',  'J',          // j        59
		'u',  'U',          // u        60
		'7',  '&',          // 7        61
		'8',  '*',          // 8        62
		 0,    0,           //          63
		 0,    0,           //          64
		',',  '<',          // ,        65
		'k',  'K',          // k        66
		'i',  'I',          // i        67
		'o',  'O',          // o        68
		'0',  ')',          // 0        69
		'9',  '(',          // 9        70
		 0,    0,           //          71
		 0,    0,           //          72
		'.',  '>',          // .        73
		'/',  '?',          // /        74
		'l',  'L',          // l        75
		';',  ':',          // ;        76
		'p',  'P',          // p        77
		'-',  '_',          // -        78
		 0,    0,           //          79
		 0,    0,           //          80
		 0,    0,           //          81
		'\'', '"',          // '        82
		 0,    0,           //          83
		'[',  '{',          // [        84
		'=',  '+',          // =        85
		 0,    0,           //          86
		 0,    0,           //          87
		 0,    0,           // caps     88
		 0,    0,           // rshft    89
		'\n', '\n',         // enter    90
		']',  '}',          // ]        91
		 0,    0,           //          92
		'\\', '|',          // \        93
		 0,    0,           //          94
		 0,    0,           //          95
		 0,    0,           //          96
		 0,    0,           //          97
		 0,    0,           //          98
		 0,    0,           //          99
		 0,    0,           //          100
		 0,    0,           //          101
		'\b', '\b',         // back     102
		 0,    0,           //          103
		 0,    0,           //          104
		'1',  '!',          // k1       105
		 0,    0,           //          106
		'4',  '$',          // k4       107
		'7',  '&',          // k7       108
		 0,    0,           //          109
		 0,    0,           //          110
		 0,    0,           //          111
		'0',  ')',          // k0       112
		'.',  '>',          // k.       113
		'2',  '@',          // k2       114
		'5',  '%',          // k5       115
		'6',  '^',          // k6       116
		'8',  '*',          // k8       117
		 0,    0,           // esc      118
		 0,    0,           // nlock    119
		 0,    0,           // f11      120
		'+',  '+',          // k+       121
		'3',  '#',          // k3       122
		'-',  '_',          // k-       123
		'*',  '*',          // k*       124
		'9',  '(',          // k9       125
		 0,    0,           // slock    126
		 0,    0,           //          127
		 0,    0,           //          128
		 0,    0,           //          129
		 0,    0,           //          130
		 0,    0            // f7       131
	};
#endif

	// PS/2 Scancodes, with 0xE0 prefix
	static const uint32_t ScanCode2_US_E0[2 * 128] =
	{
		0,      //          1
		0,      //          2
		0,      //          3
		0,      //          4
		0,      //          5
		0,      //          6
		0,      //          7
		0,      //          8
		0,      //          9
		0,      //          A
		0,      //          B
		0,      //          C
		0,      //          D
		0,      //          E
		0,      //          F
		0,      // MWsearch 10
		0,      // ralt     11
		0,      //          12
		0,      //          13
		0,      // rctrl    14
		0,      // Mprev    15
		0,      //          16
		0,      //          17
		0,      // MWfav    18
		0,      //          19
		0,      //          1A
		0,      //          1B
		0,      //          1C
		0,      //          1D
		0,      //          1E
		0,      // lsuper   1F
		0,      // MWrefrsh 20
		0,      // Mvoldown 21
		0,      //          22
		0,      // Mmute    23
		0,      //          24
		0,      //          25
		0,      //          26
		0,      // rsuper   27
		0,      // MWstop   28
		0,      //          29
		0,      //          2A
		0,      // Mcalc    2B
		0,      //          2C
		0,      //          2D
		0,      //          2E
		0,      // apps     2F
		0,      // MWfwd    30
		0,      //          31
		0,      // Mvolup   32
		0,      //          33
		0,      // Mplaypse 34
		0,      //          35
		0,      //          36
		0,      // Apower   37
		0,      // MWback   38
		0,      //          39
		0,      // MWhome   3A
		0,      // Mstop    3B
		0,      //          3C
		0,      //          3D
		0,      //          3E
		0,      // Asleep   3F
		0,      // Mmcom    40
		0,      //          41
		0,      //          42
		0,      //          43
		0,      //          44
		0,      //          45
		0,      //          46
		0,      //          47
		0,      // Memail   48
		0,      //          49
		0,      // k/       4A
		0,      //          4B
		0,      //          4C
		0,      // Mnext    4D
		0,      //          4E
		0,      //          4F
		0,      // Mselect  50
		0,      //          51
		0,      //          52
		0,      //          53
		0,      //          54
		0,      //          55
		0,      //          56
		0,      //          57
		0,      //          58
		0,      //          59
		0,      // kenter   5A
		0,      //          5B
		0,      //          5C
		0,      //          5D
		0,      // Awake    5E
		0,      //          5F
		0,      //          60
		0,      //          61
		0,      //          62
		0,      //          63
		0,      //          64
		0,      //          65
		0,      //          66
		0,      //          67
		0,      //          68
		0,      // end      69
		0,      //          6A
		0,      // left     6B
		0,      // home     6C
		0,      //          6D
		0,      //          6E
		0,      //          6F
		0,      // insert   70
		0,      // delete   71
		0,      // down     72
		0,      //          73
		0,      // right    74
		0,      // up       75
		0,      //          76
		0,      //          77
		0,      //          78
		0,      //          79
		0,      // pagedown 7A
		0,      //          7B
		0,      //          7C
		0,      // pageup   7D
	};

	// Ordered array to translate PS/2 Scancodes to USB HID Scancodes
	static const uint32_t ScanCode2_US_PS2_HID[2 * 128] =
	{
		0x00,   // NULL
		0x42,   // F9

		0x00,   // NULL

		0x3E,   // F5
		0x3C,   // F3
		0x3A,   // F1
		0x3B,   // F2
		0x45,   // F12

		0x00,   // NULL

		0x43,   // F10
		0x41,   // F8
		0x3F,   // F6
		0x3D,   // F4

		0x2B,   // Tab
		0x35,   // `

		0x00,   // NULL
		0x00,   // NULL

		0xE1,   // LeftShift
		0xE2,   // LeftAlt

		0x00,   // NULL

		0xE0,   // LeftControl
		0x14,   // Q
		0x1E,   // 1

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x1D,   // Z
		0x16,   // S
		0x04,   // A
		0x1A,   // W
		0x1F,   // 2

		0x00,   // NULL
		0x00,   // NULL

		0x06,   // C
		0x1B,   // X
		0x07,   // D
		0x08,   // E
		0x21,   // 4
		0x20,   // 3

		0x00,   // NULL
		0x00,   // NULL

		0x2C,   // Space
		0x19,   // V
		0x09,   // F
		0x17,   // T
		0x15,   // R
		0x22,   // 5

		0x00,   // NULL
		0x00,   // NULL

		0x11,   // N
		0x05,   // B
		0x0B,   // H
		0x0A,   // G
		0x1C,   // Y
		0x23,   // 6

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x10,   // M
		0x0D,   // J
		0x18,   // U
		0x24,   // 7
		0x25,   // 8

		0x00,   // NULL
		0x00,   // NULL

		0x36,   // ,
		0x0E,   // K
		0x0C,   // I
		0x12,   // O
		0x27,   // 0
		0x26,   // 9

		0x00,   // NULL
		0x00,   // NULL

		0x37,   // .
		0x38,   // /
		0x0F,   // L
		0x33,   // ;
		0x13,   // P
		0x2D,   // -

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x34,   // '

		0x00,   // NULL

		0x2F,   // [
		0x2E,   // =

		0x00,   // NULL
		0x00,   // NULL

		0x39,   // CapsLock
		0xE5,   // RightShift
		0x28,   // Enter
		0x30,   // ]

		0x00,   // NULL

		0x31,   // '\'

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x2A,   // backspace

		0xA5,   // NULL
		0xA6,   // NULL

		0x59,   // Keypad 1

		0xA7,   // NULL

		0x5C,   // Keypad 4
		0x5F,   // Keypad 7

		0xA8,   // NULL
		0xA9,   // NULL
		0xAA,   // NULL

		0x62,   // Keypad 0
		0x63,   // Keypad .
		0x5A,   // Keypad 2
		0x5D,   // Keypad 5
		0x60,   // Keypad 8
		0x5E,   // Keypad 6
		0x29,   // Escape
		0x53,   // NumLock
		0x44,   // F11
		0x57,   // Keypad +
		0x5B,   // Keypad 3
		0x56,   // Keypad -
		0x55,   // Keypad *
		0x61,   // Keypad 9
		0x47,   // ScrollLock

		0xAB,   // NULL
		0xAC,   // NULL
		0xAD,   // NULL
		0xAE,   // NULL

		0x40    // F7
	};

	// Ordered array to translate PS/2 Scancodes (With 0xE0 prefix) to USB HID Scancodes
	static const uint32_t ScanCode2_US_E0_HID[2 * 256] =
	{
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaSearch
		0xE6,   // RightAlt

		0x00,   // NULL
		0x00,   // NULL

		0xE4,   // RightControl
		0x00,   // MultimediaPrev

		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaWebFav

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0xE3,   // LeftSuper
		0x00,   // MultimediaWebRefresh
		0x00,   // MultimediaVolDown

		0x00,   // NULL

		0x00,   // MultimediaMute

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0xE7,   // RightSuper
		0x00,   // MultimediaWebStop

		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaCalc

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x65,   // App
		0x00,   // MultimediaWebForward

		0x00,   // NULL

		0x00,   // MultimediaVolUp

		0x00,   // NULL

		0x00,   // MultimediaPlayPause

		0x00,   // NULL
		0x00,   // NULL

		0x00,   // ACPIPower
		0x00,   // MultimediaWebback

		0x00,   // NULL

		0x00,   // MultimediaWebHome
		0x00,   // MultimediaStop

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x00,   // ACPISleep
		0x00,   // MultimediaMyComputer

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaEmail

		0x00,   // NULL

		0x54,   // Keypad /

		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaNext

		0x00,   // NULL
		0x00,   // NULL

		0x00,   // MultimediaSelect

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x58,   // Keypad Enter

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x00,   // ACPIWake

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x4D,   // End

		0x00,   // NULL

		0x50,   // LeftArrow
		0x4A,   // Home

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x49,   // Insert
		0x4C,   // Delete
		0x51,   // DownArrow

		0x00,   // NULL

		0x4F,   // RightArrow
		0x52,   // UpArrow

		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL
		0x00,   // NULL

		0x4E,   // PageDown

		0x00,   // NULL
		0x00,   // NULL

		0x4B,   // PageUp
	};

	static const uint32_t ScanCode2_US_E0_F0[2 * 128] =
	{
		0,      //              0
		0,      //              1
		0,      //              2
		0,      //              3
		0,      //              4
		0,      //              5
		0,      //              6
		0,      //              7
		0,      //              8
		0,      //              9
		0,      //              A
		0,      //              B
		0,      //              C
		0,      //              D
		0,      //              E
		0,      //              F
		0,      // R-MWsearch   10
		0,      // R-ralt       11
		0,      //              12
		0,      //              13
		0,      // R-rctrl      14
		0,      // R-Mprev      15
		0,      //              16
		0,      //              17
		0,      // R-MWfav      18
		0,      //              19
		0,      //              1A
		0,      //              1B
		0,      //              1C
		0,      //              1D
		0,      //              1E
		0,      // R-lsuper     1F
		0,      // R-MWrefrsh   20
		0,      // R-Mvoldown   21
		0,      //              22
		0,      // R-Mmute      23
		0,      //              24
		0,      //              25
		0,      //              26
		0,      //              27
		0,      // R-MWstop     28
		0,      //              29
		0,      //              2A
		0,      // R-Mcalc      2B
		0,      //              2C
		0,      //              2D
		0,      //              2E
		0,      // R-apps       2F
		0,      // R-MWfwd      30
		0,      //              31
		0,      // R-Mvolup     32
		0,      //              33
		0,      // R-Mply/pse   34
		0,      //              35
		0,      //              36
		0,      // R-Apower     37
		0,      // R-MWback     38
		0,      //              39
		0,      // R-MWhome     3A
		0,      // R-Mstop      3B
		0,      //              3C
		0,      //              3D
		0,      //              3E
		0,      // R-Asleep     3F
		0,      // R-Mmycom     40
		0,      //              41
		0,      //              42
		0,      //              43
		0,      //              44
		0,      //              45
		0,      //              46
		0,      //              47
		0,      // R-Memail     48
		0,      //              49
		0,      // R-k/         4A
		0,      //              4B
		0,      //              4C
		0,      // R-Mnext      4D
		0,      //              4E
		0,      //              4F
		0,      // R-Mselect    50
		0,      //              51
		0,      //              52
		0,      //              53
		0,      //              54
		0,      //              55
		0,      //              56
		0,      //              57
		0,      //              58
		0,      //              59
		0,      // R-kenter     5A
		0,      //              5B
		0,      //              5C
		0,      //              5D
		0,      // R-Awake      5E
		0,      //              5F
		0,      //              60
		0,      //              61
		0,      //              62
		0,      //              63
		0,      //              64
		0,      //              65
		0,      //              66
		0,      //              67
		0,      //              68
		0,      // R-end        69
		0,      //              6A
		0,      // R-left       6B
		0,      // R-home       6C
		0,      //              6D
		0,      //              6E
		0,      //              6F
		0,      // R-insert     70
		0,      // R-delete     71
		0,      // R-down       72
		0,      //              73
		0,      // R-right      74
		0,      // R-up         75
		0,      //              76
		0,      //              77
		0,      //              78
		0,      //              79
		0,      // R-pagedown   7A
		0,      //              7B
		0,      //              7C
		0       // R-pageup     7D
	};






	// this is temporary!!
	static const uint32_t USB_HID[4 * 0xFF] =
	{
		0x81,    0,      0,     0,      // SystemPower
		0x82,    0,      0,     0,      // SystemSleep
		0x83,    0,      0,     0,      // SystemWake
		0x00,    0,      0,     0,      // Nothing
		0x01,    0,      0,     0,      // OverrunError
		0x02,    0,      0,     0,      // POSTError
		0x03,    0,      0,     0,      // Undefined
		0x04,   'a',    'A',    0,      // A
		0x05,   'b',    'B',    0,      // B
		0x06,   'c',    'C',    0,      // C
		0x07,   'd',    'D',    0,      // D
		0x08,   'e',    'E',    0,      // E
		0x09,   'f',    'F',    0,      // F
		0x0A,   'g',    'G',    0,      // G
		0x0B,   'h',    'H',    0,      // H
		0x0C,   'i',    'I',    0,      // I
		0x0D,   'j',    'J',    0,      // J
		0x0E,   'k',    'K',    0,      // K
		0x0F,   'l',    'L',    0,      // L
		0x10,   'm',    'M',    0,      // M
		0x11,   'n',    'N',    0,      // N
		0x12,   'o',    'O',    0,      // O
		0x13,   'p',    'P',    0,      // P
		0x14,   'q',    'Q',    0,      // Q
		0x15,   'r',    'R',    0,      // R
		0x16,   's',    'S',    0,      // S
		0x17,   't',    'T',    0,      // T
		0x18,   'u',    'U',    0,      // U
		0x19,   'v',    'V',    0,      // V
		0x1A,   'w',    'W',    0,      // W
		0x1B,   'x',    'X',    0,      // X
		0x1C,   'y',    'Y',    0,      // Y
		0x1D,   'z',    'Z',    0,      // Z
		0x1E,   '1',    '!',    0,      // 1
		0x1F,   '2',    '@',    0,      // 2
		0x20,   '3',    '#',    0,      // 3
		0x21,   '4',    '$',    0,      // 4
		0x22,   '5',    '%',    0,      // 5
		0x23,   '6',    '^',    0,      // 6
		0x24,   '7',    '&',    0,      // 7
		0x25,   '8',    '*',    0,      // 8
		0x26,   '9',    '(',    0,      // 9
		0x27,   '0',    ')',    0,      // 0
		0x28,   '\n',   '\n',   0,      // Enter
		0x29,    0,      0,     0,      // Escape
		0x2A,   '\b',   '\b',   0,      // backspace
		0x2B,   '\t',   '\t',   0,      // Tab
		0x2C,   ' ',    ' ',    0,      // Space
		0x2D,   '-',    '_',    0,      // -
		0x2E,   '=',    '+',    0,      // =
		0x2F,   '[',    '{',    0,      // [
		0x30,   ']',    '}',    0,      // ]
		0x31,   '\\',   '|',    0,      // '\'
		0x32,    0,      0,     0,      // Europe1
		0x33,   ';',    ':',    0,      // ;
		0x34,   '\'',   '"',    0,      // '
		0x35,   '`',    '~',    0,      // `
		0x36,   ',',    '<',    0,      // ,
		0x37,   '.',    '>',    0,      // .
		0x38,   '/',    '?',    0,      // /
		0x39,    0,      0,     0,      // CapsLock
		0x3A,    0,      0,     0,      // F1
		0x3B,    0,      0,     0,      // F2
		0x3C,    0,      0,     0,      // F3
		0x3D,    0,      0,     0,      // F4
		0x3E,    0,      0,     0,      // F5
		0x3F,    0,      0,     0,      // F6
		0x40,    0,      0,     0,      // F7
		0x41,    0,      0,     0,      // F8
		0x42,    0,      0,     0,      // F9
		0x43,    0,      0,     0,      // F10
		0x44,    0,      0,     0,      // F11
		0x45,    0,      0,     0,      // F12
		0x46,    0,      0,     0,      // PrintScreen
		0x47,    0,      0,     0,      // ScrollLock
		0x48,    0,      0,     0,      // Break/Pause

		0x49,    0,      0,     0,      // Insert
		0x4A,    0,      0,     0,      // Home
		0x4B,    0,      0,     0,      // PageUp
		0x4C,    0,      0,     0,      // Delete
		0x4D,    0,      0,     0,      // End
		0x4E,    0,      0,     0,      // PageDown
		0x4F,    0,      0,     0,      // RightArrow
		0x50,    0,      0,     0,      // LeftArrow
		0x51,    0,      0,     0,      // DownArrow
		0x52,    0,      0,     0,      // UpArrow
		0x53,    0,      0,     0,      // NumLock
		0x54,   '/',    '/',    0,      // Keypad /
		0x55,   '*',    '*',    0,      // Keypad *
		0x56,   '-',    '-',    0,      // Keypad -
		0x57,   '+',    '+',    0,      // Keypad +
		0x58,   '\n',   '\n',   0,      // Keypad Enter
		0x59,   '1',    '1',    0,      // Keypad 1
		0x5A,   '2',    '2',    0,      // Keypad 2
		0x5B,   '3',    '3',    0,      // Keypad 3
		0x5C,   '4',    '4',    0,      // Keypad 4
		0x5D,   '5',    '5',    0,      // Keypad 5
		0x5E,   '6',    '6',    0,      // Keypad 6
		0x5F,   '7',    '7',    0,      // Keypad 7
		0x60,   '8',    '8',    0,      // Keypad 8
		0x61,   '9',    '9',    0,      // Keypad 9
		0x62,   '0',    '0',    0,      // Keypad 0
		0x63,   '.',    '.',    0,      // Keypad .
		0x64,    0,      0,     0,      // Europe 2
		0x65,    0,      0,     0,      // App
		0x66,    0,      0,     0,      // KeyboardPower
		0x67,   '=',    '=',    0,      // Keypad =
		0x68,    0,      0,     0,      // F13
		0x69,    0,      0,     0,      // F14
		0x6A,    0,      0,     0,      // F15
		0x6B,    0,      0,     0,      // F16
		0x6C,    0,      0,     0,      // F17
		0x6D,    0,      0,     0,      // F18
		0x6E,    0,      0,     0,      // F19
		0x6F,    0,      0,     0,      // F20
		0x70,    0,      0,     0,      // F21
		0x71,    0,      0,     0,      // F22
		0x72,    0,      0,     0,      // F23
		0x73,    0,      0,     0,      // F24

		0x74,    0,      0,     0,      // KeyboardExe
		0x75,    0,      0,     0,      // KeyboardHelp
		0x76,    0,      0,     0,      // KeyboardMenu
		0x77,    0,      0,     0,      // KeyboardSelect
		0x78,    0,      0,     0,      // KeyboardStop
		0x79,    0,      0,     0,      // KeyboardAgain
		0x7A,    0,      0,     0,      // KeyboardUndo
		0x7B,    0,      0,     0,      // KeyboardCut
		0x7C,    0,      0,     0,      // KeyboardCopy
		0x7D,    0,      0,     0,      // KeyboardPaste
		0x7E,    0,      0,     0,      // KeyboardFind
		0x7F,    0,      0,     0,      // KeybordMute
		0x80,    0,      0,     0,      // KeyboardVolUp
		0x81,    0,      0,     0,      // KeyboardVolDown
		0x82,    0,      0,     0,      // KeyboardCapsLock
		0x83,    0,      0,     0,      // KeyboardNumLock
		0x84,    0,      0,     0,      // KeyboardScrollLock
		0x85,   ',',    ',',    0,      // Keypad ,
		0x86,   '=',    '=',    0,      // Keyboard =
		0x87,    0,      0,     0,      // NULL
		0x88,    0,      0,     0,      // NULL
		0x89,    0,      0,     0,      // NULL
		0x8A,    0,      0,     0,      // NULL
		0x8B,    0,      0,     0,      // NULL
		0x8C,    0,      0,     0,      // NULL
		0x8D,    0,      0,     0,      // NULL
		0x8E,    0,      0,     0,      // NULL
		0x8F,    0,      0,     0,      // NULL
		0x90,    0,      0,     0,      // NULL
		0x91,    0,      0,     0,      // NULL
		0x92,    0,      0,     0,      // NULL
		0x93,    0,      0,     0,      // NULL
		0x94,    0,      0,     0,      // NULL
		0x95,    0,      0,     0,      // NULL
		0x96,    0,      0,     0,      // NULL
		0x97,    0,      0,     0,      // NULL
		0x98,    0,      0,     0,      // NULL
		0x99,    0,      0,     0,      // KeyboardAltErase
		0x9A,    0,      0,     0,      // KeyboardSysReq
		0x9B,    0,      0,     0,      // KeyboardCancel
		0x9C,    0,      0,     0,      // KeyboardClear
		0x9D,    0,      0,     0,      // KeyboardPrior
		0x9E,    0,      0,     0,      // KeyboardReturn
		0x9F,    0,      0,     0,      // KeyboardSeparator
		0xA0,    0,      0,     0,      // KeyboardOut
		0xA1,    0,      0,     0,      // KeyboardOperate
		0xA2,    0,      0,     0,      // KeyboardClear
		0xA3,    0,      0,     0,      // KeyboardProps
		0xA4,    0,      0,     0,      // KeyboardExSel
		0xA5,    0,      0,     0,      // NULL
		0xA6,    0,      0,     0,      // NULL
		0xA7,    0,      0,     0,      // NULL
		0xA8,    0,      0,     0,      // NULL
		0xA9,    0,      0,     0,      // NULL
		0xAA,    0,      0,     0,      // NULL
		0xAB,    0,      0,     0,      // NULL
		0xAC,    0,      0,     0,      // NULL
		0xAD,    0,      0,     0,      // NULL
		0xAE,    0,      0,     0,      // NULL
		0xAF,    0,      0,     0,      // NULL
		0xB0,    0,      0,     0,      // NULL
		0xB1,    0,      0,     0,      // NULL
		0xB2,    0,      0,     0,      // NULL
		0xB3,    0,      0,     0,      // NULL
		0xB4,    0,      0,     0,      // NULL
		0xB5,    0,      0,     0,      // NULL
		0xB6,    0,      0,     0,      // NULL
		0xB7,    0,      0,     0,      // NULL
		0xB8,    0,      0,     0,      // NULL
		0xB9,    0,      0,     0,      // NULL
		0xBA,    0,      0,     0,      // NULL
		0xBB,    0,      0,     0,      // NULL
		0xBC,    0,      0,     0,      // NULL
		0xBD,    0,      0,     0,      // NULL
		0xBE,    0,      0,     0,      // NULL
		0xBF,    0,      0,     0,      // NULL
		0xC0,    0,      0,     0,      // NULL
		0xC1,    0,      0,     0,      // NULL
		0xC2,    0,      0,     0,      // NULL
		0xC3,    0,      0,     0,      // NULL
		0xC4,    0,      0,     0,      // NULL
		0xC5,    0,      0,     0,      // NULL
		0xC6,    0,      0,     0,      // NULL
		0xC7,    0,      0,     0,      // NULL
		0xC8,    0,      0,     0,      // NULL
		0xC9,    0,      0,     0,      // NULL
		0xCA,    0,      0,     0,      // NULL
		0xCB,    0,      0,     0,      // NULL
		0xCC,    0,      0,     0,      // NULL
		0xCD,    0,      0,     0,      // NULL
		0xCE,    0,      0,     0,      // NULL
		0xCF,    0,      0,     0,      // NULL
		0xD0,    0,      0,     0,      // NULL
		0xD1,    0,      0,     0,      // NULL
		0xD2,    0,      0,     0,      // NULL
		0xD3,    0,      0,     0,      // NULL
		0xD4,    0,      0,     0,      // NULL
		0xD5,    0,      0,     0,      // NULL
		0xD6,    0,      0,     0,      // NULL
		0xD7,    0,      0,     0,      // NULL
		0xD8,    0,      0,     0,      // NULL
		0xD9,    0,      0,     0,      // NULL
		0xDA,    0,      0,     0,      // NULL
		0xDB,    0,      0,     0,      // NULL
		0xDC,    0,      0,     0,      // NULL
		0xDD,    0,      0,     0,      // NULL
		0xDE,    0,      0,     0,      // NULL
		0xDF,    0,      0,     0,      // NULL
		0xE0,    0,      0,     0,      // LeftControl
		0xE1,    0,      0,     0,      // LeftShift
		0xE2,    0,      0,     0,      // LeftAlt
		0xE3,    0,      0,     0,      // LeftSuper
		0xE4,    0,      0,     0,      // RightControl
		0xE5,    0,      0,     0,      // RightShift
		0xE6,    0,      0,     0,      // RightAlt
		0xE7,    0,      0,     0       // RightSuper
	};
}












