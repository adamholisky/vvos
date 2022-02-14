.set ASM_TEST_MAGIC, 0xADA11000

.section .text

.global process_test_alpha
.global process_test_alpha_end
.type process_test_alpha, @function
process_test_alpha:
	int $0x30
	int $0x32
	jmp process_test_alpha
	ret
process_test_alpha_end:
	nop

.global process_test_beta
.global process_test_beta_end
.type process_test_beta, @function
process_test_beta:
	int $0x31
	int $0x32
	jmp process_test_beta
	ret
process_test_beta_end:
	nop
