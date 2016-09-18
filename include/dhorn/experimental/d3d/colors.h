/*
 * Duncan Horn
 *
 * colors.h
 *
 * Various colors. Note that some colors may be defined more than once (same value, different names) in order to comply
 * with standard color names as much as possible. Note that this isn't always possible (e.g. 'true' green is the same
 * as HTML's 'lime' color and is twice the intensity of HTML's green color which is represented here as 'low_green')
 */
#pragma once

#include <DirectXMath.h>

namespace dhorn
{
    namespace experimental
    {
        namespace d3d
        {
            namespace colors
            {
                inline constexpr DirectX::XMVECTORF32 from_rgba(size_t red, size_t green, size_t blue, size_t alpha = 255)
                {
                    return DirectX::XMVECTORF32
                    {
                        static_cast<float>(red) / 255.0f,
                        static_cast<float>(green) / 255.0f,
                        static_cast<float>(blue) / 255.0f,
                        static_cast<float>(alpha) / 255.0f
                    };
                }

                // "Standard" colors
                const DirectX::XMVECTORF32 red = { 1.0f, 0.0f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 green = { 0.0f, 1.0f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 blue = { 0.0f, 0.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 magenta = { 1.0f, 0.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 white = { 1.0f, 1.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 black = { 0.0f, 0.0f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 transparent = { 1.0f, 1.0f, 1.0f, 0.0f };

                // HTML colors (for the most part)
                const DirectX::XMVECTORF32 silver = { 0.75f, 0.75f, 0.75f, 1.0f };
                const DirectX::XMVECTORF32 gray = { 0.5f, 0.5f, 0.5f, 1.0f };
                const DirectX::XMVECTORF32 olive = { 0.5f, 0.5f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 lime = { 0.0f, 1.0f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 aqua = { 0.0f, 1.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 teal = { 0.0f, 0.5f, 0.5f, 1.0f };
                const DirectX::XMVECTORF32 navy = { 0.0f, 0.0f, 0.5f, 1.0f };
                const DirectX::XMVECTORF32 fuchsia = { 1.0f, 0.0f, 1.0f, 1.0f };
                const DirectX::XMVECTORF32 purple = { 0.5f, 0.0f, 0.5f, 1.0f };
                const DirectX::XMVECTORF32 low_green = { 0.0f, 0.5f, 0.0f, 1.0f };
                const DirectX::XMVECTORF32 maroon = { 0.5f, 0.0f, 0.0f, 1.0f };

                // Pink colors
                const DirectX::XMVECTORF32 pink = from_rgba(255, 192, 203);
                const DirectX::XMVECTORF32 light_pink = from_rgba(255, 182, 193);
                const DirectX::XMVECTORF32 hot_pink = from_rgba(255, 105, 180);
                const DirectX::XMVECTORF32 deep_pink = from_rgba(255, 20, 147);
                const DirectX::XMVECTORF32 pale_violet_red = from_rgba(219, 112, 147);
                const DirectX::XMVECTORF32 medium_violet_red = from_rgba(199, 21, 133);

                // Red colors
                const DirectX::XMVECTORF32 light_salmon = from_rgba(255, 160, 122);
                const DirectX::XMVECTORF32 salmon = from_rgba(250, 128, 114);
                const DirectX::XMVECTORF32 dark_salmon = from_rgba(233, 150, 122);
                const DirectX::XMVECTORF32 light_coral = from_rgba(240, 128, 128);
                const DirectX::XMVECTORF32 indian_red = from_rgba(205, 92, 92);
                const DirectX::XMVECTORF32 crimson = from_rgba(220, 20, 60);
                const DirectX::XMVECTORF32 fire_brick = from_rgba(178, 34, 34);
                const DirectX::XMVECTORF32 dark_red = from_rgba(139, 0, 0);

                // Orange colors
                const DirectX::XMVECTORF32 burnt_orange = from_rgba(204, 85, 0);
                const DirectX::XMVECTORF32 orange_red = from_rgba(255, 69, 0);
                const DirectX::XMVECTORF32 tomato = from_rgba(255, 99, 71);
                const DirectX::XMVECTORF32 coral = from_rgba(255, 127, 80);
                const DirectX::XMVECTORF32 dark_orange = from_rgba(255, 140, 0);
                const DirectX::XMVECTORF32 orange = from_rgba(255, 165, 0);

                // Yellow colors
                const DirectX::XMVECTORF32 light_yellow = from_rgba(255, 255, 224);
                const DirectX::XMVECTORF32 lemon_chiffon = from_rgba(255, 250, 205);
                const DirectX::XMVECTORF32 light_goldenrod_yellow = from_rgba(250, 250, 210);
                const DirectX::XMVECTORF32 papaya_whip = from_rgba(255, 239, 213);
                const DirectX::XMVECTORF32 moccasin = from_rgba(255, 228, 181);
                const DirectX::XMVECTORF32 peach_puff = from_rgba(255, 218, 185);
                const DirectX::XMVECTORF32 pale_goldenrod = from_rgba(238, 232, 170);
                const DirectX::XMVECTORF32 khaki = from_rgba(240, 230, 140);
                const DirectX::XMVECTORF32 dark_khaki = from_rgba(189, 183, 107);
                const DirectX::XMVECTORF32 gold = from_rgba(255, 215, 0);

                // Brown colors
                const DirectX::XMVECTORF32 cornsilk = from_rgba(255, 248, 220);
                const DirectX::XMVECTORF32 blanched_almond = from_rgba(255, 235, 205);
                const DirectX::XMVECTORF32 bisque = from_rgba(255, 228, 196);
                const DirectX::XMVECTORF32 navajo_white = from_rgba(255, 222, 173);
                const DirectX::XMVECTORF32 wheat = from_rgba(245, 222, 179);
                const DirectX::XMVECTORF32 burlyWood = from_rgba(222, 184, 135);
                const DirectX::XMVECTORF32 tan = from_rgba(210, 180, 140);
                const DirectX::XMVECTORF32 rosy_brown = from_rgba(188, 143, 143);
                const DirectX::XMVECTORF32 sandy_brown = from_rgba(244, 164, 96);
                const DirectX::XMVECTORF32 goldenrod = from_rgba(218, 165, 32);
                const DirectX::XMVECTORF32 dark_goldenrod = from_rgba(184, 134, 11);
                const DirectX::XMVECTORF32 peru = from_rgba(205, 133, 63);
                const DirectX::XMVECTORF32 chocolate = from_rgba(210, 105, 30);
                const DirectX::XMVECTORF32 saddle_brown = from_rgba(139, 69, 19);
                const DirectX::XMVECTORF32 sienna = from_rgba(160, 82, 45);
                const DirectX::XMVECTORF32 brown = from_rgba(165, 42, 42);

                // Green colors
                const DirectX::XMVECTORF32 dark_olive_green = from_rgba(85, 107, 47);
                const DirectX::XMVECTORF32 olive_drab = from_rgba(107, 142, 35);
                const DirectX::XMVECTORF32 yellow_green = from_rgba(154, 205, 50);
                const DirectX::XMVECTORF32 lime_green = from_rgba(50, 205, 50);
                const DirectX::XMVECTORF32 lawn_green = from_rgba(124, 252, 0);
                const DirectX::XMVECTORF32 chartreuse = from_rgba(127, 255, 0);
                const DirectX::XMVECTORF32 green_yellow = from_rgba(173, 255, 47);
                const DirectX::XMVECTORF32 spring_green = from_rgba(0, 255, 127);
                const DirectX::XMVECTORF32 medium_spring_green = from_rgba(0, 250, 154);
                const DirectX::XMVECTORF32 light_green = from_rgba(144, 238, 144);
                const DirectX::XMVECTORF32 pale_green = from_rgba(152, 251, 152);
                const DirectX::XMVECTORF32 dark_sea_green = from_rgba(143, 188, 143);
                const DirectX::XMVECTORF32 medium_sea_green = from_rgba(60, 179, 113);
                const DirectX::XMVECTORF32 sea_green = from_rgba(46, 139, 87);
                const DirectX::XMVECTORF32 forest_green = from_rgba(34, 139, 34);
                const DirectX::XMVECTORF32 dark_green = from_rgba(0, 100, 0);

                // Cyan colors
                const DirectX::XMVECTORF32 medium_aquamarine = from_rgba(102, 205, 170);
                const DirectX::XMVECTORF32 light_cyan = from_rgba(224, 255, 255);
                const DirectX::XMVECTORF32 pale_turquoise = from_rgba(175, 238, 238);
                const DirectX::XMVECTORF32 aquamarine = from_rgba(127, 255, 212);
                const DirectX::XMVECTORF32 turquoise = from_rgba(64, 224, 208);
                const DirectX::XMVECTORF32 medium_turquoise = from_rgba(72, 209, 204);
                const DirectX::XMVECTORF32 dark_turquoise = from_rgba(0, 206, 209);
                const DirectX::XMVECTORF32 light_sea_green = from_rgba(32, 178, 170);
                const DirectX::XMVECTORF32 cadet_blue = from_rgba(95, 158, 160);
                const DirectX::XMVECTORF32 dark_cyan = from_rgba(0, 139, 139);

                // Blue colors
                const DirectX::XMVECTORF32 light_steel_blue = from_rgba(176, 196, 222);
                const DirectX::XMVECTORF32 powder_blue = from_rgba(176, 224, 230);
                const DirectX::XMVECTORF32 light_blue = from_rgba(173, 216, 230);
                const DirectX::XMVECTORF32 sky_blue = from_rgba(135, 206, 235);
                const DirectX::XMVECTORF32 light_sky_blue = from_rgba(135, 206, 250);
                const DirectX::XMVECTORF32 deep_sky_blue = from_rgba(0, 191, 255);
                const DirectX::XMVECTORF32 dodger_blue = from_rgba(30, 144, 255);
                const DirectX::XMVECTORF32 cornflower_blue = from_rgba(100, 149, 237);
                const DirectX::XMVECTORF32 steel_blue = from_rgba(70, 130, 180);
                const DirectX::XMVECTORF32 royal_blue = from_rgba(65, 105, 225);
                const DirectX::XMVECTORF32 medium_blue = from_rgba(0, 0, 205);
                const DirectX::XMVECTORF32 dark_blue = from_rgba(0, 0, 139);
                const DirectX::XMVECTORF32 midnight_blue = from_rgba(25, 25, 112);

                // Purple/Violet/Magenta colors
                const DirectX::XMVECTORF32 lavender = from_rgba(230, 230, 250);
                const DirectX::XMVECTORF32 thistle = from_rgba(216, 191, 216);
                const DirectX::XMVECTORF32 plum = from_rgba(221, 160, 221);
                const DirectX::XMVECTORF32 violet = from_rgba(238, 130, 238);
                const DirectX::XMVECTORF32 orchid = from_rgba(218, 112, 214);
                const DirectX::XMVECTORF32 medium_orchid = from_rgba(186, 85, 211);
                const DirectX::XMVECTORF32 medium_purple = from_rgba(147, 112, 219);
                const DirectX::XMVECTORF32 blue_violet = from_rgba(138, 43, 226);
                const DirectX::XMVECTORF32 dark_violet = from_rgba(148, 0, 211);
                const DirectX::XMVECTORF32 dark_orchid = from_rgba(153, 50, 204);
                const DirectX::XMVECTORF32 dark_magenta = from_rgba(139, 0, 139);
                const DirectX::XMVECTORF32 indigo = from_rgba(75, 0, 130);
                const DirectX::XMVECTORF32 dark_slate_blue = from_rgba(72, 61, 139);
                const DirectX::XMVECTORF32 rebecca_purple = from_rgba(102, 51, 153);
                const DirectX::XMVECTORF32 slate_blue = from_rgba(106, 90, 205);
                const DirectX::XMVECTORF32 medium_slate_blue = from_rgba(123, 104, 238);

                // White colors
                const DirectX::XMVECTORF32 snow = from_rgba(255, 250, 250);
                const DirectX::XMVECTORF32 honeydew = from_rgba(240, 255, 240);
                const DirectX::XMVECTORF32 mint_cream = from_rgba(245, 255, 250);
                const DirectX::XMVECTORF32 azure = from_rgba(240, 255, 255);
                const DirectX::XMVECTORF32 alice_blue = from_rgba(240, 248, 255);
                const DirectX::XMVECTORF32 ghost_white = from_rgba(248, 248, 255);
                const DirectX::XMVECTORF32 white_smoke = from_rgba(245, 245, 245);
                const DirectX::XMVECTORF32 seashell = from_rgba(255, 245, 238);
                const DirectX::XMVECTORF32 beige = from_rgba(245, 245, 220);
                const DirectX::XMVECTORF32 old_lace = from_rgba(253, 245, 230);
                const DirectX::XMVECTORF32 floral_white = from_rgba(255, 250, 240);
                const DirectX::XMVECTORF32 ivory = from_rgba(255, 255, 240);
                const DirectX::XMVECTORF32 antique_white = from_rgba(250, 235, 215);
                const DirectX::XMVECTORF32 linen = from_rgba(250, 240, 230);
                const DirectX::XMVECTORF32 lavender_blush = from_rgba(255, 240, 245);
                const DirectX::XMVECTORF32 misty_rose = from_rgba(255, 228, 225);

                // Gray/Black colors
                const DirectX::XMVECTORF32 gainsboro = from_rgba(220, 220, 220);
                const DirectX::XMVECTORF32 light_grey = from_rgba(211, 211, 211);
                const DirectX::XMVECTORF32 dark_gray = from_rgba(169, 169, 169);
                const DirectX::XMVECTORF32 dim_gray = from_rgba(105, 105, 105);
                const DirectX::XMVECTORF32 light_slate_gray = from_rgba(119, 136, 153);
                const DirectX::XMVECTORF32 slate_gray = from_rgba(112, 128, 144);
                const DirectX::XMVECTORF32 dark_slate_gray = from_rgba(47, 79, 79);
            }
        }
    }
}
