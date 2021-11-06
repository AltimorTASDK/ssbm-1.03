.section .text

.global hook_SceneLoopStart
hook_SceneLoopStart:
        bl      scene_loop_start
        b       orig_SceneLoopStart