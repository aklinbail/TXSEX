/*******************************************************************
DX7 CC to Sysex Translator for Akai Force/Raspberry PI
Author: Amit Talwar https://www.amitszone.com
Github: https://github.com/intelliriffer
*****************************************************************


SYSTEM EXCLUSIVE DATA FORMAT
The TX81Z has three types of System Exclusive message; Parameter Change messages, Bulk Data messages and
Dump Request messages.

PARAMETER CHANGE MESSAGES
These messages change the value of a parameter in TX81Z memory. There are
8 subgroups of Parameter Changes; VCED, ACED, PCED, Remote Switch,
Micro Tuning, Program Change, Effect data and System data.

VCED, ACED, PCED and Remote Switch parameter change messages have the
following format.
F0h        Exclusive
43h        I.D. number (Yamaha)
1nh        Basic receive channel
0ggggghh   ggggg = Group number, hh = Subgroup number
0ppppppp   ppppppp = Parameter number
0ddddddd   ddddddd = Data
F7h        End Of Exclusive

* VCED (Voice parameters compatible with DX21/27/100)
  ggggg = 00100 (4), hh = 10 (2)
  See p.71 for parameter numbers and data.

* ACED (Additional voice parameters for TX81Z)
  ggggg = 00100 (4), hh = 11 (3)
  See p.73 for parameter numbers and data.

* PCED (Performance parameters)
  ggggg = 00100 (4), hh = 11 (3)
  See p.74 for parameter numbers and data.

* Remote Switch (The same effect as pressing a switch on the TX81Z front
  panel, i.e., 'remote control'.)
  ggggg = 00100 (4), hh = 11 (3), ddddddd = 0 (off), 7F (on)
  See p.75 for switch numbers.

System Parameter Change (basic receive channel settings, etc.) and Effect
Parameter Change (data for delay, pan and chord) messages have the following
format.
F0h        Exclusive
43h        I.D. number (Yamaha)
1nh        Basic receive channel
0ggggghh   ggggg = 00100 (4), hh = 00 (0)
0ppppppp   ppppppp = 1111011 (123) = System Parameter
           ppppppp = 1111100 (124) = Effect Parameter
0kkkkkkk   kkkkkkk = Parameter number

Micro Tune parameter change messages have the following format.
F0h        Exclusive
43h        I.D. number (Yamaha)
1nh        Basic receive channel
0ggggghh   ggggg = 00100 (4), hh = 00
0ppppppp   ppppppp = 1111101 (125) OCT
           ppppppp = 1111110 (126) FULL
0kkkkkkk   kkkkkkk = key number
0hhhhhhh   hhhhhhh = note C#-1 to C7 (13-108)
           = data fine tuning 0 to +31, -31 to -1 (0-32, 33-63)
F7h        End Of Exclusive

Program Change Table parameter change messages have the following format.
The data is 0-184d, indicating the TX81Z memory to be selected in response to
the incoming program change number. 0-31 (I1-I32), 32-63 (A1-A32), 64-95
(B1-B32), 96-127 (C1-C32), 128-160 (D1-D32), 161-184 (PF1-PF24)

F0h        Exclusive
43h        I.D. number (Yamaha)
1nh        Basic receive channel
0ggggghh   ggggg = 00100 (4), hh = 00
0ppppppp   ppppppp = 1111111 (127)
0kkkkkkk   kkkkkkk = program change number
0hhhhhhh   hhhhhhh = data (high)
0lllllll   lllllll = data (low)
F7h        End Of Exclusive
*/
/* Source: TX81Z Owner’s Manual PDF, pp.68–69 (Parameter Change Messages section). [1](https://usa.yamaha.com/files/download/other_assets/9/316769/TX81ZE.pdf)

============================================================
 TX81Z — PARAMETER TABLES (Pages 71–73)
 Source: Yamaha TX81Z Owner’s Manual PDF
============================================================

------------------------------------------------------------
VOICE EDIT PARAMETERS (VCED)
------------------------------------------------------------
Parameter number | Parameter                       | LCD          | Data
-----------------|----------------------------------|--------------|-------------------------
0                | Attack Rate                      | AR           | 0–31
1                | Decay 1 Rate                     | D1R          | 0–31
2                | Decay 2 Rate                     | D2R          | 0–31
3                | Release Rate                     | RR           | 1–15
4                | Decay 1 Level                    | D1L          | 0–15
5                | Level Scaling                    | LS           | 0–99
6                | Rate Scaling                     | RS           | 0–3
7                | EG Bias Sensitivity              | EBS          | 0–7
8                | Amplitude Modulation Enable      | AME          | 0–1
9                | Key Velocity Sensitivity         | KVS          | 0–7
10               | Operator Output Level            | OUT          | 0–99
11               | Frequency (coarse)               | CRS          | 0–63
12               | Detune                            | DET          | 0–6 (3=center)
13–25            | Operator 3 parameters            | (same order) | (same ranges)
26–38            | Operator 2 parameters            | (same order) | (same ranges)
39–51            | Operator 1 parameters            | (same order) | (same ranges)
52               | Algorithm                         | ALG          | 0–7
53               | Feedback                          | Feedback     | 0–7
54               | LFO Speed                         | Speed        | 0–99
55               | LFO Delay                         | Delay        | 0–99
56               | Pitch Modulation Depth            | P Mod Depth  | 0–99
57               | Amp Modulation Depth              | A Mod Depth  | 0–99
58               | LFO Sync                          | Sync         | 0–1
59               | LFO Wave                          | Wave         | 0–3
60               | Pitch Mod Sensitivity             | P Mod Sens   | 0–7
61               | Amplitude Mod Sensitivity         | AMS          | 0–3
62               | Transpose                         | Middle C =   | 0–48 (24=center)
63               | Poly/Mono                         | Poly Mode    | 0–1
64               | Pitch Bend Range                  | P Bend Range | 0–12
65               | Portamento Mode                   | Full Time    | 0–1
66               | Portamento Time                   | Porta Time   | 0–99
67               | Foot Control Volume               | FC Volume    | 0–99
68               | Sustain                            | (none)       | 0–1
69               | Portamento                         | (none)       | 0–1
70               | Chorus (not used)                  | (none)       | Always 0
71               | Mod Wheel Pitch                   | MW Pitch     | 0–99
72               | Mod Wheel Amplitude               | MW Ampl      | 0–99
73               | Breath Control Pitch              | BC Pitch     | 0–99
74               | Breath Control Amplitude          | BC Ampl      | 0–99
75               | Breath Control Pitch Bias         | BC PitchBias | 0–99 (50=center)
76               | Breath Control EG Bias            | BC EG Bias   | 0–99
77–86            | Voice Name Characters 1–10        | ASCII        | 32–127
93               | Operators 4–1 On/Off (bit mask)   | —            | 0–15 (1=on)
(Parameters 87–92 are unused)

------------------------------------------------------------
ADDITIONAL VOICE EDIT PARAMETERS (ACED)
------------------------------------------------------------
Parameter number | Parameter                  | LCD    | Data
-----------------|-----------------------------|--------|----------------------------
0                | Fixed Frequency Mode        | FIX    | 0–1
1                | Fixed Frequency Range       |        | 0–7  (0=250 Hz … 7=32 kHz)
2                | Fine Frequency (fixed)      | FIN    | 0–15
3                | Operator Waveform           | OSW    | 0–7
4                | EG Shift                    | SHFT   | 0–3 (0=96dB, 1=48dB, 2=24dB, 3=12dB)
5–9              | Operator 3 extra params     | —      | same order/type as op4
10–14            | Operator 2 extra params     | —      | same order/type
15–19            | Operator 1 extra params     | —      | same order/type
20               | Reverb Rate                 | REV    | 0–7 (0=off, 7=fast)
21               | FC Pitch                    | FC Pitch | 0–99
22               | FC Amplitude                | FC Amplitude | 0–99

------------------------------------------------------------
PERFORMANCE EDIT PARAMETERS (PCED)
------------------------------------------------------------
Parameter number | Parameter               | LCD          | Data
-----------------|--------------------------|--------------|-------------------------
0                | Maximum Notes            | MAX NOTES    | 0–8
1                | Voice Number MSB         | —            | (0–127 encoded)
2                | Voice Number LSB         | 101–D32      | 0–127
3                | Receive Channel          | RECEIVE CH   | 0–16 (16=omni)
4                | Low Note Limit           | LIMIT/L      | 0–127 (C–2..G8)
5                | High Note Limit          | LIMIT/H      | 0–127 (C–2..G8)
6                | Instrument Detune        | INST DETUNE  | 0–14 (7=center)
7                | Note Shift               | NOTE SHIFT   | 0–48 (24=center)
8                | Volume                   | VOL          | 0–99
9                | Output Assign            | OUT ASSIGN   | 0–3  (0=off, 1=I, 2=II, 3=I+II)
10               | LFO Select               | LFO SELECT   | 0–3  (0=off, 1=inst1, 2=inst2, 3=vib)
11               | Micro Tune Enable        | —            | 0–1
12–23            | Instrument 2 parameters  | —            | Same structure
24–35            | Instrument 3 parameters  | —            | Same
36–47            | Instrument 4 parameters  | —            | Same
48–59            | Instrument 5 parameters  | —            | Same
60–71            | Instrument 6 parameters  | —            | Same
72–83            | Instrument 7 parameters  | —            | Same
84–95            | Instrument 8 parameters  | —            | Same
96               | Micro Tune Table          | MICTUN       | 0–12 (0=Oct, 1=Full, 2–12=presets)
97               | Assign Mode               | ASMODE       | 0–1 (0=norm, 1=altr)
98               | Effect Select             | EFSEL        | 0–3 (off/Delay/Pan/Chord)
99               | Key (for microtuning)     | KEY          | 0–11 (C–B)
100–109          | Performance Name chars 1–10 | ASCII      | 32–127

------------------------------------------------------------
REMOTE SWITCH PARAMETERS
------------------------------------------------------------
Parameter number | Parameter
-----------------|---------------------------
64               | POWER ON (reset)
65               | STORE
66               | UTILITY
67               | EDIT
68               | PLAY
69               | PARAMETER -1
70               | PARAMETER +1
71               | DATA ENTRY -1
72               | DATA ENTRY +1
73               | MASTER VOLUME -1
74               | MASTER VOLUME +1
75               | CURSOR

(Data: 0 = switch off, 127 = switch on)
*/

#include <unistd.h>
#include <sys/time.h>
#include <ctime>
#include "RtMidi.h"
#include <chrono>
#include <csignal>
const unsigned char nouts = 16;
using namespace std;
using std::chrono::duration_cast;
using std::chrono::seconds;
using std::chrono::system_clock;

const string PORT_PREFIX = "DX4OP";
void onMIDI(double deltatime, std::vector<unsigned char> *message, void * /*userData*/);
int limit(int val, int min, int max);
unsigned char validCC[14] = {1, 2, 7, 10, 64, 66, 120, 121, 122, 123, 124, 125, 126, 127};
void print();
void cleanup();
void listInports();
void initHWPORT();
void signalHandler(int signum);
string oPORTNAME = "";
bool HW_EXISTS = false;
void listOutPorts();
long long getSecs();
int getOutPort(std::string str);
int getInPort(std::string str);
long long nextCheck = 0;
void sendMessage(vector<unsigned char> *message);
vector<unsigned char> BASE_SYX{0xF0, 0x43, 0x10, 0, 0, 0, 0xF7};
enum BPOS
{
    GROUP = 3,
    PARAMETER = 4,
    DATA = 5

};
enum CCTYPES
{
    SYSTEM,
    SYSEX,
    SKIP,
    CC
};

struct CC_MAPPING
{
    //  int x = 0;
    CC_MAPPING(CCTYPES TYPE, int CC, int MIN, int MAX, int GROUP, int PARAMETER) : TYPE(TYPE), CC(CC), MIN(MIN), MAX(MAX), GROUP(GROUP), PARAMETER(PARAMETER){};
    CCTYPES TYPE = SKIP;
    int CC = 0;
    int MIN = 0;
    int MAX = 99;
    int GROUP = 0;
    int PARAMETER = 0;
};
//note - 4 op synths need value of 12 or 13 for the groups 
CC_MAPPING MAP[128] = {
    {SYSEX, 0, 0, 1, 12, 63},    //0  Poly Mono mode 
    {SYSEX, 1, 0, 48, 12, 62},   // 1 Transpose
    {CC, 2, 0, 127, 0, 0},       // breath
    {SYSEX, 3, 0, 99, 12, 54},   // 3 LFO SPEED
    {CC, 4, 0, 127, 0, 0},       // Foot
    {CC, 5, 0, 127, 0, 0},       // Portamento
    {SYSEX, 6, 0, 99, 12, 55},   // LFO DELAY
    {CC, 7, 0, 127, 0, 0},       // 7 Volume
    {SYSEX, 8, 0, 99, 12, 56},   // 8 LFO PMD
    {SYSEX, 9, 0, 99, 12, 57},   // 9 LFO AMD
    {CC, 10, 0, 127, 0, 0},      // 10 PAN
    {SYSEX, 11, 0, 12, 12, 64},   // 11  Pitch Bend Range
    {SYSEX, 12, 0, 3, 12, 59},    // 12 LFO WAVE
    {SYSEX, 13, 0, 1, 12, 58},   // 13 LFO Sync
    {SYSEX, 14, 0, 7, 12, 60},   // 14 LFO PMS
    {SYSEX, 15, 0, 3, 12, 61},    // 15 LFO AMS
    {SYSEX, 16, 0, 1, 12, 65},    // 16 Portamento Mode
    {SYSEX, 17, 0, 99, 12, 66},   // 17 Portamento Time
    {SYSEX, 18, 0, 99, 12, 67},    // 18 FC Volume 
    {SYSEX, 19, 0, 1, 12, 68},    // 19 Sustain
    {SYSEX, 20, 0, 99, 12, 69},  // 20 Portamento
    {SYSEX, 21, 0, 99, 0, 84},   // 21 Mod Wheel  Pitch
    {SYSEX, 22, 0, 99, 0, 63},   // 22 Mod Wheel Amplitude
    {SYSEX, 23, 0, 99, 0, 42},   // 23 a Rate op4
    {SYSEX, 24, 0, 99, 0, 21},   // 24
    {SYSEX, 25, 0, 99, 0, 0},    // 25 op6
    {SYSEX, 26, 0, 99, 0, 106},  // 26 Decay op1
    {SYSEX, 27, 0, 99, 0, 85},   // 27
    {SYSEX, 28, 0, 99, 0, 64},   // 28
    {SYSEX, 29, 0, 99, 0, 43},   // 29
    {SYSEX, 30, 0, 99, 0, 22},   // 30
    {SYSEX, 31, 0, 99, 0, 1},    // 1
    {SYSEX, 32, 0, 99, 0, 107},  // Sus op1
    {SYSEX, 33, 0, 99, 0, 86},   // 1
    {SYSEX, 34, 0, 99, 0, 65},   // 1
    {SYSEX, 35, 0, 99, 0, 44},   // 1
    {SYSEX, 36, 0, 99, 0, 23},   // 1
    {SYSEX, 37, 0, 99, 0, 2},    // 1
    {SYSEX, 38, 0, 99, 0, 108},  // 1 REl op1
    {SYSEX, 39, 0, 99, 0, 87},   // 1
    {SYSEX, 40, 0, 99, 0, 66},   // 1
    {SYSEX, 41, 0, 99, 0, 45},   // 1
    {SYSEX, 42, 0, 99, 0, 24},   // 1
    {SYSEX, 43, 0, 99, 0, 3},    //
    {SYSEX, 44, 0, 31, 0, 123},  // 1 Coarse op1
    {SYSEX, 45, 0, 31, 0, 102},  // 1
    {SYSEX, 46, 0, 31, 0, 81},   // 1
    {SYSEX, 47, 0, 31, 0, 60},   // 1
    {SYSEX, 48, 0, 31, 0, 39},   // 1
    {SYSEX, 49, 0, 31, 0, 18},   // 1
    {SYSEX, 50, 0, 99, 0, 124},  // 1 Fine Op1
    {SYSEX, 51, 0, 99, 0, 103},  // 1
    {SYSEX, 52, 0, 99, 0, 82},   // 1
    {SYSEX, 53, 0, 99, 0, 61},   // 1
    {SYSEX, 54, 0, 99, 0, 40},   // 1
    {SYSEX, 55, 0, 99, 0, 19},   // 1
    {SKIP, 56, 0, 127, 0, 0},    // 1
    {SKIP, 57, 0, 127, 0, 0},    // 1
    {SKIP, 58, 0, 127, 0, 0},    // 1
    {SKIP, 59, 0, 127, 0, 0},    // 1
    {SKIP, 60, 0, 127, 0, 0},    // 1
    {SKIP, 61, 0, 127, 0, 0},    // 1
    {SKIP, 62, 0, 127, 0, 0},    // 1
    {SKIP, 63, 0, 127, 0, 0},    // 1
    {CC, 64, 0, 127, 0, 0},      // Sustain
    {SKIP, 65, 0, 127, 0, 0},    // 1
    {CC, 66, 0, 127, 0, 0},      // Sostenuto
    {SKIP, 67, 0, 127, 0, 0},    // 1
    {SKIP, 68, 0, 127, 0, 0},    // 1
    {SKIP, 69, 0, 127, 0, 0},    // 1
    {SKIP, 70, 0, 127, 0, 0},    // 1
    {CC, 71, 0, 127, 0, 0},      // 1 Resonane For Dexed -
    {SKIP, 72, 0, 127, 0, 0},    // 1
    {SYSEX, 73, 0, 48, 1, 16},   // Transpose
    {CC, 74, 0, 127, 0, 0},      // Curoff for Dexed Midi Learn - not required 
    {SYSEX, 75, 0, 7, 1, 7},     // Feedback
    {SYSEX, 76, 0, 31, 1, 6},    // Algorithm
    {SKIP, 77, 0, 127, 0, 0},    // 1
    {SYSEX, 78, 0, 99, 0, 109},  // Atk level 1
    {SYSEX, 79, 0, 99, 0, 110},  //  dc 1 lvl 1
    {SYSEX, 80, 0, 99, 0, 111},  // sus lvl 1
    {SYSEX, 81, 0, 99, 0, 112},  // rel lvl 1
    {SYSEX, 82, 0, 99, 0, 88},   // a lvl 2
    {SYSEX, 83, 0, 99, 0, 89},   // 1
    {SYSEX, 84, 0, 99, 0, 90},   // 1
    {SYSEX, 85, 0, 99, 0, 91},   // 1
    {SYSEX, 86, 0, 99, 0, 67},   // op3
    {SYSEX, 87, 0, 99, 0, 68},   // 1
    {SYSEX, 88, 0, 99, 0, 69},   // 1
    {SYSEX, 89, 0, 99, 0, 70},   // 1
    {SYSEX, 90, 0, 99, 0, 46},   // op 4
    {SYSEX, 91, 0, 99, 0, 47},   // 1
    {SYSEX, 92, 0, 99, 0, 48},   // 1
    {SYSEX, 93, 0, 99, 0, 49},   // 1
    {SYSEX, 94, 0, 99, 0, 25},   // op5
    {SYSEX, 95, 0, 99, 0, 26},   // 1
    {SYSEX, 96, 0, 99, 0, 27},   // 1
    {SYSEX, 97, 0, 99, 0, 28},   // 1
    {SYSEX, 98, 0, 99, 0, 4},    // op6
    {SYSEX, 99, 0, 99, 0, 5},    // 1
    {SYSEX, 100, 0, 99, 0, 6},   // 1
    {SYSEX, 101, 0, 99, 0, 7},   // 1
    {SYSEX, 102, 0, 99, 0, 121}, // op level 1
    {SYSEX, 103, 0, 99, 0, 100}, // 1
    {SYSEX, 104, 0, 99, 0, 79},  // 1
    {SYSEX, 105, 0, 99, 0, 58},  // 1
    {SYSEX, 106, 0, 99, 0, 37},  // 1
    {SYSEX, 107, 0, 99, 0, 16},  // 1
    {SKIP, 108, 0, 127, 0, 0},   // 1
    {SKIP, 109, 0, 127, 0, 0},   // 1
    {SKIP, 110, 0, 127, 0, 0},   // 1
    {SKIP, 111, 0, 127, 0, 0},   // 1
    {SKIP, 112, 0, 127, 0, 0},   // 1
    {SKIP, 113, 0, 127, 0, 0},   // 1
    {SKIP, 114, 0, 127, 0, 0},   // 1
    {SKIP, 115, 0, 127, 0, 0},   // 1
    {SKIP, 116, 0, 127, 0, 0},   // 1
    {SKIP, 117, 0, 127, 0, 0},   // 1
    {SKIP, 118, 0, 127, 0, 0},   // 1
    {SKIP, 119, 0, 127, 0, 0},   // 1
    {SYSTEM, 120, 0, 127, 0, 0}, // 1
    {SYSTEM, 121, 0, 127, 0, 0}, // 1
    {SYSTEM, 122, 0, 127, 0, 0}, // 1
    {SYSTEM, 123, 0, 127, 0, 0}, // 1
    {SYSTEM, 124, 0, 127, 0, 0}, // 1
    {SYSTEM, 125, 0, 127, 0, 0}, // 1
    {SYSTEM, 126, 0, 127, 0, 0}, // 1
    {SYSTEM, 127, 0, 127, 0, 0}, // 1

};

RtMidiIn *midiIn = 0;
RtMidiOut *SYX = 0;
RtMidiOut *HWOUT = 0;

int main(int argc, char *argv[])
{
    midiIn = new RtMidiIn();
    midiIn->setCallback(&onMIDI);
    midiIn->ignoreTypes(false, false, true); // dont ignore clocK
    SYX = new RtMidiOut();
    HWOUT = new RtMidiOut();
    signal(SIGINT, signalHandler);

    if (argc > 1)
    {
        string cmd(argv[1]);
        cout << "Command: " << cmd << endl;
        if (cmd == "-ports")
        {
            listOutPorts();
            cout << endl
                 << endl;
            //     listInports();
            cleanup();
        }
        /*  if (cmd == "-print")
          {

              cleanup();
          }*/
        if (cmd == "-p")
        {
            if (argc < 3)
            {
                cout << "Error ! Please Provide Midi Port Name to bind to!" << endl;
                cleanup();
            }
            oPORTNAME = string(argv[2]);
            initHWPORT();
        }
    }

    if (oPORTNAME == "")
    {
        SYX->openVirtualPort(PORT_PREFIX + "SYX");
        cout << "dxsex => Created Virtual Output Port: " << PORT_PREFIX << "SYX" << endl;
    }
    midiIn->openVirtualPort(PORT_PREFIX + "CC");
    cout << "dxsex => Created Virtual Input Port: " << PORT_PREFIX << "CC" << endl;
    cout << "Send Your CC Commands to PORT: " << PORT_PREFIX << "CC" << endl;

    while (true) // the main loop
    {

        if (oPORTNAME != "")
        {
            long elapsed = getSecs() - nextCheck;
            if (elapsed >= 2)
            {
                int pid = getOutPort(oPORTNAME);
                if (pid == -1)
                {
                    HW_EXISTS = false;
                }
                else
                {
                    if (HW_EXISTS == false)
                    {
                        initHWPORT();
                    }
                }
                nextCheck = getSecs() + 2;
            }
        }

        usleep(5000);
    }
    cleanup();
    return 0;
}

void onMIDI(double deltatime, std::vector<unsigned char> *message, void * /*userData*/)
{
    static auto lastSysexTime = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();

    // ... (Your existing status/type checks) ...

    if (C.TYPE == SYSEX)
    {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSysexTime).count();

        if (elapsed >= 30) // Only send if the "window" is open
        {
            // ... (Your translation logic) ...

            sendMessage(&oSYX);
            lastSysexTime = now; // Reset the window
        }
        // else: DO NOTHING. Just exit the function.
        // This "thins" the data without pausing the CPU.
    }
}

int limit(int v, int min, int max)
{
    if (v < min)
        v = min;
    if (v > max)
        v = max;
    return v;
}

void listInports()
{
    uint nPorts = midiIn->getPortCount();
    cout << "************ INPUTS ************" << endl;
    for (uint i = 0; i < nPorts; i++)
    {
        std::string portName = midiIn->getPortName(i);
        std::cout << portName << "\n";
    }
}
void listOutPorts()
{
    uint nPorts = SYX->getPortCount();
    cout << "************ Midi Outputs ************" << endl;
    for (uint i = 0; i < nPorts; i++)
    {
        std::string portName = SYX->getPortName(i);
        std::cout << portName << "\n";
    }
}
void cleanup()
{
    delete midiIn;
    delete SYX;
    HWOUT->closePort();
    delete HWOUT;
    exit(0);
}

int getinPort(std::string str)
{
    int nPorts = midiIn->getPortCount();
    for (int i = 0; i < nPorts; i++)
    {
        std::string portName = midiIn->getPortName(i);
        size_t found = portName.find(str);
        if (found != string::npos)
        {
            return i;
        }
    }
    return 99;
}
int getOutPort(std::string str)
{
    int nPorts = SYX->getPortCount();
    for (int i = 0; i < nPorts; i++)
    {
        std::string portName = SYX->getPortName(i);
        size_t found = portName.find(str);
        if (found != string::npos)
        {
            return i;
        }
    }
    return -1;
}
void initHWPORT()
{
    int oid = getOutPort(oPORTNAME);
    if (oid != -1)
    {
        if (HWOUT->isPortOpen())
        {
            HWOUT->closePort();
        }
        try
        {
            HWOUT->openPort((unsigned int)oid, PORT_PREFIX + "SYX");
            HW_EXISTS = true;
            cout << "Opened HW Port (" << SYX->getPortName(oid) << " as " << PORT_PREFIX << "SYX) for Output with ID: " << oid << endl;
        }
        catch (...)
        {
            HW_EXISTS = false;
            cout << "Error Opening: " << SYX->getPortName(oid) << "for Output" << endl;
        }
    }
    else
    {
        HW_EXISTS = false;
        cout << oPORTNAME << "Not Available Yet" << endl;
    }
}
void sendMessage(vector<unsigned char> *message)
{
    if (oPORTNAME == "")
        SYX->sendMessage(message);
    else
    {

        try
        {
            HWOUT->sendMessage(message);
        }
        catch (...)
        {
            cout << "Error Sendind Midi to: " << oPORTNAME << endl;
        }
    }
}
long long getSecs() // gets time since epch in seconds
{
    auto t1 = std::chrono::system_clock::now();
    long long us = duration_cast<seconds>(t1.time_since_epoch()).count();
    return us;
}
void signalHandler(int signum)
{
    cout << "Interrupt signal (" << signum << ") received.\n";
    cout << "Process dxsex Terminiated!" << endl;
    cleanup();
}