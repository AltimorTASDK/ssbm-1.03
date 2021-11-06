#pragma once

constexpr auto aspect_ratio_vanilla = 913.f / 750.f;
constexpr auto aspect_ratio_factor = 320.f / 219.f;
constexpr auto aspect_ratio_wide = aspect_ratio_vanilla * aspect_ratio_factor;

// Adjust ortho camera bounds so that 0 to 640 covers a 4:3 physical area
constexpr auto ortho_left_wide  = 320.f * (1.f - 320.f / 219.f);
constexpr auto ortho_right_wide = 320.f * (1.f + 320.f / 219.f);