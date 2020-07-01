# ROOT GUI Docker Image 

# run ROOT GUI In Docker Image

## pre-start

* [set up docker container](https://docs.docker.com/engine/install/)

* enable GUI transmission 
```
sudo apt-get install x11-xserver-utils
```
* Get the docker image
```
docker pull uvasiyu/root_gui
```
## Start the ROOT_GUI Image

### Option 1, start the docker manually 
* Enable host transmission. Each time robot your PC you may need to run this command

```
xhost +
```

* run docker 
[host data file path] needs to be replaced by the full path of the folder that contains the .dat file. All your result also need to save to this path if you want to save your ROOT_GUI generated result.

Docker will not save any change you made if you do not commit it. So this path is used for exchange the files between host and docker containers.
 
``` 
sudo docker run  -v [host data file path]:/home/rootGUI/data   -v /etc/localtime:/etc/localtime:ro -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY -e GDK_SCALE -e GDK_DPI_SCALE -it uvasiyu/root_gui
```

### Option 2(recommend)

get the docker run script:
```
./runDocker.sh
```

## run the ROOT GUI in Docker

```
cd /home/rootGUI/library
source setup.sh

cd /home/rootGUI/ROOT_GUI_multiCrate
./GEMAnalyzer
```


#### [Reference]
* [How to share data between a docker container and host](https://thenewstack.io/docker-basics-how-to-share-data-between-a-docker-container-and-host/)
