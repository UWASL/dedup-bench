#!/bin/bash
 
# A script to install project dependencies on a server running a Debian-based operating system (such as Ubuntu or Debian).
# Dependencies List:
# g++ 
# libssl-dev

# Update package list
sudo apt update

# install g++
sudo apt install g++
if [ $? -eq 0 ]; then
  echo -e "\e[32mg++ installation successful\e[0m"
else
  echo -e "\e[31mg++ installation failed\e[0m"
fi

# install libssl-dev
sudo apt-get install libssl-dev
if [ $? -eq 0 ]; then
  echo -e "\e[32mlibssl-dev installation successful\e[0m"
else
  echo -e "\e[31mlibssl-dev installation failed\e[0m"
fi
