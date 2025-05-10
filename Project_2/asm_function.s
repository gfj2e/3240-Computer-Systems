.global MysteryFunction5

.text
MysteryFunction5:
	xorl	%esi, %edi
	movl	$0, %eax
	jmp	.L19
.L20:
	movl	%edi, %edx
	andl	$1, %edx
	addl	%edx, %eax
	sarl	%edi
.L19:
	testl	%edi, %edi
	jne	.L20
	ret
