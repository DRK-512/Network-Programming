# Port Scanner

## What is a Port Scanner
A port scanner is a program which probes a server for open ports by attempting a connection to the server via each port. The program then reports which ports were open and which were closed. More complex port scanners, such as Nmap, can probe for other information as well. Port scanning is usually performed by system administrators to verify network security or by attackers looking for an open port through which to compromise the security of a server. Open ports may constitute security vulnerabilities as they allow remote machines to connect. Unexpectedly-open ports may indicate malicious software listening for instructions.

## How to build: 
first we get the required networking tools, then we run make: 
```
sudo apt-get install libsfml-*
make
```

## How to use the portscanner 
To run the script, simply run ./portscanner with the following parameters in mind <br>
&nbsp; -i, --ip-address   input the IP addresses you are looking for<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp;  Examples:<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; -i 127.0.0.1 will search IP 127.0.0.1<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; -i localhost will search the IP 127.0.0.1<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; default IP is localhost<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ./portscanner -i 192.168.2.100 # will scan all ports on 192.168.2.100<br>
    
&nbsp; -p, --port       input the ports you are scanning<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; Examples: <br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; -p 80 will scan port 80<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; -p 0-65535 will scan ports 0-65535<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; default ports to scan are 0-65535<br>
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; ./portscanner -p 20-200 # will scan ports 20-200 <br>

&nbsp; -s, --show       display ports that are closed as well as open<br>

&nbsp; -h, --help       display this help and exit<br>

## Example Run: 
```
$./portscanner -i 192.168.2.100 -p 0-2048 -s
```
