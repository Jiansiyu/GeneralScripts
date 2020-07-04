#!/bin/bash 

###########################
#Bash script that used for run the root_gui docker
#Author: Siyu Jian
#email: jiansiyu@gmail.com
##########################

RED=`tput setaf 1`
GREEN=`tput setaf 2`
NC=`tput sgr0`

IsPackageInstalled(){
    dpkg -s $1 &> /dev/null
    if [ $? -eq 0 ]; then
        return 0
    else
        return 1
    fi
}

if ! IsPackageInstalled docker; then 
    echo "${RED}[ERROR] ${NC} ${GREEN}docker${NC} is Not Detected, please install docker properly"
    echo "  https://docs.docker.com/get-docker/"
    exit -1
fi

if ! IsPackageInstalled x11-xserver-utils; then 
    echo "${RED}[ERROR] ${NC} ${GREEN}x11-xserver-utils${NC} is Not Detected. Please install the package"
    echo "      apt install x11-xserver-utils " 
    echo "Exit with ${RED} ERROR ${NC}"
    exit -1
fi

echo "************************"
echo " *  ROOT_GUI Docker  *"
echo " *"
echo " * email: jiansiyu@gmail.com"
echo "************************"


HostShareDir='./'
if ! IsPackageInstalled zenity; then 
    echo 'please input the Director you want to share with Docker'
    read -p "Input Selection:" HostShareDir
    else
    HostShareDir="$(zenity --file-selection --directory  --title='Choose a Share directory')"
fi

if [ ! -d ${HostShareDir} ]; then
    echo  "${RED}[ERROR] ${NC} Cannot find folder ${HostShareDir}"
    echo  "Please choose another folder or create a new one !!"
    exit -1
fi

xhost +

sudo docker run  -v ${HostShareDir}:/home/rootGUI/data   -v /etc/localtime:/etc/localtime:ro -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY -e GDK_SCALE -e GDK_DPI_SCALE -t uvasiyu/root_gui

