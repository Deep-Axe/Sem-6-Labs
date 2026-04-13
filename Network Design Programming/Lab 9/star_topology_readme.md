# Lab: Star Topology Implementation

## Concept
All devices connect to a single central switch.  
The switch is the hub of the star — if it fails, the entire network goes down, but any single device failure does not affect others.

---

## Topology

        Laptop0
           |
Laptop3 ─ Switch ─ Laptop1
           |
        Laptop2
           |
     Laptop4 / Laptop5

---

## IP Address Table

| Device   | IP Address     | Subnet Mask     | Gateway |
|----------|---------------|-----------------|---------|
| Laptop0  | 192.168.1.1   | 255.255.255.0   | —       |
| Laptop1  | 192.168.1.2   | 255.255.255.0   | —       |
| Laptop2  | 192.168.1.3   | 255.255.255.0   | —       |
| Laptop3  | 192.168.1.4   | 255.255.255.0   | —       |
| Laptop4  | 192.168.1.5   | 255.255.255.0   | —       |
| Laptop5  | 192.168.1.6   | 255.255.255.0   | —       |

> No gateway required (single subnet)

---

## Equipment Required
- 1 × Switch (PT-Switch or 2960)  
- 6 × Laptop-PT  
- Copper straight-through cables  

---

## Step-by-Step Setup

### Step 1: Place Devices
- Place 1 switch at center  
- Arrange 6 laptops around it  

### Step 2: Expand Ports (if needed)
- Go to Physical tab  
- Power off switch  
- Add PT-SWITCH-NM-1CFE modules  
- Power on  

### Step 3: Connect Devices
- Laptop0 → Switch Fa0/1  
- Laptop1 → Switch Fa0/2  
- Laptop2 → Switch Fa0/3  
- Laptop3 → Switch Fa0/4  
- Laptop4 → Switch Fa0/5  
- Laptop5 → Switch Fa0/6  

(All connections use straight-through cables)

### Step 4: Configure IP Addresses
Desktop → IP Configuration → Set IPs as per table

---

## Verification

From Laptop0:
```
ping 192.168.1.2
ping 192.168.1.3
ping 192.168.1.4
ping 192.168.1.5
ping 192.168.1.6
```

---

## Expected Output

### Any Laptop Pair
- 0% packet loss  
- time <1ms  
- TTL = 128  

### First Ping
- May timeout due to ARP  

---

## Key Characteristic

All communication passes through **one central switch hop**,  
so latency is identical regardless of destination.

---

## Central Switch Failure Test

1. Confirm connectivity  
2. Remove switch or disconnect cables  
3. Ping again  

Expected:
- 100% packet loss  
- Entire network fails  

---

## Observation

Star topology was implemented using one central switch and six laptops.  
All devices were configured in the same subnet.  
Ping tests confirmed consistent low latency across all nodes.  
All communication passed through a single switch hop.  
When the central switch was removed, the entire network failed, confirming it as a single point of failure.

---

## Key Learning
- Star topology is simple and efficient  
- Centralized control via switch  
- Single point of failure at the switch  
