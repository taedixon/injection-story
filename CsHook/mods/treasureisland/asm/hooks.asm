
offset 413770
; hook load map function
; mapdata+0
JMP [4937B0]

offset 413930
; hook freeing map tiles
; mapdata+1
JMP [4937B4]

offset 413AF0
; hook drawing background map tiles
; mapdata+2
JMP [4937B8]

offset 413C60
; hook drawing foreground map tiles
; mapdata+3
JMP [4937BC];

offset 416470
; hook UNI0001 movement
; mapdata +4
JMP [4937C0]

offset 414BF0
; hook player frame calc
; mapdata +5
JMP [4937C4]

offset 415250
; hook player draw
; mapdata +6
JMP [4937C8]

offset 417E40
; hook player tile collision check
; mapdata +7
JMP [4937CC]

offset 40F9B0
; hook the title screen
; this is a bit tricky bc the title screen is
; where the DLL itself actually gets called for the first time.
; so, we do some regular asm stuff to make sure that gets done.
; mapdata +8
push ebp
mov ebp, esp
sub esp, 10
call 409650 ; wipe effects
call 420FA0 ; wipe tsc
call 416D40 ; clear stream sound
lea eax, [ebp-4]
push eax
sub eax, 4
push eax
sub eax, 4
push eax
sub eax, 4
push eax
call 410990 ; get version number
add esp, 10
push [ebp+8]
call [4937D0] ; call hook
add esp, 4
mov esp, ebp
pop ebp
retn

offset 40902A
; hook bullet functions
; here we want to do something a bit different than vanilla cave story, and
; use a table of function pointers (as entities do) rather than a big switch
; statement for each type of bullet.
; mapdata +9
mov ecx, [ebp-8] ; bullet ID
sub ecx, 1
mov [ebp-8], ecx
cmp ecx, 2C
JA 4094F8

; obtain the shot's level by taking shot.ID%3 + 1
mov eax, ecx
mov dl, 3
idiv dl
movzx eax, ah
inc eax
push eax

mov eax, [ebp-4]
shl eax, 7
mov edx, [4937D4] ; hooked table
add eax, 499C98
push eax
call [edx + ecx*4]
add esp, 8

jmp 4094F8

offset 403DC0
; hook draw bullet function
; mapdata +10
JMP [4937D8]


; do the regular stuff
mov edx, [ebp-10]
cmp edx, 3
ja 403EFF
jmp [edx*4+403F69]

mov eax, [ebp-0C]
shl eax, 7
add eax, 499C98
mov edx, [eax+10]
sub edx, [eax+70]
mov [ebp-8], edx
mov ecx, [eax+14]
sub ecx, [eax+74]
mov [ebp-4], ecx
jmp 403EFF

offset 403F69
; fix switch table a bit
data 2F 3E 40 00

offset 41DBD0
; hook snake weapon
; mapdata +11
JMP [4937DC]

offset 4138A0
; hook load PXA
; mapdata +12
JMP [4937E0]

offset 4139A0
; hook getPXA
; mapdata +13
JMP [4937E4]

offset 419D10
; hook draw weapon/ammo
; mapdata +14
JMP [4937E8]

offset 41A1D0
; hook draw HP
; mapdata +15
JMP [4937EC]

offset 4225ED
; hook the TSC parser
; this destroys the END command which is handled in the new parser
; mapdata +16
CALL [4937F0]
cmp eax, -1
jle 422666
mov [ebp-2C], eax
jmp 4252A7

offset 414250
; hook rendering map name
; mapdata 17
JMP [4937F4]

offset 4168C0
; hook player act negotiator
; mapdata +18
JMP [4937F8]

offset 4196F0
; hook add exp function
; mapdata +19
JMP [4937FC]

offset 425BF0
; hook "createNumberObject" func
; mapdata +20
JMP [493800]

offset 418B10
; hook "checkHitNpcNonSolid" function
; mapdata +21
JMP [493804]
