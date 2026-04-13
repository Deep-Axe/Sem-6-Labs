# Lab No. 11: Configuring DHCP on a Multi-Router Network using Packet Tracer

## Aim
To configure DHCP on routers so that PCs automatically receive IP addresses, and verify connectivity between all PCs across the network.

---

## Apparatus / Software Required
- Cisco Packet Tracer  
- 3 × Cisco 2621XM Routers (R1, R2, R3)  
- 2 × Cisco 2950-24 Switches (Switch0, Switch1)  
- 4 × PC-PT (PC0, PC1, PC2, PC3)  

---

## Theory

DHCP (Dynamic Host Configuration Protocol) automatically assigns IP addresses to client devices.

### DHCP Process
1. DHCPDISCOVER — Client broadcasts request  
2. DHCPOFFER — Server offers IP  
3. DHCPREQUEST — Client requests IP  
4. DHCPACK — Server confirms lease  

---

## Network Topology

        [R2] ----------- [R1] ----------- [R3]
       10.10.10.1      10.10.20.1
          |                 |
      [Switch0]        [Switch1]
       /     \          /          PC0     PC1      PC2     PC3

---

## Router Configuration (Summary)

### R1
```
interface serial 0/2
ip address 10.10.10.2 255.255.255.252
clock rate 64000

interface serial 0/3
ip address 10.10.20.1 255.255.255.252
clock rate 64000

router rip
network 10.10.10.0
network 10.10.20.0
```

### R2 DHCP
```
ip dhcp excluded-address 192.168.10.1 192.168.10.20
ip dhcp pool LEFTNET
network 192.168.10.0 255.255.255.0
default-router 192.168.10.1
dns-server 8.8.8.8
```

### R3 DHCP
```
ip dhcp excluded-address 192.168.1.1 192.168.1.20
ip dhcp pool RIGHTNET
network 192.168.1.0 255.255.255.0
default-router 192.168.1.1
dns-server 8.8.8.8
```

---

## Verification

```
show ip dhcp binding
show ip route
ping 192.168.1.21
```

---

## Result

DHCP successfully assigns IPs and routing works across subnets.

---

## Key Learning
- DHCP automates IP assignment  
- Routers act as DHCP servers  
- RIP enables connectivity  
