#!/bin/bash

cat <<'EOF'
*************************************************************
*      github Repo   :    https://github.com/Jiansiyu
*      
*      📦 whoami  
*      🤐 
*      💻 emoji is cool
*      💖 provide with love 
*      
*      Youtube: https://www.youtube.com/channel/UCFBHlyED8_VZ2yfLXoAXrbg
*************************************************************
EOF

remoteServerUser='pi'
remoteServerURL='192.168.2.144'
remoteServerPasscode=''
IsPackageInstalled(){
    dpkg -s $1 &> /dev/null
    if [ $? -eq 0 ]; then
        return 0
    else
        return 1
    fi
}

if ! IsPackageInstalled zenity1; then 
    echo "GUI mode Disabled, working on the batch mode!!"
    echo "if you want to run on GUI mode, please install zenity by"
    echo "    sudo apt install zenity"
    echo 
    read -p "please input the [username] of remote server:" remoteServerUser
    read -p "please input the [URL/IP] of remote server:" remoteServerURL
    echo "please input the passcode of ${remoteServerUser}@${remoteServerURL}:"
    read -s remoteServerPasscode
else
    result=$(zenity --forms --title="Remote Server Setting" --text="Remote Server" \
   --add-entry="Server User" \
   --add-entry="Server URL"  \
   --add-password="passcode")

   remoteServerUser="$(echo "$result"| cut -d '|' -f 1)"
   remoteServerURL="$(echo "$result"| cut -d '|' -f 2)"
   remoteServerPasscode="$(echo "$result"| cut -d '|' -f 3)"
fi

# check the existance of the current client ssh keys
if [ ! -f ~/.ssh/id_rsa.pub ]; then
     ssh-keygen -t rsa
fi

# echo ${remoteServerUser}
# echo ${remoteServerURL}
# echo ${remoteServerPasscode}
# create folder if the folder does not exist
ssh ${remoteServerUser}@${remoteServerURL} mkdir -p .ssh
expect "password:"
send "${remoteServerPasscode}\r"
cat ~/.ssh/id_rsa.pub | ssh ${remoteServerUser}@${remoteServerURL} 'cat >> .ssh/authorized_keys'
expect "password:"
send "${remoteServerPasscode}\r"
