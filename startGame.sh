#a quick and dirty script for automating running the program after building
cd ../pru/led_pru
make install_PRU1
cd ../../
echo "back to start"
cd myApps
echo "in apps"
sudo ./pentris
echo "started"
