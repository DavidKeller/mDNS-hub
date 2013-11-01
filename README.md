# Description
Dead simple C++03 mDNS daemon that forwards mDNS traffic accross subnets.

It allows devices living on different subnets to see each others.

# Usage
Start the application with each subnet interface you want to share mDNS traffic.

    $mdnshubd eth1 eth2 .. ethX

You can use the -f argument if you want application to stay in the foreground:

    $mdnshubd -f eth1 eth2 .. ethX

