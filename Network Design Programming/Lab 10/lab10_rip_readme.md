# Lab No. 10: Configuring Routing Protocols using Packet Tracer (RIP)

## Aim
To configure RIP (Routing Information Protocol) on a 4-router network and verify connectivity between PC0 and PC1 using Packet Tracer.

---

## Apparatus / Software Required
- Cisco Packet Tracer  
- 2 × Cisco 2620XM Routers  
- 1 × Cisco 1841 Router  
- 1 × Cisco 2811 Router  
- 2 × PC-PT  

---

## Theory

Routing Information Protocol (RIP) is a distance-vector interior gateway protocol that uses hop count as its routing metric.  
- Maximum hop count: 15 (16 = unreachable)  
- Periodic updates: every 30 seconds  
- Best suited for small networks  

### RIP Timers
- Update Timer: 30 seconds  
- Invalid Timer: 180 seconds  
- Hold-down Timer: 180 seconds  
- Flush Timer: 240 seconds  

---

## Network Topology

Router0 (2620XM) ↔ Router1 (2620XM) — Ethernet  
Router0 ↔ Router2 (1841) — Serial  
Router1 ↔ Router3 (2811) — Serial  
Router2 ↔ PC0 — Ethernet  
Router3 ↔ PC1 — Ethernet  

---

## IP Address Table

| Device | Interface | IP Address     | Subnet Mask     | Role |
|--------|----------|---------------|-----------------|------|
| PC0    | NIC      | 192.168.1.2   | 255.255.255.0   | End device |
| PC1    | NIC      | 192.168.2.2   | 255.255.255.0   | End device |
| R2     | Fa0/0    | 192.168.1.1   | 255.255.255.0   | PC0 Gateway |
| R2     | Se0/0/0  | 10.0.0.2      | 255.255.255.0   | Link to R0 |
| R0     | Se0/0    | 10.0.0.1      | 255.255.255.0   | Link to R2 |
| R0     | Fa0/0    | 10.0.1.1      | 255.255.255.0   | Link to R1 |
| R1     | Fa0/0    | 10.0.1.2      | 255.255.255.0   | Link to R0 |
| R1     | Se0/0    | 10.0.2.1      | 255.255.255.0   | Link to R3 |
| R3     | Se0/0/0  | 10.0.2.2      | 255.255.255.0   | Link to R1 |
| R3     | Fa0/0    | 192.168.2.1   | 255.255.255.0   | PC1 Gateway |

---

## Procedure

### Step 1: Place Devices
- 2 × 2620XM (Router0, Router1)  
- 1 × 1841 (Router2)  
- 1 × 2811 (Router3)  
- 2 × PCs  

### Step 2: Connect Devices
- R0 Fa0/0 → R1 Fa0/0 (Ethernet)  
- R0 Se0/0 → R2 Se0/0/0 (Serial)  
- R1 Se0/0 → R3 Se0/0/0 (Serial)  
- R2 Fa0/0 → PC0  
- R3 Fa0/0 → PC1  

### Step 3: Configure PCs
Assign IP and default gateway via Desktop → IP Configuration  

---

## Router Configuration

### Router0
```
enable
configure terminal
hostname Router0
interface fastethernet 0/0
ip address 10.0.1.1 255.255.255.0
no shutdown
exit
interface serial 0/0
ip address 10.0.0.1 255.255.255.0
clock rate 64000
no shutdown
exit
router rip
network 10.0.1.0
network 10.0.0.0
end
write memory
```

### Router1
```
enable
configure terminal
hostname Router1
interface fastethernet 0/0
ip address 10.0.1.2 255.255.255.0
no shutdown
exit
interface serial 0/0
ip address 10.0.2.1 255.255.255.0
clock rate 64000
no shutdown
exit
router rip
network 10.0.1.0
network 10.0.2.0
end
write memory
```

### Router2
```
enable
configure terminal
hostname Router2
interface fastethernet 0/0
ip address 192.168.1.1 255.255.255.0
no shutdown
exit
interface serial 0/0/0
ip address 10.0.0.2 255.255.255.0
no shutdown
exit
router rip
network 192.168.1.0
network 10.0.0.0
end
write memory
```

### Router3
```
enable
configure terminal
hostname Router3
interface fastethernet 0/0
ip address 192.168.2.1 255.255.255.0
no shutdown
exit
interface serial 0/0/0
ip address 10.0.2.2 255.255.255.0
no shutdown
exit
router rip
network 192.168.2.0
network 10.0.2.0
end
write memory
```

---

## Verification

### Check Routing Table
```
show ip route
```

Look for:
- R = RIP routes  
- [120/x] = administrative distance / hop count  

### Check RIP Status
```
show ip protocols
```

---

## Ping Test

From PC0:
```
ping 192.168.1.1
ping 192.168.2.2
```

Expected:
- First ping may timeout (ARP)  
- Subsequent replies successful  
- TTL ≈ 124 (indicates multi-hop routing)  

---

## Result

RIP routing protocol was successfully configured.  
All routers learned routes dynamically, and end-to-end communication between PC0 and PC1 was achieved.

---

## Observation

A four-router network was configured using RIP as the dynamic routing protocol.  
Router0 and Router1 formed the backbone via Ethernet.  
Router2 and Router3 connected end devices via FastEthernet and linked to backbone routers via serial links.  
Clock rate was configured on DCE interfaces.  
After convergence, routing tables were updated automatically.  
TTL values confirmed multi-hop traversal, validating RIP operation.

---

## Key Learning
- RIP enables dynamic routing  
- Hop count determines path selection  
- Serial links require clock rate on DCE  
- Routing tables validate connectivity  
