#!/bin/bash

Target=$1
Destination=$2
while inotifywait -r -e modify,create,delete,move ${Target}; do
    rsync -avP --ignore-existing ${Target} ${Destination}
done
