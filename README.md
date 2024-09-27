# CMPT 433 Pentris

An implementation of a pentomino block stacking game on a BeagleBone Green.

## Usage

- Build with CMake (copies files to shared directory `~/cmpt433/public/`)
- On target, navigate to `/mnt/remote/pru/led_pru/` and run `make`, then `make install_PRU1`
- Navigate to `/mnt/remote/myApps` and run `sudo ./pentris`

The `startGame.sh` file should be copied manually into `myApps` to automate starting the program.

## Demo
![demo.png](https://github.com/martin-tesar-DGBR/Pentris/blob/master/example/demo.png?raw=true)
