# SSBM 1.03

## Building

Invoke `make` with `VERSION` set to one of `102`, `101`, `100`, or `PAL`. This indicates the target Melee version.

`MODVERSION` may also be specified to add a suffix to the output gci filenames.

Example: `make -j8 VERSION=PAL MODVERSION=b3.4`

NTSC 1.02 must be built before any other versions, as all other versions are stored as a diff against the 1.02 build.
If the 1.02 version is rebuilt, all other versions must also be rebuilt to be valid.