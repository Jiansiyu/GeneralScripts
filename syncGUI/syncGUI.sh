#!/bin/bash
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

zenity --forms --title "Select Flash Drives" --text  "Select Flash Drives" \
               --add-list "Insert your choice" --list-values 'a|b|c' \
               --add-entry="IP address" --add-entry="PORT"

zenity --forms --title="rync GUI" --text="sync GUI" \
   --add-entry="First Name" \
   --add-entry="Last Name" \
   --add-entry="Username" \
   --add-password="Password" \
   --add-password="Confirm Password" \
   --add-calendar="Expires"

# TargetDir='./'
# sourceDir='./'
# prefex='*'
# HostShareDir='./'
# if ! IsPackageInstalled zenity; then 
#     echo 'please input the Director you want to share with Docker'
#     read -p "Input Selection:" HostShareDir
#     else
#     HostShareDir="$(zenity --file-selection --directory  --title='Choose a Share directory')"
# fi

# if [ ! -d ${HostShareDir} ]; then
#     echo  "${RED}[ERROR] ${NC} Cannot find folder ${HostShareDir}"
#     echo  "Please choose another folder or create a new one !!"
#     exit -1
# fi