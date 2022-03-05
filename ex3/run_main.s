.section .rodata

.INT_FORMAT:
	.string	"%d"
.STR_FORMAT:
	.string	"%s"


.section .text

.globl run_main
    .type run_main, @function
    # int run_main ()
run_main:
    pushq	%rbp
    pushq   %r12    # save the callee-saved registers
    pushq   %r13
    movq	%rsp, %rbp

    subq    $528, %rsp  # 256 each for one struct, and 16 for user input

    # ----- initialization of the first pstring -----
    leaq	-528(%rbp), %rsi        # %rsi = address on the stack where user input integers will be stored
    movq    $.INT_FORMAT, %rdi      # %rdi = the address of the format string
    xorq	%rax, %rax
    call    scanf

    movzbq  -528(%rbp), %r12        # move the int the user entered to %r12
    movb    %r12b, -256(%rbp)       # move the int the user entered to the struct p1

    leaq	-255(%rbp), %rsi        # %rsi = address on the stack where the first string will be stored (in p1)
    movq    $.STR_FORMAT, %rdi      # %rdi = the address of the format string
    xorq	%rax, %rax
    call    scanf

    # ----- initialization of the second pstring -----
    leaq	-528(%rbp), %rsi        # %rsi = address on the stack where user input integers will be stored
    movq    $.INT_FORMAT, %rdi      # %rdi = the address of the format string
    xorq	%rax, %rax
    call    scanf

    movzbq  -528(%rbp), %r12        # move the int the user entered to %r12
    movb    %r12b, -512(%rbp)       # move the int the user entered to the struct p1

    leaq	-511(%rbp), %rsi        # %rsi = address on the stack where the first string will be stored (in p1)
    movq    $.STR_FORMAT, %rdi      # %rdi = the address of the format string
    xorq	%rax, %rax
    call    scanf

    # ----- selecting which function to run -----
    leaq	-528(%rbp), %rsi        # %rsi = address on the stack where user input integers will be stored
    movq    $.INT_FORMAT, %rdi      # %rdi = the address of the format string
    xorq	%rax, %rax
    call    scanf

    # ----- calling run_func() -----
    movq    -528(%rbp), %rdi        # %rdi = the option the user chose
    leaq	-256(%rbp), %rsi        # %rsi = address of struct p1
    leaq	-512(%rbp), %rdx        # %rdx = address of struct p2
    call    run_func                # call run_func()

    # finish run_main() execution
    .finish_run_main:
        addq    $528, %rsp   # add 528 bytes back to the %rsp

        movq    %rbp, %rsp
        popq    %r13    # retrieve back the callee-saved registers
        popq    %r12
        popq    %rbp
        ret
