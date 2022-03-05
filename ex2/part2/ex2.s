.section .text

.globl even
    .type even, @function
    # int even (int num, int i)
even:
    pushq	%rbp
    movq	%rsp, %rbp

    # %edi = num, %sil = i
    # return int on %eax

    movl %edi, %eax # move num from %edi to %eax
    mov %sil, %cl   # move i to %cl
    sall %cl, %eax  # num << i

    movq    %rbp, %rsp
    popq    %rbp
    ret


.globl go
    .type go, @function
    # int go (int A[10]);
go:
    pushq	%rbp
    movq	%rsp, %rbp

    # %rdi = pointer to beginning of A
    # return int on %eax
    
    # take 8 bytes from stack
    subq $8, %rsp
    movq %rdi, (%rsp)

    movl $0x0, %r12d # (%r12d) sum = 0
    movl $0x0, %r13d # i = 0

.WHILE_LOOP:
    movq (%rsp), %rdi
    movl (%rdi, %r13, 4), %r14d # %r14d = A[i]
    test $1, %r14d # 1 & A[i]
    jz .IF # if (A[i] % 2 == 0)
    jnz .ELSE # else
    .IF: # if (A[i] % 2 == 0)
        movl %r14d, %edi
        movl %r13d, %esi
        call even
        addl %eax, %r12d # add the return value from even() to sum
        jmp .END_OF_LOOP
    .ELSE:
        addl %r14d, %r12d # sum += A[i]
        jmp .END_OF_LOOP 

.END_OF_LOOP:
    incl %r13d # i++    
    cmpl $10, %r13d # compare i to 10
    js .WHILE_LOOP # if i<10, jump to beginning of the WHILE_LOOP

    movl %r12d, %eax # %eax = sum (%r12d)

    # add back the 8 bytes
    addq $8, %rsp
    nop
    movq    %rbp, %rsp
    popq    %rbp
    ret
