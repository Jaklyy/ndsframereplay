A tool to replay frame dumps captured in emulator, supports playback on real hardware.
    - By Jakly

Building:
    Use blocksds.
    Also make a folder called "fs" because i think it needs that to build without complaining.
    Frame dump data can be embedded into the romfs by building with it placed into a folder named "fs".

Loading Frame Dumps:
    Frame dumps can be loaded from either root of sd, romfs, or flashcart.
    Frame dumps will be loaded from a folder in root called "framedumps", should it exist. (does not apply to romfs).

Generating Frame Dumps:
    Frame dumps can be generated from certain emulators.
    Supported Emulators Include:
        melonDS (not merged yet) --- https://github.com/melonDS-emu/melonDS/actions?query=branch%3Aframedump
