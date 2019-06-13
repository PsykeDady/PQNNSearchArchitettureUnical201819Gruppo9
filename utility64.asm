;section .data

;section .bss

section .text


; R10 = i       ymm8 = somma    xmm10 = somma residua       R11 = d - p + 1     r12 = xi + i oppure yi + i   R13 = [x] oppure [y]   ymm9 = differenza

; xmm11 = differenza residua





%macro dist64_2_step 0
    ;inserimento x in ymm9 (packed)
    mov      r12, rdx ; r12=xi
    add      r12, r10 ; r12=xi+i
    mov      r13, rsi ; r13= [x]
    vmovups   ymm9, [r13+r12*4];ymm9=x[r12]=x + r12

;sottrazione di y in ymm9 (packed)
    mov     r12, r8 ; r12=yi
    add     r12, r10 ; r12=yi+i
    mov     r13, rcx ; r13= [y]
    vsubps  ymm9, [r13+r12*4];ymm9=x[r12]=x + r12

;elevamento al quadrato di ymm9, aggiunta in ymm8 e incremento di i + 4
    vmulps   ymm9,ymm9;ymm9*=ymm9
    vaddps   ymm8,ymm9;somma+=ymm9
    add      r10, 8
%endmacro

	global dist64_2_asm

dist64_2_asm:
	; float dist_2(int d, float *x, int xi, float *y, int yi){
    ;     float somma=0, differenza=0;
    ;     for(int i=0;i<d;i++){
    ;       differenza=x[xi+i]-y[yi+i];
    ;       differenza*=differenza;
    ;       somma+=differenza;
    ;    }
    ;    return somma;
    ; }

    push    rbp ; RDI = d, RSI = x*, RDX = xi, RCX = y*, R8 = yi
    mov     rbp, rsp
    pushaq

    xor     r10, r10 ; i = 0
    vxorps  ymm8, ymm8 ; somma = 0
    vxorps  ymm10, ymm10 ; somma residua = 0
    mov     r11, rdi; r11 = d  
    sub     r11, 8*3 ; r11 = d - p*r
    inc     r11     ; r11 = d - p*r + 1

LOOPU:
    
    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge ENDU

    dist64_2_step
    
    dist64_2_step

    dist64_2_step


    
    jmp     LOOPU

ENDU:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*1 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOPQ:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge LOOPR
 
    
    dist64_2_step

    
    jmp     LOOPQ

LOOPR:
    cmp r10, rdi ; i<d?
    jge END

; inserimento x in ymm9 (scalar)
    mov      r12, rdx ; r12=xi
    add      r12, r10 ; r12=xi+i
    mov      r13, rsi ; r13= [x]
    vmovss   xmm11, [r13+r12*4];xmm11=x[r12]=x + r12  

;sottrazione di y in ymm9 (scalar)
    mov     r12, r8 ; r12=yi
    add     r12, r10 ; r12=yi+i
    mov     r13, rcx ; r13= [y]
    vsubss  xmm11, [r13+r12*4];xmm11=x[r12]=x + r12 

;elevamento al quadrato di ymm9, aggiunta in ymm8 e incremento di i + 1
    vmulss   xmm11,xmm11;xmm11*=xmm11
    vaddss   xmm10,xmm11;somma+=xmm11    
    inc r10
    jmp LOOPR

END:

    vhaddps ymm8, ymm8   ; per spostare la somma totale nella prima cella di ymm8
    vhaddps ymm8, ymm8   ; idem sopra
    vperm2f128 ymm9, ymm8, ymm8, 1
    vaddss xmm8, xmm9
    vaddss xmm8, xmm10
    vmovss xmm0, xmm8

    ;i==d
    ; somma è già ymm8, e ymm8 è il valore che contiene il return value
    ; mov eax, [rbp+8] ;vedi riga sotto
    ; cvtsi2ss xmm0,eax ; decommentare per far vedere che il valore di ritorno esiste
    ;vmovd [rbp-4], xmm0
    ;fld dword [rbp-4]
    popaq
    pop rbp
    
    ret