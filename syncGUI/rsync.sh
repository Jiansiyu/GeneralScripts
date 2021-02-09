#!/bin/bash

Target=$1
Destination=$2
while inotifywait -r -e modify,create,delete,move ${Target}
do
    echo "file changed start sync!"
    echo ${Target}
    rsync -avP --ignore-existing ${Target} ${Destination}
done
