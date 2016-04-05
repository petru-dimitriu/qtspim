# Slightly modified version of QtSpim, MIPS processor simulator
This is a version modified to tweak my and my fellow colleagues MIPS assembly studying experience.

Refer to the original README and http://spimsimulator.sourceforge.net/ for more info on QtSpim.

Modifications mainly consist of the following:
* The Program Counter register is set to `0x400000` automatically whenever an assembly file is loaded.
* The Data tab presents by default 1 dataword per line instead of 4 and the user can change this number in the Settings window.
* The program searches the working directory for and tries to load a Romanian language translation file for the Qt UI `qtspim_ro.qm`.

## Licence
Just as the original, this version of QtSpim is BSD-licensed.