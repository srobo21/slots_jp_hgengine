# slots_jp_hgengine
Port of the Slot Machine game from the Japanese version of Pokemon Heartgold into hg-engine

# Installation
This replaces Voltorb Flip with the real Japanese HeartGold slots game:
Japanese overlay 77, moved into hg-engine's overlay 152 with every call re-pointed at the
US ROM. Graphics and text need to come from a legally sourced Japanese HeartGold ROM.

What you need from an unpacked Japanese HeartGold ROM (using a program such as DSPRE)
  - `arm9_overlays/ov077.bin`       - the slots code
  - `arm9/arm9.bin`               - used only to double-check addresses
  - `files/a/1/2/7`                - the slots graphics (the US ROM has this archive, but blanked)

1. Additions that need to be made to your hg-engine repository.
      - In /hooks: `arm9 Slots_LaunchApp 0203FA38 2`
      - In /include/constants/file.h: `#define OVERLAY_SLOTS 152`
      - In /rom.ld: `FieldSystem_LaunchApplication = 0x0203DFB4 | 1;`

2. Extract the archive into your hg-engine root.

3. Build the relocated code (from the hg-engine root, after at least one make, "JP" is your unpacked JP HeartGold ROM root directory):
     - `python3 relocate_jp_slots.py <JP>/arm9_overlays/ov077.bin --us-arm9 base/arm9.bin --jp-arm9 <JP>/arm9/arm9.bin --out data/slots/slots_jp_blob.bin`
   
   It checks all 142 functions against your ROM first and stops if any address is wrong.
   If you ever re-run it, delete build/slots/slots_jp.o so make picks up the new file.

4. Text: The script points the game at text bank 854. If data/text/854.txt is already used,
   rename the file to the next free number and add  --bank <number>  in step 3.

5. Graphics
     - `cp <JP>/files/a/1/2/7 base/root/a/1/2/7`

6. make

# Notes
How luck works
   casino_game <machine>, 0  -> each day 7 Goldenrod machines get luck 1 and 3 get luck 2;
   the rest get 0. Machine 21 (the 13th machine in the script) always gets 3.

This does not make any map changes nor does it add the dialogue/scripts for the old Game Corner. Those edits need to be made separately. As it stands, if this is plugged into your hg-engine repository, it will simply change the Voltorb Flip game to the Slot Machine.

# AI Disclosure
This repository was generated entirely by Claude Opus 5.5.
