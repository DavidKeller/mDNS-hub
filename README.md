# Description
Dead simple C++03 mDNS daemon that forwards mDNS traffic accross subnets.

It allows devices living on different subnets to see each others.

# Usage
Start the application with each subnet interface you want to share mDNS traffic.

    $mDNS-hub eth1 eth2 .. ethX

You can use the -d argument if you want application to daemonize in the background:

    $mDNS-hub -d eth1 eth2 .. ethX

