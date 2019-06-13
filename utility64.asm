;section .data

;section .bss

section .text

%macro dist64_2_step 0
    ;inserimento x in ymm1 (packed)
    mov      rdx, [rbp+16] ; rdx=xi
    add      rdx, rdi ; rdx=xi+i
    mov      rax, [rbp+12] ; rax= [x]
    vmovups   ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;sottrazione di y in ymm1 (packed)
    mov     rdx, [rbp+24] ; rdx=yi
    add     rdx, rdi ; rdx=yi+i
    mov     rax, [rbp+20] ; rax= [y]
    vsubps  ymm1, [rax+edx*4];ymm1=x[rdx]=x + rdx

;elevamento al quadrato di ymm1, aggiunta in ymm0 e incremento di i + 4
    vmulps   ymm1,ymm1;ymm1*=ymm1
    vaddps   ymm0,ymm1;somma+=ymm1
    add      rdi, 8
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

    push    rbp ;[rbp+12]= d, [rbp+20]= xi, [rbp+28]= yi, [rbp+16]= x, [rbp+30]= y
    mov     rbp, rsp
    push    rax
    push    rdx
    push    rdi 
    push    rsi 
    
    xor     rdi, rdi ; i = 0
    vxorps   ymm0, ymm0 ; somma = 0
    vxorps  ymm3, ymm3 ; somma residua = 0
    mov     rsi, [rbp+12]; rsi = d  
    sub     rsi, 8*2 ; rsi = d - p*r
    inc     rsi     ; rsi = d - p*r + 1

LOOPU:
    
    cmp rdi, rsi ; i<d-p+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge ENDU

     ;inserimento x in ymm1 (packed)
    mov      rdx, [rbp+24] ; rdx=xi
    add      rdx, rdi ; rdx=xi+i
    mov      rax, [rbp+20] ; rax= [x]
    vmovups   ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;sottrazione di y in ymm1 (packed)
    mov     rdx, [rbp+36] ; rdx=yi
    add     rdx, rdi ; rdx=yi+i
    mov     rax, [rbp+32] ; rax= [y]
    vsubps  ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;elevamento al quadrato di ymm1, aggiunta in ymm0 e incremento di i + 4
    vmulps   ymm1,ymm1;ymm1*=ymm1
    vaddps   ymm0,ymm1;somma+=ymm1
    add      rdi, 8
    
     ;inserimento x in ymm1 (packed)
    mov      rdx, [rbp+24] ; rdx=xi
    add      rdx, rdi ; rdx=xi+i
    mov      rax, [rbp+20] ; rax= [x]
    vmovups   ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;sottrazione di y in ymm1 (packed)
    mov     rdx, [rbp+36] ; rdx=yi
    add     rdx, rdi ; rdx=yi+i
    mov     rax, [rbp+32] ; rax= [y]
    vsubps  ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;elevamento al quadrato di ymm1, aggiunta in ymm0 e incremento di i + 4
    vmulps   ymm1,ymm1;ymm1*=ymm1
    vaddps   ymm0,ymm1;somma+=ymm1
    add      rdi, 8
    
    jmp     LOOPU

ENDU:

    mov     rsi, [rbp+12] ; rsi = d  
    sub     rsi, 8 ; rsi = d - p
    inc     rsi     ; rsi = d - p + 1

LOOPQ:

    cmp rdi, rsi ; i<d-p+1? // da provare se è possibile fare direttamente [rbp+8]-4+1
    jge LOOPR
 
     ;inserimento x in ymm1 (packed)
    mov      rdx, [rbp+24] ; rdx=xi
    add      rdx, rdi ; rdx=xi+i
    mov      rax, [rbp+20] ; rax= [x]
    vmovups   ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;sottrazione di y in ymm1 (packed)
    mov     rdx, [rbp+36] ; rdx=yi
    add     rdx, rdi ; rdx=yi+i
    mov     rax, [rbp+32] ; rax= [y]
    vsubps  ymm1, [rax+rdx*4];ymm1=x[rdx]=x + rdx

;elevamento al quadrato di ymm1, aggiunta in ymm0 e incremento di i + 4
    vmulps   ymm1,ymm1;ymm1*=ymm1
    vaddps   ymm0,ymm1;somma+=ymm1
    add      rdi, 8 
    
    jmp     LOOPQ

LOOPR:
    cmp rdi, [rbp+12] ; i<d?
    jge END

; inserimento x in ymm1 (scalar)
    mov      rdx, [rbp+24] ; rdx=xi
    add      rdx, rdi ; rdx=xi+i
    mov      rax, [rbp+20] ; rax= [x]
    vmovss   xmm2, [rax+rdx*4];xmm2=x[rdx]=x + rdx  

;sottrazione di y in ymm1 (scalar)
    mov     rdx, [rbp+36] ; rdx=yi
    add     rdx, rdi ; rdx=yi+i
    mov     rax, [rbp+32] ; rax= [y]
    vsubss  xmm2, [rax+rdx*4];xmm2=x[rdx]=x + rdx 

;elevamento al quadrato di ymm1, aggiunta in ymm0 e incremento di i + 1
    vmulss   xmm2,xmm2;xmm2*=xmm2
    vaddss   xmm3,xmm2;somma+=xmm2    
    inc rdi
    jmp LOOPR

END:

    vhaddps ymm0, ymm0   ; per spostare la somma totale nella prima cella di ymm0
    vhaddps ymm0, ymm0   ; idem sopra
    vperm2f128 ymm1, ymm0, ymm0, 1
    vaddss xmm0, xmm1
    vaddss xmm0, xmm3

    ;i==d
    ; somma è già ymm0, e ymm0 è il valore che contiene il return value
    ; mov eax, [rbp+8] ;vedi riga sotto
    ; cvtsi2ss xmm0,eax ; decommentare per far vedere che il valore di ritorno esiste
    ;vmovd [rbp-4], xmm0
    ;fld dword [rbp-4]
    pop rsi
    pop rdi
    pop rdx
    pop rax
    pop rbp
    
    ret