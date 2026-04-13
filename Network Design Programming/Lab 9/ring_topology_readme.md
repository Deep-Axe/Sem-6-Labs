# Lab: Ring Topology Implementation

## Concept
Each device connects to exactly two other devices, forming a closed loop.  
Data travels in one direction around the ring until it reaches the destination.

---

## Topology

Standard 4-node ring:

Laptop0        Laptop1  
   |               |  
Switch0 ──── Switch1  
   |               |  
Switch3 ──── Switch2  
   |               |  
Laptop3        Laptop2  

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
- 4 × Laptop-PT (or PC-PT)  
- 4 × 2950/2960 Switches  
- Copper crossover cables (switch-to-switch)  
- Copper straight-through cables (laptop-to-switch)  

---

## Step-by-Step Setup

### Step 1: Place Devices
- Arrange 4 switches in a square (ring)
- Place one laptop near each switch

### Step 2: Connect Switches (Ring Formation)
- Switch0 → Switch1 (Fa0/1 to Fa0/1)
- Switch1 → Switch2 (Fa0/2 to Fa0/1)
- Switch2 → Switch3 (Fa0/2 to Fa0/1)
- Switch3 → Switch0 (Fa0/2 to Fa0/2) ← closes the ring

### Step 3: Connect Laptops
- Laptop0 → Switch0 (Fa0/3)
- Laptop1 → Switch1 (Fa0/3)
- Laptop2 → Switch2 (Fa0/3)
- Laptop3 → Switch3 (Fa0/3)

### Step 4: Configure IP Addresses
On each laptop:
Desktop → IP Configuration → Set IP as per table

---

## Important: STP Behavior

- When the ring is closed, some switch ports turn **orange**
- This is **Spanning Tree Protocol (STP)** blocking one link
- Prevents broadcast loops
- Wait 30–50 seconds for convergence

STP converts the ring into a loop-free topology while keeping redundancy.

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

### Adjacent Node (Laptop0 → Laptop1)
- 0% packet loss  
- time <1ms  

### Opposite Node (Laptop0 → Laptop2)
- First ping timeout (ARP)  
- Remaining replies successful (~1ms)  

---

## Fault Tolerance Test

1. Confirm normal connectivity  
2. Delete one switch-to-switch link  
3. Wait 30–50 seconds (STP reconvergence)  
4. Ping again  

Expected:
- Communication still works  
- Slightly higher latency (~2ms)  

This demonstrates self-healing behavior of ring topology.

---

## Simulation Mode

- Switch to Simulation Mode  
- Add Simple PDU (Laptop0 → Laptop2)  
- Play simulation  

Observe packet traveling around the ring.

---

## Observation

Ring topology was implemented using 4 laptops and 4 switches connected in a closed loop.  
All devices were configured in the same subnet.  
STP blocked one redundant link to prevent loops.  
Ping tests confirmed connectivity across all nodes.  
After link failure, STP reconverged and traffic rerouted through the alternate path, demonstrating fault tolerance.  
Increased latency confirmed longer path traversal.

---

## Key Learning
- Ring topology provides redundancy  
- STP prevents loops in switched networks  
- Network can recover from link failures automatically  
