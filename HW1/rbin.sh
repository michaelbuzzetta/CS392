#!/bin/bash

################################################################################
# Author  : Michael Buzzetta
# Date    : Feb 4, 2023
# Description: CS392 - Homework 1
# Pledge : I pledge my honor that I have abided by the stevns honor system
################################################################################
#Recieved help from CA Eddison So and Marcos Traverso during office hours
#referenced code and concepts from websites provided in assignment PDF and textbook

# TODO: Fill the header above, and then complete rbin.sh below

#Destination: recycle bin
readonly dest=$HOME/.recycle

#Declare the flags
hFlag=0
lFlag=0
pFlag=0

function correctUsage()
{
    cat << EOF
Usage: rbin.sh [-hlp] [list of files]
   -h: Display this help;
   -l: List files in the recycle bin;
   -p: Empty all files in the recycle bin;
   [list of files] with no other flags,
        these files will be moved to the
        recycle bin.
EOF
}

while getopts ":hlp" option; do
    case "$option" in
    h)
        hFlag=$(( hFlag+=1 ))
        ;;
    l)
        lFlag=$(( lFlag+=1 ))
        ;;
    p)
        pFlag=$(( pFlag+=1 ))
        ;;
    *)
        echo "Error: Unknown option" "'-"$OPTARG"'." >&2
        correctUsage
        exit 1
        ;;
    esac
done

shift "$(($OPTIND -1))"

#tracks number of valid flags
countFlags=$(( hFlag+ lFlag + pFlag ))

#tracks number of attempted inputed files
temp=0
for files in "$@"; do
    temp=$(( temp + 1))
done

#If there is more than one valid flag do:
if [ $countFlags -gt 1 ]; 
    then
        echo 'Error: Too many options enabled.' >&2
        correctUsage
        exit 1

#If there are one or more flags and files do:
elif ([ $countFlags -gt 0 ] && [ $temp -gt 0 ]); 
    then
        echo 'Error: Too many options enabled.' >&2
        correctUsage
        exit 1
fi

#If the user inputs h flag or just ./rbin.sh
if [ $hFlag = 1 ]; 
    then correctUsage
elif [[ $countFlags = 0 && $temp = 0 ]]; 
    then correctUsage
fi

#Checks if recycle path exists, if not, it creates and initializes it
if [[ -d "$dest" ]]; 
    then true
else
    mkdir "$dest"
fi

#If the user gives the -l flag: list all files in recycle
if [ $lFlag = 1 ]; 
    then ls -lAF "$dest"
fi

#If the user gives the -p flag: delete all files in recycle
if [ $pFlag = 1 ]; 
    then
        shopt -s dotglob
        rm -rf "$dest"/*
fi

#Move requested files in and out of recycle
for files in "$@"; do
    if [[ -f "$files" || -d "$files" ]]; then
        mv "$files" "$dest"
    else
        echo "Warning: '$files' not found." >&2
        exit 1
    fi
done

exit 0
