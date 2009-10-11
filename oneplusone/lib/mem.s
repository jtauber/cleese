    .globl  bcopy
    .globl  _bcopy
    .align  4

bcopy:
_bcopy: 
    pushl   %esi
    pushl   %edi
    movl    0xC(%esp),%esi
    movl    0x10(%esp),%edi
    movl    0x14(%esp),%ecx
    cmpl    %edi,%esi       /* Potential ripple in copy? */
    jb  2f

1:  cld
    cmpl    $4,%ecx         /* Move longwords */
    jb  5f
    shrl    $2,%ecx         /* Scale units */
    rep
    movsl
    movl    0x14(%esp),%ecx
    andl    $3,%ecx
    jnz 5f
    popl    %edi            /* Restore registers and return */
    popl    %esi
    ret

5:  rep             /* Resid copy of bytes */
    movsb
    popl    %edi            /* Restore registers and return */
    popl    %esi
    ret

    .align  4,0x90

2:  addl    %ecx,%esi       /* If no overlap, still copy forward */
    cmpl    %edi,%esi
    jae 4f
    subl    %ecx,%esi       /* Restore register */
    jmp 1b          /* Forward copy */

4:  std             /* Overlap; copy backwards */
    decl    %esi
    addl    %ecx,%edi
    decl    %edi
    rep
    movsb
    cld
    popl    %edi            /* Restore registers and return */
    popl    %esi
    ret
