# Description
Dead simple C++03 mDNS daemon that forwards mDNS traffic accross subnets barriers.

It allows Bonjour enabled devices living on different subnets to see each others.
E.g. your printer is on your subnet 172.18.0.0/24 and you want your
wifi hosts on subet 172.18.1.0/24 to see it, then you may want
to run mdnshubd on your router.

# Usage
Start the application with each subnet interface you want to share mDNS traffic.

    $mdnshubd eth1 eth2 .. ethX

You can use the -f argument if you want the application to stay in the foreground:

    $mdnshubd -f eth1 eth2 .. ethX

# Development
Feel free to enhance code or add rc.d scripts for your OS.

## Project structure
* data/ Contains OS specific files like rc.d/ init scripts.
* src/ Contains daemon sources.
* build\_scripts/ Contains cmake build scripts.
