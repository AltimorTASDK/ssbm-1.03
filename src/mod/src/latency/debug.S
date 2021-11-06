
.section .text

.global hook_FirstEngineFrame
hook_FirstEngineFrame:
        bl      first_engine_frame
        b       orig_FirstEngineFrame
