# SmileBASIC-CYX Launcher

This GitHub repository contains the source code for the launcher of the [SmileBASIC-CYX plugin](https://github.com/CyberYoshi64/PTC3-Plugin).

---

# Credits

- [devkitPro](https://github.com/devkitPro)
    - Toolset used for compiling Nintendo 3DS-compatible executables
- [PabloMK7](https://github.com/PabloMK7)
    - Inspiration

---

# Building

You need the following items:

- [A modified version of `bannertool`](https://github.com/CyberYoshi64/bannertool)
- [`makerom`](https://github.com/3DSGuy/Project_CTR)
- devkitPro with all 3DS-based packages installed
- [A slightly modified version of Citro2D that addresses this issue](https://github.com/devkitPro/citro2d/issues/46) (not entirely needed but recommended)


## Why the modified builds?

`bannertool` unfortunately is archived, so no pull requests can be made there...

### Relevant changes made to `bannertool`

The modified version of bannertool adds support to specify the smaller icon (i.e. when shrinking the icons in HOME Menu enough) instead of generating it from the main icon.

I did this because I feel like it's a nice addition to fine-tune the look of the icon in its smaller format.

~~If Nintendo did this a lot, why shouldn't we let this be possible, at least as an optional perk?~~
