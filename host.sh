#!/bin/sh

host=ant.apmath.spbu.ru
name=Arnoochka

adguardvpn-cli disconnect

if [ $# -gt 0 ]; then
    mpiDir=$name@$host:/home/$name

    if [ "$1" == "send" ]; then

        if [ "$2" == "all" ]; then
            rsync -av --include="?/" --include="??/" --include="Matrix" --include="Vector" --exclude="??*/" --exclude="*" "." "$mpiDir"
        else
            scp -r "$2" "$mpiDir"
        fi

    elif [ "$1" == "recv" ]; then

        if [ "$2" == "all" ]; then
            rsync -av --include="*/" --include="results*.log" --exclude="*" "$mpiDir" "ssh"
        else

            mkdir "ssh/$2"
            scp "$mpiDir/$2/results*.log" "ssh/$2/"
        fi

    else 
        echo "uncnown command"
    fi
else
    ssh "$name@$host"
fi