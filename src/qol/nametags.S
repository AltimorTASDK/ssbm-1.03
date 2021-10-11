.section .text

.global hook_ShowNameTagCheck
hook_ShowNameTagCheck:
        # Check original condition
        cmpwi   r3, 3
        beq     end
        
        lbz     r3, 0(r31)
        bl      should_show_nametag
        cmpwi   r3, 0
        
end:
        # Skip overwritten instruction
        b       orig_ShowNameTagCheck+4
