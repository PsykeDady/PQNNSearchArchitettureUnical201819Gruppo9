;section .data

;section .bss

%include "sseutils.nasm"
section .text

	global dist_2_asm

dist_2_asm:
	; float dist_2(int d, float *x, int xi, float *y, int yi){
    ;     float somma=0, differenza=0;
    ;     for(int i=0;i<d;i++){
    ;       differenza=x[xi+i]-y[yi+i];
    ;       differenza*=differenza;
    ;       somma+=differenza;
    ;    }
    ;    return somma;
    ; }

    push    ebp
    mov     ebp, esp
    push    eax
    push    edx
    push    edi
    

    xorps   xmm0,xmm0 ; somma=0
    xor     edi,edi ;i=0
    jmp     CMPI

FORI:
    mov     edx, [ebp+16] ; edx=xi
    add     edx, edi ; edx=xi+i
    mov     eax, [ebp+12] ; eax= [x]
    movss   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx
    
DEBUG1:
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    mov     eax, [ebp+20] ; eax= [y]
    subss   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx
DEBUG2:
    mulss   xmm1,xmm1;xmm1*=xmm1

    addss   xmm0,xmm1;somma+=xmm1

    inc     edi

CMPI:
    cmp     edi,   [ebp+8]; i<d?
    jl      FORI ;  i<d
    ;i==d
    ; somma è già xmm0, e xmm0 è il valore che contiene il return value
    ; mov eax, [ebp+8] ;vedi riga sotto
    ; cvtsi2ss xmm0,eax ; decommentare per far vedere che il valore di ritorno esiste
    movd [ebp-4], xmm0
    fld dword [ebp-4]
    pop edi
    pop edx
    pop eax
    pop ebp
    
    ret



