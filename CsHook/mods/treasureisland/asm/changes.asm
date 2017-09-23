
offset 410815
; reordered calls to rendering HUD
push ebx
mov ebx, [49E1E8]
and ebx, 2
push ebx
call 41A1D0 ; draw HP / ui background
add esp, 4
push 0
call 414250 ; draw map name
add esp, 4
push ebx
call 419D10 ; draw weapon stuff
add esp, 4
push 68
push 78
call 41A350 ; draw air
call 40E770 ; put fade
pop ebx
jmp 41085B


offset 429BF6
; change treasure chest (closed) sprite rects
mov [ebp-30], 80
mov [ebp-2C], 98
mov [ebp-28], 0B0
mov [ebp-24], 0B0

mov [ebp-20], 0B0
mov [ebp-1C], 98
mov [ebp-18], 0E0
mov [ebp-14], 0B0

mov [ebp-10], 0E0
mov [ebp-0C], 98
mov [ebp-08], 110
mov [ebp-04], 0B0

offset 42A974
; change treasure chest (open) sprite
mov [ebp-10], 70
mov [ebp-0C], 0E0
mov [ebp-08], 0A0
mov [ebp-04], 0F0


offset 416408
; add a vertical offset to the camera position
mov ecx, 49E654
mov eax, [ecx]
add eax, [ecx+10]
mov [ecx+8], eax
mov eax, [ecx+4]
add eax, [ecx+14]
sub eax, 7000 ; camera focus offset
mov [ecx+0C], eax
mov eax, 0 ; padding
nop
nop

offset 41C2B9
; allow organya to fade in as well as out

#define
orgvol=4937A4
orgfade=4A4E10
#enddefine

mov ecx, orgvol
cmp [orgfade], 0
jnz :isFadeOut
cmp [ecx], 5F
jge :end
add [ecx], 5
jmp :end
:isFadeOut
cmp [ecx], 1
jle :end
sub [ecx], 2

; padding
and ecx, 12345678
and ecx, 12345678
fnop
fnop
fnop
:end

offset 415EE6
; make BoosterV0.8 give bigger boosts, spawn a better FX
mov dword [49E670], -480
; create fx
push 17F
push 0
push 0
push 200
push 0
mov eax, [49E688]
cdq
sub eax, edx
sar eax, 1
add eax, [49E658]
push eax
mov edx, [49E654]
push edx
push 29
call 46EFD0
add esp, 10
push 1
push 1b
call 420640
add esp, 8
jmp 415F33