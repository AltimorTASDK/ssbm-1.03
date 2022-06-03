# SSBM 1.03

## Overview

1.03 consists of a loader, located in `src/loader`, which is built into a Melee
save file using dansalvato's Melee GCI Compiler, and the mod itself, located in
`src/mod`.

The loader is executed upon entering *Name Entry* using the standard nametag
buffer overflow exploit. The loader then copies itself out of the nametag
region and reads the `103Code` save (output as `ssbm-1.03-$(MODVERSION).gci`)
containing the mod from the memory card into main memory. `103Code` contains a
full copy of 1.03 for Melee NTSC 1.02 and diffs against this copy for other game
versions, which the loader will apply if necessary. The loader then executes the
mod entry point and makes Melee load an alternate save file that won't trigger
the nametag overflow.

1.03 uses this two-stage loading process because its main binary is too large to
fit in a Melee save file.

## Build environment

You need to have [devkitPro](https://devkitpro.org/wiki/Getting_Started)
installed, and the environment variable `DEVKITPRO` must be set to your
devkitPro install path.

You need at least Python 3.6.

You need to install the `pypng` Python library.
* Example: `pip install pypng`

All submodules in the 1.03 repo must be cloned.
* Example: `git submodule update --init --recursive`

## Building

Invoke `make` with `VERSION` set to one of `102`, `101`, `100`, or `PAL`. This
indicates the target Melee version.

`NOPAL` may be specified to exclude PAL, 20XX, and UnclePunch support from the
build. This helps 1.03 fit on 4Mb memory cards.

`BETA` may be specified to include a version watermark on the character select
screen.

`MODVERSION` may be specified to add a suffix to the output gci filenames.
`MODVERSION` may imply other build options.

* If `MODVERSION` starts with "a" followed by a digit, hyphen, or EOL, `NOPAL`
is implied.
* * Example: `MODVERSION=a3.4`
* If `MODVERSION` contains "beta" or "rc" followed by a digit, hyphen, or EOL
at the start or after a hyphen, `BETA` is implied.
* * Example: `MODVERSION=b3.4-beta1`

`USERDEFS` may be specified to supply additional preprocessor macros. Additional
functionality may be added using the following macros:

* `CYAN_CSS_READY` - Recolor the "Ready to fight!" banner to cyan
* `OLD_CSS_TOGGLES` - Allow CSS toggles for 1.03 controls like the first version
of 1.03
* `POLL_DEBUG` - Show debug information on the game's input poll/fetch timing
and latency
* `POLL_DEBUG_VERBOSE` - Show additional poll debug info
* `UCF_ROTATOR` - Reinstate the UCF toggle in the additional rules menu

**Example `make` command:**

    make -j8 USERDEFS=OLD_CSS_TOGGLES VERSION=PAL MODVERSION=b3.4

NTSC 1.02 must be built before any other versions, as all other versions are
stored as a diff against the 1.02 build.  If the 1.02 version is rebuilt, all
other versions must also be rebuilt to be valid.