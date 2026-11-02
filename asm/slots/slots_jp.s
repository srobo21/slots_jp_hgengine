@ Overlay 152: the Japanese HeartGold slot machine game (Japanese overlay 77), relocated.
@ src/slots_launch.c reads the entry table below and checks the 'SLOT' word at 0x023C801C.
@ data/slots/slots_jp_blob.bin is made by relocate_jp_slots.py, which assumes this header is
@ exactly 0x20 bytes and the overlay starts at 0x023C8000 (src/slots/linker.ld).

    .section .init,"ax"
    .align 2
    .global SlotsJP_Header
SlotsJP_Header:
    .word SlotsJP_Blob + 0x01           @ 0x023C8000  Init  (Japanese 0x021E4E41)
    .word SlotsJP_Blob + 0x69           @ 0x023C8004  Main  (Japanese 0x021E4EA9)
    .word SlotsJP_Blob + 0xC5           @ 0x023C8008  Exit  (Japanese 0x021E4F05)

@ 0x023C800C: Coins_SetValue(u16 *coins, u32 value). The Japanese game calls this when it
@ closes, to write the coin total back; the US ROM no longer has it, so it lives here.
    .thumb
    .thumb_func
    .global Slots_CoinsSetValue
Slots_CoinsSetValue:
    ldr r2, 1f
    cmp r1, r2
    bls 2f
    mov r1, r2
2:  strh r1, [r0]
    bx lr
    .align 2
1:  .word 50000                         @ 0x023C8018
    .word 0x544F4C53                    @ 0x023C801C  'SLOT'

    .org 0x20                           @ fails to assemble if the header grew past 0x20

SlotsJP_Blob:                           @ 0x023C8020
    .incbin "data/slots/slots_jp_blob.bin"
