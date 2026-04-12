# Lab: Network with Router, 2 Switches, and 4 PCs (Inter-Subnet Routing)

## Topology
PC0 ─┐                    ┌─ PC2  
PC1 ─┴─ Switch0 ── Router ── Switch1 ─┴─ PC3  

- Switch0 → PC0, PC1 → Router Fa0/0  
- Switch1 → PC2, PC3 → Router Fa0/1  

---

## IP Address Table

| Device | Interface | IP Address     | Subnet Mask     | Default Gateway |
|--------|----------|---------------|-----------------|-----------------|
| PC0    | NIC      | 192.168.1.2   | 255.255.255.0   | 192.168.1.1     |
| PC1    | NIC      | 192.168.1.3   | 255.255.255.0   | 192.168.1.1     |
| PC2    | NIC      | 192.168.2.2   | 255.255.255.0   | 192.168.2.1     |
| PC3    | NIC      | 192.168.2.3   | 255.255.255.0   | 192.168.2.1     |
| Router | Fa0/0    | 192.168.1.1   | 255.255.255.0   | —               |
| Router | Fa0/1    | 192.168.2.1   | 255.255.255.0   | —               |

---

## Equipment Required
- 1 × 1841 Router  
- 2 × 2960 Switches  
- 4 × PC-PT  
- Straight-through cables  

---

## Step-by-Step Setup

### Step 1: Place Devices
- Add 1 Router, 2 Switches, 4 PCs

### Step 2: Connect Devices
- PC0 → Switch0 (Fa0/1)
- PC1 → Switch0 (Fa0/2)
- PC2 → Switch1 (Fa0/1)
- PC3 → Switch1 (Fa0/2)
- Switch0 → Router (Fa0/0)
- Switch1 → Router (Fa0/1)

### Step 3: Configure PCs
Set IP and Default Gateway as per table

---

## Router Configuration

```
enable
configure terminal
hostname R1

interface fastethernet 0/0
ip address 192.168.1.1 255.255.255.0
no shutdown
exit

interface fastethernet 0/1
ip address 192.168.2.1 255.255.255.0
no shutdown
exit

end
write memory
```

---

## Verification

### Router Commands
```
show ip interface brief
show ip route
```

Expected:
- Both interfaces up/up
- Two directly connected routes

---

## Ping Tests

From PC0:
```
ping 192.168.1.1
ping 192.168.2.1
ping 192.168.2.2
ping 192.168.2.3
```

---

## Expected Output

### Same Subnet
- 0% loss
- TTL = 128

### Cross Subnet
- First ping timeout (ARP)
- TTL = 127 (router hop)

---

## Observation

A network was implemented using one router, two switches, and four PCs across two subnets.  
Router interfaces were configured and enabled.  
Same subnet communication had TTL=128, cross-subnet had TTL=127 confirming routing.  
All PCs communicated successfully.

---

## Key Learning
- Routers connect different networks  
- Default gateway is mandatory for inter-subnet communication  
- TTL decrement confirms routing  
