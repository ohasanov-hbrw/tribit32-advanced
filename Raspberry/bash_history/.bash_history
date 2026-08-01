df -h
sudo apt update && sudo apt upgrade
sudo swapon --show
htop
locale
sudo update-locale LC_ALL="en_US.UTF-8"
locale
sudo raspi-config
reboot
sudo reboot
sudo apt install git lsb-release
cd
git clone --depth=1 https://github.com/RetroPie/RetroPie-Setup.git
cd RetroPie-Setup
chmod +x retropie_setup.sh 
sudo ./retropie_setup.sh 
lsmod | grep -E "vc4|v3d"
sudo ./retropie_setup.sh 
locale
sudo update-locale LC_ALL="en_US.UTF-8"
locale
reboot
sudo reboot
cleaer
clear
locale
cd RetroPie-Setup/
ls
sudo ./retropie_setup.sh 
sudo apt install libfreeimage-dev
sudo apt install libcur14-openssl-dev libvlc-dev libvlccore-dev
sudo apt install libvlc-dev libvlccore-dev
sudo apt install rapidjson-dev omxplayer
ls
cd logs
ls
sudo ./retropie_setup.sh 
cd ..
sudo ./retropie_setup.sh 
sudo reboot
sudo nano /opt/retropie/configs/all/autoconf.cfg 
sudo nano /opt/retropie/configs/all/autostart.sh
pegasus-fe 
apt search openvg
nano /opt/retropie/supplementary/pegasus-fe/
nano /opt/retropie/supplementary/pegasus-fe/pegasus-fe 
nano /usr/bin/pegasus-fe 
sudo nano /usr/bin/pegasus-fe 

sudo nano /usr/bin/pegasus-fe 
pegasus-fe 
QT_QPA_PLATFORM=linuxfb pegasus-fe 
sudo nano RetroPie-Setup/retropie_setup.sh 
sudo ./RetroPie-Setup/retropie_setup.sh 
pegasus-fe 
export QT_QPA_EGLFS_PHYSICAL_WIDTH=154
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=96
export QT_QPA_EGLFS_PHYSICAL_WIDTH=154
pegasus-fe 
export QT_QPA_EGLFS_PHYSICAL_HEIGHT=48
export QT_QPA_EGLFS_PHYSICAL_WIDTH=77
pegasus-fe 
sudo ./RetroPie-Setup/retropie_setup.sh 
sudo raspi-config
sudo apt install raspi-config
raspi-config
sudo raspi-config
sudo reboot
pkill pegasus-fe
sudo ./RetroPie-Setup/retropie_setup.sh 
sudo apt install libcurl4-openssl-dev
nano RetroPie-Setup/scriptmodules/supplementary/emulationstation.sh 
sudo ./RetroPie-Setup/retropie_setup.sh 
sudo reboot
sudo ./RetroPie-Setup/retropie_setup.sh 
sudo reboot
ls
cd fbcp-nexus/
ls
cd build/
ls
cat build.sh 
sudo apt install cmake libevdev-lib
sudo apt search libevdev
sudo apt install cmake libevdev-dev
chmod +x build.sh
./build.sh 
make clean
rm CMakeCache.txt 
./build.sh 
ls
make -j\
sudo apt-get install libraspberrypi-dev
make -j
./fbcp-ili9341 
sudo ./fbcp-ili9341 
cd ..
ls
cd Raspberry-UART-Controller/
ls
make -j
make clean
make -j
./joystick_emulator 
sudo nano /boot/config.txt
sudo nano /boot/firmware/config.txt 
sudo reboot
ls
cd Raspberry-UART-Controller/
ls
./joystick_emulator 
sudo nano /boot/firmware/config.txt 
sudo nano /etc/systemd/system/fbcp.service
ls
cd fbcp-nexus/
cd build/
whereis 
whereis .
sudo nano /etc/systemd/system/fbcp.service
sudo nano /etc/systemd/system/promini.service
rc/linux-headers-6.12.87+rpt-rpi-v7l/. /usr/lib/linux-kbuild-6.12.75+rpt/.. /usr/lib/linux-kbuild-6.12.75+rpt/. /usr/src/linux-headers-6.12.75+rpt-rpi-v6/.. /usr/src/linux-headers-6.12.75+rpt-rpi-v6/.
ohasanov@tribit32:~/fbcp-nexus/build $ sudo nano /etc/systemd/syst
sudo systemctl daemon-reload
sudo systemctl enable fbcp
sudo systemctl enable promini
sudo systemctl start fbcp
sudo systemctl start promini
sudo systemctl status promini
sudo systemctl status fbcp
sudo reboot
sudo systemctl status promini
sudo systemctl status fbcp
amixer
amixer -l
amixer -L
sudo nano /boot/firmware/config.txt 
aplay -l
amixer
sudo nano /etc/modprobe.d/raspi-blacklist.conf
sudo nano /etc/modules
sudo nano /etc/asound.conf
nano ~/.asoundrc
sudo reboot
sudo systemctl status fbcp
sudo systemctl status promini
alsamixer
nano ~/.asoundrc
sudo nano /etc/asound.conf
nano ~/.asoundrc
sudo reboot
alsamixer
sudo nano /boot/firmware/config.txt 
sudo reboot
alsamixer
sudo nano /boot/firmware/config.txt 
sudo apt install python3-venv
python -m venv env --system-site-packages
source env/bin/activate
sudo apt install -y wget
pip3 install adafruit-python-shell
wget https://github.com/adafruit/Raspberry-Pi-Installer-Scripts/raw/main/i2samp.py
sudo -E env PATH=$PATH python3 i2samp.py
sudo apt install libraspberrypi-bin
sudo -E env PATH=$PATH python3 i2samp.py
alsamixer
sudo apt install raspi-utils-core
alsamixer
nano ~/.asoundrc 
sudo nano /boot/firmware/config.txt 
sudo reboot
alsamixer
nano ~/.asoundrc 
alsamixer
sudo nano /etc/asound.conf
alsamixer
sudo nano /etc/asound.conf
nano ~/.asoundrc 
sudo nano /etc/asound.conf
alsamixer
sudo reboot
cat /proc/asound/cards
cat /proc/asound/card0/pcm0p/info
aplay -l
alsamixer
sudo systemctl status promini
alsamixer
sudo systemctl disable promini
sudo systemctl stop promini
sudo systemctl status promini
ls
cd Raspberry-UART-Controller/
ls
aplay -l
nano config.prop
./joystick_emulator 
nano config.prop
./joystick_emulator 
aplay -l
amixer -c HifiBerry DAC HiFi pcm5102a-hifi-0 [HifiBerry DAC HiFi pcm5102a-hifi-0] info
amixer -c "HifiBerry DAC HiFi pcm5102a-hifi-0 [HifiBerry DAC HiFi pcm5102a-hifi-0]" info
amixer -c 0 info
nano config.prop 
./joystick_emulator 
amixer sset snd_rpi_hifiberry_dac 5 -
amixer scontrols
amixer sset PCM 5 -
nano config.prop 
./joystick_emulator 
sudo systemctl enable promini
sudo systemctl satrt promini
sudo systemctl start promini
sudo systemctl status promini
sudo reboot
sudo systemctl status promini
what
sudo systemctl restart promini
sudo systemctl status promini
sudo shutdown -h now
pkill emulationstation
htop
sudo systemctl status fbcp
sudo systemctl restart fbcp
sudo raspi-config
speaker-test -c2 --test=wav -w /usr/share/sounds/alsa/Front_Center.wav
htop
sudo nano /opt/retropie/configs/all/autostart.sh 
sudo reboot
fftest
evtest
sudo apt install evtest
evtest
sudo systemctl status promini
sudo shutdown -h now
sudo systemctl restart fbcp
sudo systemctl status promini
evtest
sudo systemctl restart promini
evtest
sudo shutdown -h now
clear
ls
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo systemctl stop promini
sudo systemctl status promini
sudo systemctl disable promini
sudo systemctl status promini
screen /dev/serial0 57600
sudo apt install screen
screen /dev/serial0 57600
sudo systemctl enable promini
sudo systemctl start promini
sudo systemctl status promini
sudo systemctl disable hciuart
sudo systemctl disable bluetooth
sudo systemctl disable promini
sudo systemctl status promini
sudo systemctl stop promini
screen /dev/serial0 57600
sudo nano /boot/firmware/cmdline.txt 
sudo reboot
sudo nano /boot/firmware/cmdline.txt 
sudo nano /boot/cmdline.txt 
MINICOM
sudo apt install minicom
minicom
minicom -s
minicom
pkill screen
sudo systemctl enable promini
sudo systemctl status promini
sudo systemctl start promini
evtest
sudo systemctl disable promini
sudo systemctl stop promini
screen /dev/serial0 57600
clear
minicom -b 9600 -o -D /dev/serial0
minicom -b 57600 -o -D /dev/serial0
ls
cd Raspberry-UART-Controller/
ls
nano config.prop 
ls -l /dev | grep ttyAMA0 && ls -l /dev | grep ttyS0
sudo nano /boot/firmware/config.txt 
sudo reboot
pkill screen
ls -l /dev | grep ttyAMA0 && ls -l /dev | grep ttyS0
sudo usermod -a -G dialout ohasanov
sudo usermod -a -G tty ohasanov
sudo apt install picocom
picocom /dev/ttyAMA0 -b 57600 --imap lfcrlf --echo
screen /dev/ttyAMA0 57600
sudo nano /boot/firmware/config.txt 
sudo reboot
evtest
ls -l /dev | grep ttyAMA0 && ls -l /dev | grep ttyS0
minicom -b 57600 -o -D /dev/serial0
sudo shutdown -h now
sudo systemctl enable promini
sudo systemctl start promini
sudo systemctl disable promini
sudo systemctl stop promini
ls -l /dev/serial*
sudo minicom -b 115200 -D /dev/serial0 -o
sudo minicom -b 57600 -D /dev/serial0 -o
sudo systemctl start promini
evtest
sudo evtest
sudo systemctl sttop promini
sudo systemctl stop promini
ls
cd Raspberry-UART-Controller/
ls
sudo minicom -b 57600 -D /dev/serial0 -o
ls
make clean
make
./joystick_emulator 
sudo apt-get install libevdev-dev
nano pico_monitor.
nano pico_monitor.c
nano pico_monitor.cpp
make
./joystick_emulator 
nano pico_monitor.cpp
make
./joystick_emulator 
sudo shutdown -h now
cd Raspberry-UART-Controller/
./joystick_emulator 
nano pico_monitor.cpp
make
sudo systemctl start promini
evtest
sudo systemctl stop promini
nano pico_monitor.cpp
make
./joystick_emulator 
sudo shutdown -h now
./joystick_emulator 
cd Raspberry-UART-Controller/
nano pico_monitor.cpp 
make
sudo systemctl enable promini
sudo systemctl start promini
sudo systemctl disable promini
sudo systemctl stop promini
nano controller.cpp 
nano controller.h 
nano config.prop 
sudo systemctl enable promini
sudo systemctl start promini
sudo systemctl restart fbcp
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo systemctl restart fbcp
sudo systemctl status fbcp
sudo raspi-config
sudo apt install raspi-config
raspi-config
sudo raspi-config
sudo apt install  libraspberrypi-dev libraspberrypi0 vcdbg
sudo nano /boot/firmware/config.txt 
sudo nano /boot/firmware/cmdline.txt 
sudo reboot
sudo systemctl status fbcp
sudo systemctl restart fbcp
sudo nano /opt/retropie/configs/all/autostart.sh 
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo systemctl status fbcp
ls
cd RetroPie-Setup/
ls
sudo ./retropie_setup.sh 
cd /opt/retropie/configs/all
ls
nano runcommand.cfg
nano runcommand-onstart.sh
nano runcommand-onlaunch.sh
nano autostart.sh
htop
nano autostart.sh
sudo reboot
cd /opt/retropie/configs/all
nano autostart.sh
nano runcommand-onend.sh
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo shutdown -h now
sudo ./RetroPie-Setup/retropie_setup.sh 
ls
cd Raspberry-UART-Controller/
ls
nano main.cpp
ls
make 
pkill joystick_emulator
ls
htop
sudo systemctl restart promini
nano main.cpp
make \
sudo systemctl restart promini
nano main.cpp
make 
sudo systemctl restart promini
nano main.cpp
sudo nano /opt/retropie/configs/all/autostart.sh
cd ..
cd fbcp-nexus/
ls
cd build/
ls
nano build.sh 
./build.sh 
make
sudo nano /boot/firmware/cmdline.txt
sudo reboot
systemctl status fbcp
ls
cd fbcp-nexus/
ls
nano config.h
cd build/
./build.sh 
make
sudo systemctl restart fbcp
sudo systemctl status fbcp
htop
sudo nano /etc/systemd/system/fbcp.service
sudo systemctl daemon-reload
sudo systemctl restart fbcp
htop
sudo systemctl status fbcp
sudo nano /etc/systemd/system/fbcp.service
sudo systemctl daemon-reload
sudo systemctl restart fbcp
sudo nano /etc/systemd/system/fbcp.service
sudo nano /boot/firmware/cmdline.txt
sudo reboot
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo nano /opt/retropie/configs/all/autostart.sh
sudo nano /opt/retropie/configs/all/runcommand-onstart.sh 
sudo nano /opt/retropie/configs/all/runcommand-onend.sh 
sudo nano /opt/retropie/configs/all/runcommand-onlaunch.sh 
sudo reboot
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo nano /boot/firmware/config.txt 
sudo reboot
sudo systemctl status fbcp
sudo nano /boot/firmware/config.txt 
cat /sys/class/graphics/fb0/modes
sudo nano /boot/firmware/config.txt 
sudo reboot
cat /sys/class/graphics/fb0/modes
sudo nano /boot/firmware/cmdline.txt
sudo reboot
sudo nano /boot/firmware/cmdline.txt
sudo reboot
