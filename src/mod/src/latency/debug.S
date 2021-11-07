
.section .text

.global hook_SceneLoopStart
hook_SceneLoopStart:
        bl      scene_loop_start
        b       orig_SceneLoopStart

.global hook_FirstEngineFrame
hook_FirstEngineFrame:
        bl      first_engine_frame
        b       orig_FirstEngineFrame
