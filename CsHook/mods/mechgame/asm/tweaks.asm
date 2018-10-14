
offset 40248D
; lock scroll type 1 on y axis
xor edx, edx

offset 4106F5
; don't call fade in the normal spot
fnop
nop
fnop

offset 413750
; don't allocate map tiles
RETN

offset 41153C 
; allow backgrounds to be 2x as large
push 1E0
push 280

offset 41157E
; allow tilesets to be 4x as tall
push 400
push 100


offset 4115BA
; allow NPC slot 1 to be twice as large
push 1E0
push 280

offset 4115D0
; allow NPC slot 2 to be twice as large
push 1E0
push 280

offset 40F690
; boot to title screen
mov dword [EBP-4], 2

offset 410508
; esc to title screen
mov eax, 2

offset 410742
; esc to title screen
mov eax, 2

offset 4107A2
; esc to title screen
mov eax, 2


offset 41080B
; esc to title screen
mov eax, 2

offset 41A0B0
; skip drawing weapon icons honestly it can go in the other method
RETN;

offset 42245C
; fix yes/no box width
mov [ebp-50], 0EC

offset 4115AC
; allow longer number objects
push 70

offset 471B21
; make fall-thru tiles solid for NPCs
data 01

offset 498298
; make textbox bitmaps a bit wider
data F0

offset 421A0D
; make textbox cliprect a bit wider
mov [4A5B24], 120


offset 416103
; let the player move a lot faster upwards
cmp [49E670], -1000
jge 416119
mov [49E670], -1000

offset 4219F2
; make invuln timer not reset on events
add ecx, 123456
nop

offset 4162EF
; make spikes do 4 dommage
push 04

; recover all health on profile load
offset 41D463
mov word [49E6CC], ax
nop

