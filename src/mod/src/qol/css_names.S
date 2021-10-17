.section .text

.global hook_CreateCSSNameText
hook_CreateCSSNameText:
        mr      r3, r29 # port
        lwz     r4, 0(r27) # text
        bl      set_css_name_color
        b       orig_CreateCSSNameText
