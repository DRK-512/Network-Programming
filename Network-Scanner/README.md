# Newtwork Scanner

## What does this network scanner do? 
The purpose of this project is to create a network scanner that can identify all devices connected to a local network, <br>
then it will display all the devices IPs, MAC addresses, and hostnames

## How to build: 
first we get the required networking tools, then we run make: 
```bash 
sudo apt install libsfml-* libboost-all-dev
make
```

## How to use the portscanner 
-i, --interface    is the network interface you are scanning<br>
-s, --subnet       subnet used to scan the network interface<br>
-h, --help         display this help and exit<br>

## Example Run: 
```bash
./networkscanner -i eth0 -s 192.168.2.0
```