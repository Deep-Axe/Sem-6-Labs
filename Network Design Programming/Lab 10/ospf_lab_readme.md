# Lab No. 10(b): Configuring OSPF Routing Protocol using Packet Tracer

## Aim

To configure OSPF (Open Shortest Path First) on a 4-router network and verify connectivity between PC0 and PC1.

---

## Apparatus / Software Required

- Cisco Packet Tracer
- 2 × Cisco 2620XM Routers
- 1 × Cisco 1841 Router
- 1 × Cisco 2811 Router
- 2 × PC-PT

---

## Theory

OSPF is a link-state routing protocol that uses Dijkstra's Shortest Path First (SPF) algorithm to calculate the best path to each destination.

- Metric: Cost (based on bandwidth — higher bandwidth = lower cost)
- Fast convergence — only sends updates when topology changes, not periodically
- Uses LSDB (Link State Database) — every router maintains an identical map of the network
- Administrative Distance: 110

### Key Differences (RIP vs OSPF)

| Feature     | RIP             | OSPF           |
| ----------- | --------------- | -------------- |
| Type        | Distance Vector | Link State     |
| Metric      | Hop Count       | Cost           |
| Convergence | Slow            | Fast           |
| Updates     | Periodic (30s)  | Event-driven   |
| Max Hops    | 15              | Unlimited      |
| Scale       | Small networks  | Large networks |
| AD          | 120             | 110            |

---

## Network Topology

```
  PC0 ── R2 (1841) ──── R0 (2620XM) ──── R1 (2620XM) ──── R3 (2811) ── PC1
               Serial 0/0/0     Se0/0   Fa0/0   Fa0/0    Se0/0   Se0/0/0
                         10.0.0.x       10.0.1.x          10.0.2.x
```

- Router0 ↔ Router1 — FastEthernet (10.0.1.0/24)
- Router0 ↔ Router2 — Serial (10.0.0.0/24)
- Router1 ↔ Router3 — Serial (10.0.2.0/24)
- Router2 ↔ PC0 — FastEthernet (192.168.1.0/24)
- Router3 ↔ PC1 — FastEthernet (192.168.2.0/24)

---

## IP Address Table

| Device | Interface | IP Address  | Subnet Mask   | Role        |
| ------ | --------- | ----------- | ------------- | ----------- |
| PC0    | NIC       | 192.168.1.2 | 255.255.255.0 | End device  |
| PC1    | NIC       | 192.168.2.2 | 255.255.255.0 | End device  |
| R2     | Fa0/0     | 192.168.1.1 | 255.255.255.0 | PC0 Gateway |
| R2     | Se0/0/0   | 10.0.0.2    | 255.255.255.0 | Link to R0  |
| R0     | Se0/0     | 10.0.0.1    | 255.255.255.0 | Link to R2  |
| R0     | Fa0/0     | 10.0.1.1    | 255.255.255.0 | Link to R1  |
| R1     | Fa0/0     | 10.0.1.2    | 255.255.255.0 | Link to R0  |
| R1     | Se0/0     | 10.0.2.1    | 255.255.255.0 | Link to R3  |
| R3     | Se0/0/0   | 10.0.2.2    | 255.255.255.0 | Link to R1  |
| R3     | Fa0/0     | 192.168.2.1 | 255.255.255.0 | PC1 Gateway |

> **Note on serial interface naming:**
>
> - R0, R1 (2620XM) use `serial 0/0`
> - R2 (1841) and R3 (2811) use `serial 0/0/0`
>   This is a hardware difference between models, not a typo.

---

## Procedure

### Step 1: Place Devices

- 2 × 2620XM (Router0, Router1)
- 1 × 1841 (Router2)
- 1 × 2811 (Router3)
- 2 × PC-PT

### Step 2: Connect Devices

| From | Port  | To  | Port    | Cable            |
| ---- | ----- | --- | ------- | ---------------- |
| R0   | Fa0/0 | R1  | Fa0/0   | Straight-Through |
| R0   | Se0/0 | R2  | Se0/0/0 | Serial (DCE→R0)  |
| R1   | Se0/0 | R3  | Se0/0/0 | Serial (DCE→R1)  |
| R2   | Fa0/0 | PC0 | NIC     | Straight-Through |
| R3   | Fa0/0 | PC1 | NIC     | Straight-Through |

### Step 3: Configure PCs

**PC0** — Desktop → IP Configuration:

- IP Address: `192.168.1.2`
- Subnet Mask: `255.255.255.0`
- Default Gateway: `192.168.1.1`

**PC1** — Desktop → IP Configuration:

- IP Address: `192.168.2.2`
- Subnet Mask: `255.255.255.0`
- Default Gateway: `192.168.2.1`

### Step 4: Remove RIP (only if continuing from RIP lab)

Run this on each router before configuring OSPF:

```
enable
configure terminal
no router rip
exit
```

---

## Router Configurations

> These are complete standalone configs — interface IPs + OSPF.
> If continuing from the RIP lab, skip the interface sections and only run the `router ospf` blocks.

### Router0 (Cisco 2620XM)

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

router ospf 1
network 10.0.1.0 0.0.0.255 area 0
network 10.0.0.0 0.0.0.255 area 0
exit

end
write memory
```

### Router1 (Cisco 2620XM)

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

router ospf 1
network 10.0.1.0 0.0.0.255 area 0
network 10.0.2.0 0.0.0.255 area 0
exit

end
write memory
```

### Router2 (Cisco 1841)

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

router ospf 1
network 192.168.1.0 0.0.0.255 area 0
network 10.0.0.0 0.0.0.255 area 0
exit

end
write memory
```

### Router3 (Cisco 2811)

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

router ospf 1
network 192.168.2.0 0.0.0.255 area 0
network 10.0.2.0 0.0.0.255 area 0
exit

end
write memory
```

---

## Verification Commands

### 1. Check OSPF neighbors

```
show ip ospf neighbor
```

Look for **FULL** state — this means the routers have successfully formed adjacency and exchanged their LSDBs.

### 2. Check routing table

```
show ip route
```

Look for:

- `O` = OSPF learned route
- `[110/x]` = administrative distance 110 / cost metric

### 3. Check OSPF database (LSDB)

```
show ip ospf database
```

Confirms all routers are sharing Link State Advertisements (LSAs). All routers in the same area should have identical databases.

### 4. Check OSPF process details

```
show ip protocols
```

Shows OSPF process ID, router ID, and which networks are being advertised.

### 5. Check DCE/DTE role on serial links

```
show controllers serial 0/0
```

Only the DCE end needs `clock rate`. If a router is DTE, remove the `clock rate` line from its serial config.

---

## Ping Test

From PC0 command prompt:

```
ping 192.168.1.1
ping 192.168.2.2
```

Expected output:

- First packet may timeout (ARP resolution)
- Subsequent packets reply successfully
- TTL ≈ 124 (confirms multi-hop traversal)

---

## Result

OSPF was successfully configured on all four routers.
All routers formed FULL adjacency with their neighbors and exchanged link-state information.
End-to-end connectivity between PC0 (192.168.1.2) and PC1 (192.168.2.2) was verified successfully.

---

## Observation

OSPF was configured using process ID 1 with all interfaces placed in Area 0 (backbone area).
Routers formed adjacencies over both FastEthernet and Serial links and synchronized their Link State Databases.
The `show ip ospf neighbor` command confirmed FULL state on all neighbor pairs.
Routing tables showed OSPF routes marked with `O` and administrative distance of 110.
Unlike RIP, OSPF converged immediately after configuration without waiting for periodic update timers.
Cost-based metrics were used instead of hop count, making OSPF more suitable for larger, bandwidth-varied networks.

---

## Key Learning

- OSPF uses link-state routing — each router knows the full network topology
- Cost metric favours high-bandwidth links over low-bandwidth ones
- All routers in the same area must have identical LSDBs
- OSPF converges faster than RIP as updates are event-driven, not periodic
- `clock rate` is only needed on the DCE end of serial links
- Administrative distance of 110 means OSPF is preferred over RIP (AD 120) if both are configured
