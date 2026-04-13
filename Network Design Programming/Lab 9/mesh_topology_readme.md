# Lab: Mesh Topology Implementation

## Concept
Every device has a dedicated point-to-point link to every other device.  
Provides maximum redundancy — if any link fails, alternate paths exist.

Number of links = n(n-1)/2  
For 4 switches: 4×3/2 = 6 links

---

## Topology

Laptop0              Laptop1  
   |                    |  
Switch0 ────────── Switch1  
   | ╲              ╱ |  
   |   ╲          ╱   |  
   |     ╲      ╱     |  
   |       ╲  ╱       |  
Switch3 ────────── Switch2  
   |                    |  
Laptop3              Laptop2  

---

## IP Address Table

| Device   | IP Address     | Subnet Mask     | Gateway |
|----------|---------------|-----------------|---------|
| Laptop0  | 192.168.1.1   | 255.255.255.0   | —       |
| Laptop1  | 192.168.1.2   | 255.255.255.0   | —       |
| Laptop2  | 192.168.1.3   | 255.255.255.0   | —       |
| Laptop3  | 192.168.1.4   | 255.255.255.0   | —       |

> No gateway required (single subnet)

---

## Equipment Required
- 4 × Switches (2950/2960)  
- 4 × Laptop-PT  
- 6 × Copper crossover cables  
- 4 × Copper straight-through cables  

---

## Step-by-Step Setup

### Step 1: Place Devices
- Arrange 4 switches in a square  
- Place one laptop per switch  

### Step 2: Connect Switches (Full Mesh)
- Switch0 → Switch1 (Fa0/1 to Fa0/1)  
- Switch0 → Switch2 (Fa0/2 to Fa0/1)  
- Switch0 → Switch3 (Fa0/3 to Fa0/1)  
- Switch1 → Switch2 (Fa0/2 to Fa0/2)  
- Switch1 → Switch3 (Fa0/3 to Fa0/2)  
- Switch2 → Switch3 (Fa0/3 to Fa0/3)  

### Step 3: Connect Laptops
- Laptop0 → Switch0 (Fa0/4)  
- Laptop1 → Switch1 (Fa0/4)  
- Laptop2 → Switch2 (Fa0/4)  
- Laptop3 → Switch3 (Fa0/4)  

### Step 4: Configure IP Addresses
Desktop → IP Configuration → Set IPs as per table  

### Step 5: Wait for STP
- Ports may show orange initially  
- Wait ~30–50 seconds for convergence  

---

## STP Behavior

Spanning Tree Protocol blocks redundant links to prevent loops.  
Blocked links automatically activate if active links fail.

---

## Verification

From Laptop0:
```
ping 192.168.1.2
ping 192.168.1.3
ping 192.168.1.4
```

---

## Expected Output

- 0% packet loss  
- time <1ms  
- TTL = 128  

---

## Fault Tolerance Test

1. Use `show spanning-tree` to identify active links  
2. Remove a direct link (e.g., Switch0–Switch1)  
3. Ping again  

Expected:
- Communication still works  
- Slightly higher latency (~2ms)  

4. Remove another link → network still operational  

---

## Observation

Mesh topology was implemented using 4 switches and 4 laptops, with every switch connected to every other switch using 6 links.  
All devices were configured in the same subnet.  
STP blocked redundant links initially.  
Ping tests confirmed full connectivity.  
When links were removed, traffic automatically rerouted through alternate paths with no packet loss, demonstrating high redundancy.  
The main trade-off is increased cabling complexity and cost.

---

## Key Learning
- Mesh topology provides maximum redundancy  
- Multiple paths ensure reliability  
- STP prevents loops while preserving backup links  
