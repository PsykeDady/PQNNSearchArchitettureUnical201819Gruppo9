;section .data

;section .bss

section .text

%macro dist_2_step 0
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

;elevamento al quadrato di ymm9, aggiunta in ymm8 e incremento di i + 8
    vmulps   ymm9,ymm9;ymm9*=ymm9
    vaddps   ymm8,ymm9;somma+=ymm9
    add      r10, 8
%endmacro

%macro diffvf_step 0
;inserimento x in ymm8 (packed)
    mov      r12, rdx ; r12=xi
    add      r12, r10 ; r12=xi+i
    mov      r13, rsi ; r13= [x]
    vmovups   ymm8, [r13+r12*4];ymm8=x[r12]=x + r12

;sottrazione di y in ymm8 (packed)
    mov     r12, r8 ; r12=yi
    add     r12, r10 ; r12=yi+i
    mov     r13, rcx ; r13= [y]
    vsubps  ymm8, [r13+r12*4];ymm8=x[r12]=x + r12

;inserimento differenza in res (packed)
    mov      r12, rax ; r12=ri
    add      r12, r10 ; r12=xi+i
    mov      r13, r9 ; r13= [res]
    vmovups  [r13+r12*4], ymm8  ;res[ri+1]=x[xi+i]-y[yi+i]
;incremento i = i+8
    add      r10, 8
%endmacro


%macro copyv_step 0
;inserimento src in ymm8 (packed)
    mov      r12, rdx ; r12=srcii
    add      r12, r10 ; r12=srci+i
    mov      r13, rsi ; r13= [x]
    vmovups   ymm8, [r13+r12*4]


;inserimento in dest(packed)
    mov      r12, rax ; r12=desti
    add      r12, r10 ; r12=desti+i
    mov      r13, r9 ; r13= [dest]
    vmovups  [r13+r12*4], ymm8  ;dest[desti+1]=src[srcii+i]

;incremento i = i+8
    add      r10, 8
%endmacro

	
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

; R10 = i       ymm8 = somma    xmm10 = somma residua       R11 = d - p + 1     r12 = xi + i oppure yi + i   R13 = [x] oppure [y]   ymm9 = differenza

; xmm11 = differenza residua


    push    rbp ; RDI = d, RSI = x*, RDX = xi, RCX = y*, R8 = yi
    mov     rbp, rsp
    
    

    xor     r10, r10 ; i = 0
    vxorps  ymm8, ymm8 ; somma = 0
    vxorps  ymm10, ymm10 ; somma residua = 0
    mov     r11, rdi; r11 = d  
    sub     r11, 8*32 ; r11 = d - p*r
    inc     r11     ; r11 = d - p*r + 1


LOOP32DIST:
    
    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge  END32DIST 

    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    
    jmp     LOOP32DIST

END32DIST:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*8 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP8DIST:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge END8DIST
 
    
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step
    dist_2_step

    
    jmp     LOOP8DIST

END8DIST:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*2 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP2DIST:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge LOOPSDIST
 
    
    dist_2_step
    dist_2_step

    jmp     LOOP2DIST

LOOPSDIST:
    cmp r10, rdi ; i<d?
    jge ENDDIST

; inserimento x in ymm9 (scalar)
    mov      r12, rdx ; r12=xi
    add      r12, r10 ; r12=xi+i
    mov      r13, rsi ; r13= [x]
    vmovss   xmm11, [r13+r12*4] ;xmm11=x[r12]=x + r12  

;sottrazione di y in ymm9 (scalar)
    mov     r12, r8 ; r12=yi
    add     r12, r10 ; r12=yi+i
    mov     r13, rcx ; r13= [y]
    vsubss  xmm11, [r13+r12*4];xmm11=x[r12]=x + r12 

;elevamento al quadrato di ymm9, aggiunta in ymm8 e incremento di i + 1
    vmulss   xmm11,xmm11;xmm11*=xmm11
    vaddss   xmm10,xmm11;somma+=xmm11    
    inc r10
    jmp LOOPSDIST

ENDDIST:

    vhaddps ymm8, ymm8   ; per spostare la somma totale nella prima cella di ymm8
    vhaddps ymm8, ymm8   ; idem sopra
    vperm2f128 ymm9, ymm8, ymm8, 1
    vaddss xmm8, xmm9
    vaddss xmm8, xmm10
    vmovss xmm0, xmm8

  
    pop rbp
    
    ret


    global diffvf_asm

diffvf_asm:

;void diffvf(int d, float* x, int xi, float *y, int yi, float *res, int ri){
;    int i;
;    for(i=0; i<d; i++)
;        {res[ri+i]=x[xi+i]-y[yi+i];}
;}

; R10 = i       ymm9 = differenza tra x e y     R11 = d-p*r+1 oppure d-p+1    r12 = xi + i oppure yi + i  oppure ri + i  R13 = [x] oppure [y] oppure [res]

; RAX = ri



    push    rbp ; RDI = d, RSI = x*, RDX = xi, RCX = y*, R8 = yi , R9 = res*
    mov     rbp, rsp
    
   

    xor     r10, r10 ; i = 0
    mov     rax, [rbp+16] ; rax = ri
    mov     r11, rdi; r11 = d  
    sub     r11, 8*32 ; r11 = d - p*r
    inc     r11     ; r11 = d - p*r + 1

LOOP32DIFF:
    
    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge  END32DIFF

    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step

    jmp LOOP32DIFF

END32DIFF:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*8 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP8DIFF:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge END8DIFF

    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step

    jmp LOOP8DIFF

END8DIFF:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*2 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP2DIFF:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge LOOPSDIFF
 
    
    diffvf_step
    diffvf_step

    jmp     LOOP2DIFF

LOOPSDIFF:
    cmp r10, rdi ; i<d?
    jge ENDDIFF

;inserimento x in ymm9 (scalar)
    mov      r12, rdx ; r12=xi
    add      r12, r10 ; r12=xi+i
    mov      r13, rsi ; r13= [x]
    vmovss   xmm8, [r13+r12*4];ymm9=x[r12]=x + r12

;sottrazione di y in ymm9 (scalar)
    mov     r12, r8 ; r12=yi
    add     r12, r10 ; r12=yi+i
    mov     r13, rcx ; r13= [y]
    vsubss  xmm8, [r13+r12*4];ymm9=x[r12]=x + r12

;inserimento x in ymm9 (scalar)
    mov      r12, rax ; r12=ri
    add      r12, r10 ; r12=xi+i
    mov      r13, r9 ; r13= [res]
    vmovss  [r13+r12*4], xmm8  ;res[ri+1]=x[xi+i]-y[yi+i]
;incremento i = i+1
    inc     r10

    jmp LOOPSDIFF

ENDDIFF:

 
    pop rbp
    
    ret


global copyv_asm

copyv_asm:

;void copyv(int d, float* dest, int desti, float *src, int srci){
;   for(int i=0;i<d;i++){
;      dest[desti+i]=src[srci+i];

; R10 = i     R11 = d-p*r+1 oppure d-p+1    r12 = srci + i   R13 = [src]  oppure [dest]

; RAX = desti



    push    rbp ; RDI = d, RSI = dest*, RDX = desti, RCX = src*, R9 = srci
    mov     rbp, rsp
    
    push    rax
    push    rcx
    push    rdx
    push    rdi
    push    rsi

    xor     r10, r10 ; i = 0
    ;???????????mov     rax, [rbp+24] ; rax = srci
    mov     r11, rdi; r11 = d  
    sub     r11, 8*32 ; r11 = d - p*r
    inc     r11     ; r11 = d - p*r + 1

LOOP32COPYV:
    
    cmp r10, r11 ; i<d-p*r+1? 
    jge  END32COPYV

    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step

    jmp LOOP32COPYV

END32COPYV:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*8 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP8COPYV:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge END8COPYV

    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step
    copyv_step

    jmp LOOP8COPYV

END8COPYV:

    mov     r11, rdi ; r11 = d  
    sub     r11, 8*2 ; r11 = d - p * r
    inc     r11     ; r11 = d - p + 1

LOOP2COPYV:

    cmp r10, r11 ; i<d-p*r+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge LOOPSCOPYV
 
    
    copyv_step
    copyv_step

    jmp     LOOP2COPYV

LOOPSCOPYV:
    cmp r10, rdi ; i<d?
    jge ENDCOPYV

;inserimento src in xmm9 (scalar)
    mov      r12, rdx ; r12=srci
    add      r12, r10 ; r12=srci+i
    mov      r13, rsi ; r13= [src]
    vmovss   xmm8, [r13+r12*4]


    mov      r12, rax ; r12=desti
    add      r12, r10 ; r12=desti+i
    mov      r13, r9 ; r13= [dest]
    vmovss  [r13+r12*4], xmm8  ;dest[desti+1]=src[srci+i]
;incremento i = i+1
    inc     r10

    jmp LOOPSCOPYV

ENDCOPYV:

    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rax
    pop rbp
    
    ret