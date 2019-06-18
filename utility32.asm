;section .data

;section .bss

section .text

%macro dist_2_step 0
    ;inserimento x in xmm1 (packed)
    mov      edx, [ebp+16] ; edx=xi
    add      edx, edi ; edx=xi+i
    ;mov      eax, [ebp+12] ; eax= [x]
    movups   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx

;sottrazione di y in xmm1 (packed)
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    ;mov     eax, [ebp+20] ; eax= [y]
    subps   xmm1, [ecx+edx*4];xmm1=x[edx]=x + edx

;elevamento al quadrato di xmm1, aggiunta in xmm0 e incremento di i + 4
    mulps   xmm1,xmm1;xmm1*=xmm1
    addps   xmm0,xmm1;somma+=xmm1
    add     edi, 4
%endmacro

%macro diffvf_step 0
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

;inserimento della differenza in res (packed)
    mov     edx, [ebp+32] ; edx=ri
    add     edx, edi ; edx=ri+i
    mov     eax, [ebp+28] ; eax= [res]
    movups  [eax+edx*4], xmm1 ;res[ri+i] = xmm0

;incremento i= i + 4
    add edi, 4
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

    push    ebp ;[ebp+8]= d, [ebp+16]= xi, [ebp+24]= yi, [ebp+12]= x, [ebp+20]= y
    mov     ebp, esp
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    edi 
    push    esi 
    
    xor     edi, edi ; i = 0
    xorps   xmm0, xmm0 ; somma = 0
    mov     eax, [ebp+12] ; eax = [x]
    mov     ebx, [ebp+8] ;ebx = d 
    mov     ecx, [ebp+20] ; ecx = [y]
    mov     esi, ebx ; esi = d  
    sub     esi, 4*32 ; esi = d - p*r
    inc     esi     ; esi = d - p*r + 1

LOOP32DIST:
    
    cmp edi, esi ; i<d-p*r+1? // da provare se è possibile fare direttamente [ebp+8]-4+1
    jge END32DIST

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

    mov     esi, ebx ; esi = d  
    sub     esi, 4*8 ; esi = d - p * r
    inc     esi     ; esi = d - p *r + 1

LOOP8DIST:

    cmp edi, esi ; i<d-p*r+1? // da provare se è possibile fare direttamente [ebp+8]-4+1
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

    mov     esi, ebx ; esi = d  
    sub     esi, 4*2 ; esi = d - p * r
    inc     esi     ; esi = d - p *r + 1

LOOP2DIST:

    cmp edi, esi ; i<d-p*r+1? // da provare se è possibile fare direttamente [ebp+8]-4+1
    jge LOOPSDIST
 
    dist_2_step
    dist_2_step 
    
    jmp     LOOP2DIST

LOOPSDIST:
    cmp edi, ebx ; i<d?
    jge ENDDIST

; inserimento x in xmm1 (scalar)
    mov      edx, [ebp+16] ; edx=xi
    add      edx, edi ; edx=xi+i
    ;mov      eax, [ebp+12] ; eax= [x]
    movss   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx  

;sottrazione di y in xmm1 (scalar)
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    ;mov     eax, [ebp+20] ; eax= [y]
    subss  xmm1, [ecx+edx*4];xmm1=x[edx]=x + edx 

;elevamento al quadrato di xmm1, aggiunta in xmm0 e incremento di i + 1
    mulss   xmm1,xmm1;xmm1*=xmm1
    addss   xmm0,xmm1;somma+=xmm1    
    inc edi
    jmp LOOPSDIST

ENDDIST:
    haddps xmm0, xmm0   ; per spostare la somma totale nella prima cella di xmm0
    haddps xmm0, xmm0   ; idem sopra

    movd [ebp-4], xmm0
    fld dword [ebp-4]
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    
    ret





    global diffvf_asm

diffvf_asm:

;void diffvf(int d, float* x, int xi, float *y, int yi, float *res, int ri){
;    int i;
;    for(i=0; i<d; i++)
;        {res[ri+i]=x[xi+i]-y[yi+i];}
;}



    push    ebp ;[ebp+8]= d, [ebp+16]= xi, [ebp+24]= yi, [ebp+12]= x, [ebp+20]= y   [ebp+28]=res    [ebp+32]=ri
    mov     ebp, esp
    push    eax
    push    ebx
    push    edx
    push    edi 
    push    esi

    xor     edi, edi        ;i = 0
    mov     ebx, [ebp+8]    ; ebx = d
    mov     esi, ebx    ; esi = d
    sub     esi, 4*32      ; esi = d - p * r
    inc     esi         ; esi = d - p * r + 1


LOOP32DIFF:
    cmp edi, esi ; i<d - p * r + 1 ?
    jge LOOP8DIFF

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

LOOP8DIFF:
    mov     esi, ebx ; esi = d  
    sub     esi, 4*8 ; esi = d - p *r
    inc     esi     ; esi = d - p * r + 1

LOOPP8DIFF:
    cmp edi, esi ; i<d-p*r+1?
    jge LOOP2DIFF

    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step
    diffvf_step

    jmp LOOPP8DIFF

LOOP2DIFF:
    mov     esi, ebx ; esi = d  
    sub     esi, 4*2 ; esi = d - p *r
    inc     esi     ; esi = d - p * r + 1

LOOPP2DIFF:
    cmp edi, esi ; i<d-p*r+1?
    jge LOOPSDIFF

    diffvf_step
    diffvf_step

    jmp LOOPP2DIFF

LOOPSDIFF:
    cmp edi, ebx
    jge ENDDIFF

;inserimento x in xmm1 (scalar)
    mov      edx, [ebp+16] ; edx=xi
    add      edx, edi ; edx=xi+i
    mov      eax, [ebp+12] ; eax= [x]
    movss   xmm1, [eax+edx*4];xmm1=x[edx]=x + edx

;sottrazione di y in xmm1 (scalar)
    mov     edx, [ebp+24] ; edx=yi
    add     edx, edi ; edx=yi+i
    mov     eax, [ebp+20] ; eax= [y]
    subss  xmm1, [eax+edx*4];xmm1=x[edx]=x + edx

;inserimento della differenza in res (scalar)
    mov     edx, [ebp+32] ; edx=ri
    add     edx, edi ; edx=ri+i
    mov     eax, [ebp+28] ; eax= [res]
    movss  [eax+edx*4], xmm1 ;res[ri+i] = xmm0

; incremento di i = i + 1
    inc edi
    jmp LOOPSDIFF

ENDDIFF:
    pop esi
    pop edi
    pop edx
    pop ebx
    pop eax
    pop ebp
    
    ret