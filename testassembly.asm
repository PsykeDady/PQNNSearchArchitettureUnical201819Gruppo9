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

    push    ebp ;[ebp+8]= d, [ebp+16]= xi, [ebp+24]= yi, [ebp+12]= x, [ebp+24]= y
    mov     ebp, esp
    push    eax
    push    edx
    push    edi 
    push    esi 
    
    xor     edi, edi ; i = 0
    xorps   xmm0, xmm0 ; somma = 0
    mov     esi, [ebp+8] ; esi = d  
    sub     esi, 4 ; esi = d - p
    inc     esi     ; esi = d - p + 1

LOOPQ:
    
    cmp edi, esi ; i<d-p+1? // da provare se è possibile fare direttamente [ebp+8]-4+1
    jge ENDQ

;inserimento x in xmm1 (packed)
    mov      edx, [ebp+16] ; edx=xi
    add      edx, edi ; edx=xi+i
    mov      eax, [ebp+12] ; eax= [x]
    movups   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx

;sottrazione di y in xmm1 (packed)
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    mov     eax, [ebp+20] ; eax= [y]
    subps  xmm1, [eax+edx*4];xmm1=x[edx]=x + edx

;elevamento al quadrato di xmm1, aggiunta in xmm0 e incremento di i
    mulps   xmm1,xmm1;xmm1*=xmm1
    addps   xmm0,xmm1;somma+=xmm1
    add     edi, 4
    jmp     LOOPQ

ENDQ:
    cmp edi, [ebp+8] ; i == d?
    je  END

LOOPR:
    cmp edi, [ebp+8] ; i<d?
    jge END

; inserimento x in xmm1 (scalar)
    mov      edx, [ebp+16] ; edx=xi
    add      edx, edi ; edx=xi+i
    mov      eax, [ebp+12] ; eax= [x]
    movss   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx  

;sottrazione di y in xmm1 (scalar)
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    mov     eax, [ebp+20] ; eax= [y]
    subss  xmm1, [eax+edx*4];xmm1=x[edx]=x + edx 
    inc edi

END:
    ;i==d
    ; somma è già xmm0, e xmm0 è il valore che contiene il return value
    ; mov eax, [ebp+8] ;vedi riga sotto
    ; cvtsi2ss xmm0,eax ; decommentare per far vedere che il valore di ritorno esiste
    movd [ebp-4], xmm0
    fld dword [ebp-4]
    pop esi
    pop edi
    pop edx
    pop eax
    pop ebp
    
    ret