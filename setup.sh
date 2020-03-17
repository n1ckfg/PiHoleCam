#!/bin/bash

SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"
  SOURCE="$(readlink "$SOURCE")"
  [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE" )" && pwd )"

cd $DIR

cd bin/data

wget https://fox-gieg.com/stuff/rpi/Comp_8_360p.mp4
wget https://fox-gieg.com/stuff/rpi/Comp_8_540p.mp4
wget https://fox-gieg.com/stuff/rpi/Comp_8_1080p.mp4