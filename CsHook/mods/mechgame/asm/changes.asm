
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
add esp, 8
call 40E770 ; put fade
pop ebx
jmp 41085B

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


offset 48B900
; Load cshook.dll and call HookMod()

push ebp
mov ebp, esp
mov eax, :STR_LIBNAME
push eax
call [48C0AC]
mov ecx, :STR_FUNCNAME
push ecx
push eax
call [48C134]
call eax
pop ebp
retn

:STR_LIBNAME
data 63 73 68 6f 6f 6b 2e 64 6c 6c 00 ; cshook.dll
:STR_FUNCNAME
data 48 6f 6f 6b 4d 6f 64 00 ; HookMod

offset 412445
; inject call to LoadLibrary
cmp eax, 0
je :gotMutex
push eax 
call [48C0D8]
add esp, 4
xor eax, eax
jmp 412B83
:gotMutex
call 48B900
jmp 412467