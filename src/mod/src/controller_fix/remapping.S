.section .text

.global hook_UpdatePadButtons
hook_UpdatePadButtons:
        mr      r3, r24 # port
        bl      process_remapping
        b       orig_UpdatePadButtons
