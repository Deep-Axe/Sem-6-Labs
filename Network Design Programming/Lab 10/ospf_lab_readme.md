# Lab No. 10 (b): Configuring OSPF Routing Protocol using Packet Tracer

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

OSPF is a link-state routing protocol that uses Dijkstra’s SPF algorithm.  
- Metric: Cost (based on bandwidth)  
- Fast convergence  
- Uses LSDB (Link State Database)  

### Key Differences (RIP vs OSPF)

| Feature | RIP | OSPF |
|--------|-----|------|
| Type | Distance Vector | Link State |
| Metric | Hop Count | Cost |
| Convergence | Slow | Fast |
| Updates | Periodic | Event-driven |
| Scale | Small | Large |

---

## Topology
Same as RIP lab:
- Router0 ↔ Router1 (Ethernet)  
- Router0 ↔ Router2 (Serial)  
- Router1 ↔ Router3 (Serial)  
- Router2 ↔ PC0  
- Router3 ↔ PC1  

---

## IP Addressing
Same as RIP lab (no changes)

---

## Procedure

### Step 1: Remove RIP (if configured)
```
no router rip
```

### Step 2: Configure OSPF

---

## Router Configurations

### Router0
```
router ospf 1
network 10.0.1.0 0.0.0.255 area 0
network 10.0.0.0 0.0.0.255 area 0
```

### Router1
```
router ospf 1
network 10.0.1.0 0.0.0.255 area 0
network 10.0.2.0 0.0.0.255 area 0
```

### Router2
```
router ospf 1
network 192.168.1.0 0.0.0.255 area 0
network 10.0.0.0 0.0.0.255 area 0
```

### Router3
```
router ospf 1
network 192.168.2.0 0.0.0.255 area 0
network 10.0.2.0 0.0.0.255 area 0
```

---

## Verification

### Check Neighbors
```
show ip ospf neighbor
```
- FULL state = successful adjacency  

### Check Routing Table
```
show ip route
```
- O = OSPF routes  
- AD = 110  

---

## Ping Test

From PC0:
```
ping 192.168.2.2
```

Expected:
- First packet timeout (ARP)  
- Successful replies after  

---

## Result

OSPF was successfully configured.  
All routers formed FULL adjacency and exchanged routes.  
End-to-end connectivity was verified.

---

## Observation

OSPF replaced RIP on the same topology.  
Routers formed adjacencies and synchronized LSDB.  
Routing tables showed OSPF routes with cost-based metrics.  
OSPF converged faster and provided better scalability.

---

## Key Learning
- OSPF uses link-state routing  
- Faster convergence than RIP  
- Uses cost instead of hop count  
- Scales well for large networks  
