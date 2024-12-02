#!/bin/bash

# Flags
FLAG_H=false
FLAG_L=false
FLAG_P=false

# Parse command line arguments
while getopts ":hlp" opt; do
  case $opt in
    h)
      FLAG_H=true
      ;;
    l)
      FLAG_L=true
      ;;
    p)
      FLAG_P=true
      ;;
    \?)
      echo "Illegal operation" >&2
      exit 1
      ;;
  esac
done

# Check for specific flags
if $FLAG_H || $FLAG_L || $FLAG_P; then
  echo "Flag(s) $(echo -n "$FLAG_H" | sed 's/true/h/')$(echo -n "$FLAG_L" | sed 's/true/l/')$(echo -n "$FLAG_P" | sed 's/true/p/') is used"
  exit 1
fi

# No errors, exit with code 0
exit 0