.section .rodata
    .align 8

# the format string for printf in case 50 (and 60)
.FORMAT_STRING_50:
	.string	"first pstring length: %d, second pstring length: %d\n"

# the format string for printf in case 52
.FORMAT_STRING_52:
    .string "old char: %c, new char: %c, first string: %s, second string: %s\n"

# the format string for printf in case 53 and 54
.FORMAT_STRING_53:
.FORMAT_STRING_54:
    .string "length: %d, string: %s\n"

# the format string for printf in case 55
.FORMAT_STRING_55:
    .string "compare result: %d\n"

# the format string for printf in the default case
.FORMAT_STRING_DEFAULT:
    .string "invalid option!\n"

.SCANF_2_CHARS: # a scanf format for a char
        .string " %c %c"

.SCANF_INT: # a scanf format for an int
        .string "%d"

.JUMP_TABLE:
    .quad   .OP50   # the user enters 50
    .quad   .OP51   # the user enters 51
    .quad   .OP52   # the user enters 52
    .quad   .OP53   # the user enters 53
    .quad   .OP54   # the user enters 54
    .quad   .OP55   # the user enters 55
    .quad   .OP56   # the user enters 56
    .quad   .OP57   # the user enters 57
    .quad   .OP58   # the user enters 58
    .quad   .OP59   # the user enters 59
    .quad   .OP60   # the user enters 60


.section .text

.globl run_func
    .type run_func, @function
    # void run_func (int option_chosen, Pstring* pstr1, Pstring* pstr2)
run_func:
    pushq	%rbp
    pushq   %r12    # save the callee-saved registers
    pushq   %r13
    movq	%rsp, %rbp

    # %edi = num, %rsi = Pstring* pstr1, %rdx = Pstring* pstr2

    # here, we push to the stack 16 bytes. therefore when we want to return from the func, we need to add those back.
    pushq   %rsi    # push the pointer to Pstring to the stack
    pushq   %rdx    # push the pointer to Pstring to the stack

    # execute the switch statement:
    subl    $50, %edi
    cmpl    $10, %edi   # compare %edi : 10
    ja  .DEFAULT_CASE   # if %edi > 10, go to default case.
    jmp *.JUMP_TABLE(,%edi,8) # go to JUMP_TABLE[%edi]


# finish run_func() execution
.finish_run:

    addq    $16, %rsp   # add 16 bytes back to the %rsp

    movq    %rbp, %rsp
    popq    %r13    # retrieve back the callee-saved registers
    popq    %r12
    popq    %rbp
    ret

# the default case in the switch-case
.DEFAULT_CASE:
    movq    $.FORMAT_STRING_DEFAULT, %rdi    # %rdi = the address of the format string
    xorq	%rax, %rax
    call	printf  # call printf
    jmp .finish_run

# case 50 and 60 run the same code
.OP50:
.OP60:
    # print out the length of the two pstrings

    popq    %rdi    # %rdi = address of pstr2
    call    pstrlen # call pstrlen() and pass it the address of pstr2
    movq    %rax, %rdx  # %rdx = length returned from pstrlen()

    popq    %rdi    # %rdi = address of pstr1
    call    pstrlen # call pstrlen() and pass it the address of pstr1
    movq    %rax, %rsi  # %rsi = length returned from pstrlen()
    
    movq    $.FORMAT_STRING_50, %rdi    # %rdi = the address of the format string
    xorq	%rax, %rax
    call	printf  # call printf
    jmp .finish_run

# case 52
.OP52:
    subq    $16, %rsp    # take 16 bytes from the stack

    leaq	-17(%rbp), %rsi         # %rsi = address of the old char in the stack
    leaq	-18(%rbp), %rdx         # %rdx = address of the new char in the stack
    movq    $.SCANF_2_CHARS, %rdi   # %rdi = the address of the format string
    xorq	%rax, %rax
    call	scanf                   # call scanf

    movq    -8(%rbp), %rdi      # %rdi = address of pstr1
    movb    -17(%rbp), %sil     # %sil = char oldChar
    movb    -18(%rbp), %dl      # %dl = char newChar
    call    replaceChar
    movq    %rax, %r12

    movq    -16(%rbp), %rdi     # %rdi = address of pstr2
    movb    -17(%rbp), %sil     # %sil = char oldChar
    movb    -18(%rbp), %dl      # %dl = char newChar
    call    replaceChar
    movq    %rax, %r8

    movq    %r12, %rcx          # %rcx = address of pstr1
    movb    -17(%rbp), %sil     # %sil = char oldChar
    movb    -18(%rbp), %dl      # %dl = char newChar

    incq    %rcx            # (%rcx)++, now points to the begining of string1
    incq    %r8             # (%r8)++, now points to the begining of string2
    
    movq    $.FORMAT_STRING_52, %rdi    # %rdi = the address of the format string
    xorq	%rax, %rax
    call	printf      # call printf

    addq    $16, %rsp   # add the 16 bytes back
    jmp .finish_run

# case 53
.OP53:
    subq    $16, %rsp    # take 16 bytes from the stack

    # ----- get user input (i and j) -----
    leaq	-20(%rbp), %rsi         # %rsi = address on the stack where the first int will be stored
    movq    $.SCANF_INT, %rdi       # %rdi = the address of the format string
    xorq	%rax, %rax
    call	scanf                   # call scanf

    leaq	-24(%rbp), %rsi         # %rsi = address on the stack where the second int will be stored
    movq    $.SCANF_INT, %rdi       # %rdi = the address of the format string
    xorq	%rax, %rax
    call	scanf                   # call scanf    

    # ----- call pstrijcpy -----

    movq    -8(%rbp), %rdi      # %rdi = address of pstr1
    movq    -16(%rbp), %rsi     # %rsi = address of pstr2
    movl    -20(%rbp), %edx     # %edx = the first int received the user entered
    movl    -24(%rbp), %ecx     # %ecx = the second int received the user entered

    call    pstrijcpy

    # ----- printing of pstr1: -----

    movq    -8(%rbp), %rcx      # %rcx = address of pstr1
    incq    %rcx                # (%rcx)++, now points to the begining of string1

    movq    -8(%rbp), %rdi
    call    pstrlen
    movzbq  %al, %rsi   # %rsi = length of string1

    movq    $.FORMAT_STRING_54, %rdi # %rdi = the address of the format string
    movq    %rcx, %rdx  # %rdx = address of the string1

    xorq	%rax, %rax
    call	printf          # print the first string

    # ----- printing of pstr2: -----

    movq    -16(%rbp), %rcx     # %rcx = address of pstr2
    incq    %rcx                # (%rcx)++, now points to the begining of string2

    movq    -16(%rbp), %rdi
    call    pstrlen
    movzbq  %al, %rsi   # %rsi = length of string2

    movq    $.FORMAT_STRING_54, %rdi # %rdi = the address of the format string
    movq    %rcx, %rdx  # %rdx = address of the string2

    xorq	%rax, %rax
    call	printf      # print the first string

    addq    $16, %rsp   # add the 16 bytes back
    jmp .finish_run

# case 54
.OP54:
    movq    -8(%rbp), %rdi      # %rdi = address of pstr1
    call    swapCase    # swap case for pstr1
    movq    -16(%rbp), %rdi      # %rdi = address of pstr2
    call    swapCase    # swap case for pstr1

    # ----- printing of pstr1: -----

    movq    -8(%rbp), %rcx      # %rcx = address of pstr1
    incq    %rcx                # (%rcx)++, now points to the begining of string1

    movq    -8(%rbp), %rdi
    call    pstrlen
    movzbq  %al, %rsi   # %rsi = length of string1

    movq    $.FORMAT_STRING_54, %rdi # %rdi = the address of the format string
    movq    %rcx, %rdx  # %rdx = address of the string1

    xorq	%rax, %rax
    call	printf          # print the first string

    # ----- printing of pstr2: -----

    movq    -16(%rbp), %rcx     # %rcx = address of pstr2
    incq    %rcx                # (%rcx)++, now points to the begining of string2

    movq    -16(%rbp), %rdi
    call    pstrlen
    movzbq  %al, %rsi   # %rsi = length of string2

    movq    $.FORMAT_STRING_54, %rdi # %rdi = the address of the format string
    movq    %rcx, %rdx  # %rdx = address of the string2

    xorq	%rax, %rax
    call	printf          # print the first string

    jmp .finish_run

# case 55
.OP55:
    subq    $16, %rsp    # take 16 bytes from the stack

    # ----- get user input (i and j) -----
    leaq	-20(%rbp), %rsi     # %rsi = address on the stack where the first int will be stored
    movq    $.SCANF_INT, %rdi   # %rdi = the address of the format string
    xorq	%rax, %rax
    call	scanf               # call scanf

    leaq	-24(%rbp), %rsi     # %rsi = address on the stack where the second int will be stored
    movq    $.SCANF_INT, %rdi   # %rdi = the address of the format string
    xorq	%rax, %rax
    call	scanf               # call scanf    

    # ----- call pstrijcmp -----

    movq    -8(%rbp), %rdi      # %rdi = address of pstr1
    movq    -16(%rbp), %rsi     # %rsi = address of pstr2
    movl    -20(%rbp), %edx     # %edx = the first int received the user entered
    movl    -24(%rbp), %ecx     # %ecx = the second int received the user entered

    call    pstrijcmp

    # ----- print the result of the compare: -----

    movq    %rax, %rsi

    movq    $.FORMAT_STRING_55, %rdi # %rdi = the address of the format string

    xorq	%rax, %rax
    call	printf          # print the first string

    addq    $16, %rsp    # add the 16 bytes back
    jmp .finish_run

# the default cases
.OP51:
.OP56:
.OP57:
.OP58:
.OP59:
    jmp .DEFAULT_CASE
