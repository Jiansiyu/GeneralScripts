# ssh toolkit

A very naive tool used for send the current client  ssh key to the remote server. It can auto detector the client enviroment to choose GUI or Batch Mode.

* check the existance of local .ssh/id_rsa.pub. If not, generate a new one 
* login to the servere and create .ssh folder if does not exist
* send the local pub key to the server


### 1. GUI mode
GUI mode need zenity support. If zenity is not installed:

```
sudo apt install zenity -y
```

![zenity_gui](zenity_gui.png)

### 2. batch mode
![batchmode](batch.png)
## Usage

```
bash autossSet.sh
```

or 

```
curl -sL https://raw.githubusercontent.com/Jiansiyu/GeneralScripts/master/autoLogon/autosshSet.sh | bash 

```
