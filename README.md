*This README is about the extension of G4HepEm with AD capabilities. See [README_ORIGINAL.md](README_ORIGINAL.md) for information about G4HepEm.*

# Differentiated G4HepEm
[G4HepEm](https://github.com/mnovak42/g4hepem/) is a compact, self-contained toolkit for electromagnetic shower simulation. This repository contains a version of G4HepEm that has been extended with operator-overloading algorithmic differentiation (AD) capabilities using CoDiPack [CoDiPack](https://github.com/SciCompKL/CoDiPack), making it possible to 
- propagate dot values through G4HepEm functionalities with **forward-mode AD**, and
- record G4HepEm calculations on a "tape" to implement operator-overloading **reverse-mode AD**.

This differentiated version of G4HepEm is used by a [differentiated version of HepEmshow](https://github.com/maxaehle/hepemshow) to compute derivatives in a simple calorimetry setup.

## Building from source
A version of G4HepEm without any AD capabilities can be built as usual: Download the code into some directory, enter the directory, checkout the `with_codipack` branch, and run
```bash
mkdir build
cd build
cmake ../ -DG4HepEm_GEANT4_BUILD=OFF -DCMAKE_INSTALL_PREFIX=$PWD/../install -DCMAKE_BUILD_TYPE=Release
make
make install
```
To create a forward-mode or reverse-mode AD build, supply `-DCODI_FORWARD=yes` or `-DCODI_REVERSE=yes` to the `cmake` call, respectively. Most likely, you also have to download the AD tool [CoDiPack](https://github.com/SciCompKL/CoDiPack) and specify its path to `cmake` via `-DCoDiPack_DIR=/path/to/CoDiPack/cmake`. 

If you want to make non-AD and AD builds at the same time, consider using directory names `build_no`/`build_ad` and `$PWD/../install_no`/`$PWD/../install_ad` instead of `build` and `$PWD/../install` in the above build commands.

## License Hints
[G4HepEm](https://github.com/mnovak42/g4hepem/) has been released under the [Apache-2.0 license](https://github.com/mnovak42/g4hepem/blob/master/LICENSE). Please note that CoDiPack has been released under the [GNU General Public License (GPL) version 3](https://github.com/SciCompKL/CoDiPack/blob/master/LICENSE), which also applies to the combined work.



