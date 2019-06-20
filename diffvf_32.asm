; section .data

;section  .bss

section   .text

%macro diffvf32_step 0
; inserimento di x in xmm1 (packed)

        mov     edx, [ebp+16]       ;edx=xi
        add     edx, edi            ;edx=xi+i
        mov     eax, [ebp+12]       ;eax=[x]
        movups  xmm1, [eax+edx*4]   

; sottrazione di y in xmm1 (packed)

        mov     edx, [ebp+24]       ;edx=yi
        add     edx, edi            ;edx=yi+i
        mov     eax, [ebp+20]       ;eax=[y]
        subps   xmm1, [eax+edx*4]  

; inserimento di res in xmm2 (packed)

        mov     edx, [ebp+32]       ;edx=ri
        add     edx, edi            ;edx=ri+i
        mov     ebx, [ebp+28]       ;ebx=[res]
        movups  xmm2, [ebx+edx*4]
        movups  xmm2, xmm1

        add     edi,4
%endmacro

        global diffvf32_asm

diffvf32_asm: 

        push    ebp
        mov     ebp, esp
        push    eax
        push    ebx
        push    edx
        push    edi
        push    esi

        xor     edi, edi            ;i=0
        mov     esi, [ebp+8]        ;esi=d
        sub     esi, 4*2            ;esi = d - p*r
        inc     esi                 ;esi = d - p*r + 1

LOOPU:

        cmp     edi, esi
        jge     ENDU

        diffvf32_step

        diffvf32_step

        jmp     LOOPU

ENDU:

        mov     esi, [ebp+8]        ;esi = d  
        sub     esi, 4              ;esi = d - p
        inc     esi                 ;esi = d - p + 1

LOOPQ:

        cmp     edi, esi
        jge     LOOPR

        diffvf32_step

        jmp     LOOPQ

LOOPR:

        cmp edi, esi ; i<d?
        jge END

; inserimento di x in xmm1 (scalar)

        mov     edx, [ebp+16]       ;edx=xi
        add     edx, edi            ;edx=xi+i
        mov     eax, [ebp+12]       ;eax=[x]
        movss  xmm1, [eax+edx*4]

; sottrazione di y in xmm1 (scalar)

        mov     edx, [ebp+24]       ;edx=yi
        add     edx, edi            ;edx=yi+i
        mov     eax, [ebp+20]       ;eax=[y]
        subss   xmm1, [eax+edx*4]

; inserimento di res in xmm2 (scalar)

        mov     edx, [ebp+32]       ;edx=ri
        add     edx, edi            ;edx=ri+i
        mov     ebx, [ebp+28]       ;ebx=[res]
        movss   xmm2, [ebx+edx*4]
        movss   xmm2, xmm1
        inc     edi
        jmp     LOOPR

END:

        movd [ebp-4], xmm0
        fld dword [ebp-4]
        pop esi
        pop edi
        pop edx
        pop ebx
        pop eax
        pop ebp
    
    ret