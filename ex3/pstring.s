.section .rodata
    .align 8

.INVALID_INPUT_FORMAT_STR:
	.string	"invalid input!\n"


.section .text

.globl pstrlen
    .type pstrlen, @function
    # char pstrlen(Pstring* pstr)
pstrlen:
    pushq	%rbp
    movq	%rsp, %rbp

    # %rdi = Pstring* pstr

    xorq    %rax, %rax
    movb    (%rdi), %al # %al = length of pstr2 (%al is the lower 8 bytes of %rax)

    movq    %rbp, %rsp
    popq    %rbp
    ret


.globl replaceChar
    .type replaceChar, @function
    # Pstring* replaceChar(Pstring* pstr, char oldChar, char newChar)
replaceChar:
    pushq	%rbp
    movq	%rsp, %rbp

    pushq   %r12    # save the callee-saved registers
    pushq   %r13

    # %rdi = Pstring* pstr, %sil = char oldChar, %dl = char newChar

    subq    $2, %rsp

    movb   %sil, -17(%rbp)  # (%sil = char oldChar)
    movb   %dl, -18(%rbp)   # (%dl = char newChar)

    xorq    %r12, %r12    # i=0
    xorq    %r13, %r13

    .RC_CHECK_LOOP_CONDITION:
        call    pstrlen
        cmpb    %al, %r12b      # compare i (%r12b) : %al
        jae     .RC_finish_run  # if i >= pstrlen (%al)

    .RC_LOOP_BODY:
        movb    1(%rdi, %r12, 1), %r13b
        cmpb    %r13b, -17(%rbp)            # compare oldChar : (%rdi)[i]
        jne     .RC_LOOP_BODY_CLOSURE       # if not equal, continue to the next char
        movb    -18(%rbp), %r13b
        movb    %r13b, 1(%rdi, %r12, 1)     # (%rdi)[i] = newChar

    .RC_LOOP_BODY_CLOSURE:
        incb    %r12b   # i++
        jmp     .RC_CHECK_LOOP_CONDITION

    .RC_finish_run:
        movq    %rdi, %rax
        addq    $2, %rsp    # add 2 bytes back to the %rsp (pushb   %sil, pushb   %dl)

        popq    %r13        # retrieve back the callee-saved registers
        popq    %r12

        movq    %rbp, %rsp
        popq    %rbp
        ret



.globl swapCase
    .type swapCase, @function
    # Pstring* swapCase(Pstring* pstr)
swapCase:
    pushq	%rbp
    movq	%rsp, %rbp

    pushq   %r12    # save the callee-saved registers
    pushq   %r13

    # %rdi = Pstring* pstr

    xorq    %r12, %r12    # i=0
    xorq    %r13, %r13

    .SC_CHECK_LOOP_CONDITION:
        call    pstrlen
        cmpb    %al, %r12b # compare i (%r12b) : %al
        jae     .SC_finish_run     # if i >= pstrlen (%al)

    .SC_LOOP_BODY:
        movb    1(%rdi, %r12, 1), %r13b
        cmpb    $0x41, %r13b            # compare (%rdi)[i] : 'A'
        jb     .SC_LOOP_BODY_CLOSURE    # if (%rdi)[i] < 'A'
        cmpb    $0x5A, %r13b        # compare (%rdi)[i] : 'Z'
        jbe     .SC_UPPER_CASE      # if (%rdi)[i] <= 'Z'

        cmpb    $0x7a, %r13b            # compare (%rdi)[i] : 'z'
        ja     .SC_LOOP_BODY_CLOSURE    # if (%rdi)[i] > 'z'
        cmpb    $0x61, %r13b        # compare (%rdi)[i] : 'a'
        jae     .SC_LOWER_CASE      # if (%rdi)[i] >= 'a'

        jmp .SC_LOOP_BODY_CLOSURE

    .SC_UPPER_CASE:
        addb    $32, %r13b
        movb    %r13b, 1(%rdi, %r12, 1)    # (%rdi)[i] = (%rdi)[i] + 32
        jmp .SC_LOOP_BODY_CLOSURE
    
    .SC_LOWER_CASE:
        subb    $32, %r13b
        movb    %r13b, 1(%rdi, %r12, 1)    # (%rdi)[i] = (%rdi)[i] - 32
        jmp .SC_LOOP_BODY_CLOSURE

    .SC_LOOP_BODY_CLOSURE:
        incb    %r12b   # i++
        jmp     .SC_CHECK_LOOP_CONDITION

    .SC_finish_run:
        movq    %rdi, %rax

        popq    %r13    # retrieve back the callee-saved registers
        popq    %r12

        movq    %rbp, %rsp
        popq    %rbp
        ret


.globl pstrijcpy
    .type pstrijcpy, @function
    # Pstring* pstrijcpy(Pstring* dst, Pstring* src, char i, char j)
pstrijcpy:
    pushq	%rbp
    pushq   %r12    # save the callee-saved registers
    pushq   %r13
    movq	%rsp, %rbp

    # %rdi = Pstring* dst, %rsi = Pstring* src, %dl = char i, %cl = char j

    movzbq  %dl, %rdx   # make sure the rest of %rdx is zeroes

    movq    %rdi, %r12

    .PCPY_check_indexes:
        call    pstrlen
        cmpb    %al, %cl    # compare j : pstrlen(dst) (%al)
        jae     .PCPY_invalid_indexes    # if j >= pstrlen(dst)

        movq    %rsi, %rdi  # move Pstring* src to %rdi
        call    pstrlen
        cmpb    %al, %cl    # compare j : pstrlen(src) (%al)
        jae     .PCPY_invalid_indexes    # if j >= pstrlen(src)

    .PCPY_CHECK_LOOP_CONDITION:
        cmpb    %cl, %dl    # compare i (%dl) : j (%cl)
        ja     .PCPY_finish_run     # if i >= j

    .PCPY_LOOP_BODY:
        movb    1(%rsi, %rdx, 1), %r8b  # %r8b = src[i]
        movb    %r8b, 1(%r12, %rdx, 1)  # dst[i] = %r8b

    .PCPY_LOOP_BODY_CLOSURE:
        incb    %dl   # i++
        jmp     .PCPY_CHECK_LOOP_CONDITION

    .PCPY_invalid_indexes:
        movq    $.INVALID_INPUT_FORMAT_STR, %rdi
        call printf     # print "invalid input"

    .PCPY_finish_run:
        movq    %r12, %rax  # return Pstring* dst

        movq    %rbp, %rsp
        popq    %r13    # retrieve back the callee-saved registers
        popq    %r12
        popq    %rbp
        ret


.globl pstrijcmp
    .type pstrijcmp, @function
    # int pstrijcmp(Pstring* pstr1, Pstring* psrt2, char i, char j)
pstrijcmp:
    pushq	%rbp
    pushq   %r12    # save the callee-saved registers
    pushq   %r13
    movq	%rsp, %rbp

    # %rdi = Pstring* pstr1, %rsi = Pstring* pstr2, %dl = char i, %cl = char j

    movzbq  %dl, %rdx   # make sure the rest of %rdx is zeroes

    movq    %rdi, %r12
    xorq    %r13, %r13

    .PCMP_check_indexes:
        call    pstrlen
        cmpb    %al, %cl    # compare j : pstrlen(dst) (%al)
        jae     .PCMP_invalid_indexes    # if j >= pstrlen(dst)

        movq    %rsi, %rdi  # move Pstring* src to %rdi
        call    pstrlen
        cmpb    %al, %cl    # compare j : pstrlen(src) (%al)
        jae     .PCMP_invalid_indexes    # if j >= pstrlen(src)

    .PCMP_CHECK_LOOP_CONDITION:
        cmpb    %cl, %dl    # compare i (%dl) : j (%cl)
        ja     .PCMP_finish_run     # if i >= j

    .PCMP_LOOP_BODY:
        movb    1(%r12, %rdx, 1), %r8b  # %r8b = pstr1[i]
        movb    1(%rsi, %rdx, 1), %r9b  # %r9b = pstr2[i]
        cmpb    %r9b, %r8b  # compare pstr1[i] : pstr2[i]
        jb  .PCMP_2BIGGER   # if pstr1[i] < pstr2[i], meaning pstr2 is bigger
        ja  .PCMP_1BIGGER   # if pstr1[i] > pstr2[i], meaning pstr1 is bigger

    .PCMP_LOOP_BODY_CLOSURE:
        incb    %dl   # i++
        jmp     .PCMP_CHECK_LOOP_CONDITION
    
    .PCMP_2BIGGER:  # if pstr2 is bigger
        movq    $-1, %r13
        jmp .PCMP_finish_run
    
    .PCMP_1BIGGER:  # if pstr1 is bigger
        movq    $1, %r13
        jmp .PCMP_finish_run

    .PCMP_invalid_indexes:
        movq    $.INVALID_INPUT_FORMAT_STR, %rdi
        call printf     # print "invalid input"
        movq    $-2, %r13   # value to return = -2

    .PCMP_finish_run:
        movq    %r13, %rax  # return %r13

        movq    %rbp, %rsp
        popq    %r13    # retrieve back the callee-saved registers
        popq    %r12
        popq    %rbp
        ret
