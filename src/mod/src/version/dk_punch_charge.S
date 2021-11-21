#ifdef PAL
.section .text

# The neutral B hit callback also has to be hooked because it redundantly calls
# the up B hit callback in PAL for some reason
.global hook_Player_DK_NeutralBHitCallback
.global hook_Player_DK_UpBHitCallback
hook_Player_DK_NeutralBHitCallback:
hook_Player_DK_UpBHitCallback:
        # Make poor DK lose his charge when he gets hit out of up B
        b       Player_DK_LoseChargeCallback

#endif