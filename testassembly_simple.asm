;section .data

A dd 1.0,1.0
B dd 1.0,0.0

;section .bss

section .text

	global main

main:

    xorps   xmm0,xmm0 ; somma=0
    xor     edi,edi ;i=0
    jmp     CMPI

FORI:
    movss   xmm1, [A+edi*4];xmm1=x[edx]=x + edx
    
DEBUG1:
    
    subss   xmm1, [B+edi*4];xmm1=x[edx]=x + edx
DEBUG2:
    mulss   xmm1,xmm1;xmm1*=xmm1

    addss   xmm0,xmm1;somma+=xmm1

    inc     edi

CMPI:
    cmp     edi,   2; i<d?
    jl      FORI ;  i<d
    ;i==d
    ; somma è già xmm0, e xmm0 è il valore che contiene il return value
    ; mov eax, [esp+8] ;vedi riga sotto
    ; cvtsi2ss xmm0,eax ; decommentare per far vedere che il valore di ritorno esiste
FINE:
    movd [esp-4], xmm0
    fld dword [esp-4]
    ; pop edi
    ; pop edx
    ; pop eax
    ; pop ebp
