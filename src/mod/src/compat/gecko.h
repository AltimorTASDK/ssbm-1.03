#pragma once

#if defined(NTSC102) && !defined(DOL) && !defined(NOPAL) && !defined(STEALTH)
void check_hdmi_prompts();
#else
consteval void check_hdmi_prompts() {}
#endif