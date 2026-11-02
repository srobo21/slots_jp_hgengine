// Main hg-engine code (always loaded). Replaces VoltorbFlip_LaunchApp via the hooks file:
//   arm9 Slots_LaunchApp 0203FA38 2
//
// Launches the Japanese HeartGold slot machine game (overlay 152, see asm/slots/slots_jp.s).
// This rebuilds the Japanese launcher (Japanese ROM 0x0203F798) for the US ROM.
//
// The US casino_game command (ScrCmd_CasinoGame) still runs the Japanese per-machine luck
// roll (SlotLuckiness) before calling here, so `luck` is already the authentic 0-3 value:
// each day, Goldenrod gets 7 machines at 1 and 3 at 2 (the rest 0); machine 21 is always 3.

#include "../include/types.h"
#include "../include/pokemon.h"            // FieldSystem (savedata at 0xC)
#include "../include/save.h"               // Save_GameStats_Get
#include "../include/constants/file.h"     // OVERLAY_SLOTS

typedef BOOL (*SlotsAppFunc)(void *man, int *state);

typedef struct {
    SlotsAppFunc init;
    SlotsAppFunc main;
    SlotsAppFunc exit;
    u32 ovyId;
} SlotsOverlayTemplate;

// What the Japanese slot game reads from its arguments (28 bytes)
typedef struct {
    /* 0x00 */ u32   unk0;
    /* 0x04 */ u32   luck;            // 0-3
    /* 0x08 */ void *gameStats;       // Save_GameStats_Get(save)
    /* 0x0C */ u32   unkC;
    /* 0x10 */ u32   unk10;
    /* 0x14 */ void *saveData;
    /* 0x18 */ void *menuInputState;  // &fieldSystem->menuInputState
} SlotsJPArgs;

#define FIELDSYS_MENU_INPUT_STATE_OFFSET 0x10C   // same in the Japanese and US ROMs

void LONG_CALL FieldSystem_LaunchApplication(FieldSystem *fsys, const SlotsOverlayTemplate *tmpl, void *args);

// The overlay is linked separately, so this file can't name its functions directly.
// It starts with a table of Init/Main/Exit (asm/slots/slots_jp.s); these wrappers call
// through it once the overlay is loaded.
#define SLOTS_ENTRY_TABLE ((const SlotsAppFunc *)0x023C8000)
#define SLOTS_MAGIC_ADDR  ((volatile u32 *)0x023C801C)
#define SLOTS_MAGIC       0x544F4C53   // 'SLOT'

static BOOL Slots_InitWrapper(void *man, int *state)
{
    if (*SLOTS_MAGIC_ADDR != SLOTS_MAGIC) {
        return FALSE;   // overlay didn't load; don't jump into empty memory
    }
    return SLOTS_ENTRY_TABLE[0](man, state);
}
static BOOL Slots_MainWrapper(void *man, int *state) { return SLOTS_ENTRY_TABLE[1](man, state); }
static BOOL Slots_ExitWrapper(void *man, int *state) { return SLOTS_ENTRY_TABLE[2](man, state); }

static const SlotsOverlayTemplate sSlotsTemplate = {
    Slots_InitWrapper, Slots_MainWrapper, Slots_ExitWrapper, OVERLAY_SLOTS
};

void *Slots_LaunchApp(FieldSystem *fsys, u32 luck)
{
    void *save = fsys->savedata;
    SlotsJPArgs *a = sys_AllocMemoryLo(11 /* HEAP_ID_FIELD2 */, sizeof(SlotsJPArgs));

    memset(a, 0, sizeof(SlotsJPArgs));
    a->saveData       = save;
    a->menuInputState = (u8 *)fsys + FIELDSYS_MENU_INPUT_STATE_OFFSET;
    a->gameStats      = Save_GameStats_Get(save);
    a->luck           = luck;

    FieldSystem_LaunchApplication(fsys, &sSlotsTemplate, a);
    return a;   // the script waits for the game to close, then frees this
}
