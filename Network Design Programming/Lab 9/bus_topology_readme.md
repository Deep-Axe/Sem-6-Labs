# Lab: Bus Topology Implementation

## Topology
Laptop0  Laptop1    Laptop2  Laptop3  
   |        |          |        |  
[Sw0]──[Sw1]──[Sw2]──[Sw3]  
        (backbone bus)

- One laptop per switch  
- Switches connected in a linear chain (bus backbone)

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
- 4 × Laptop-PT  
- 4 × Switches (2960 or generic)  
- Copper straight-through cables  
- Copper crossover cables  

---

## Step-by-Step Setup

### Step 1: Place Devices
- Add 4 laptops and 4 switches

### Step 2: Connect Switches (Backbone)
- Switch0 → Switch1 (Fa0/2 to Fa0/1)
- Switch1 → Switch2 (Fa0/2 to Fa0/1)
- Switch2 → Switch3 (Fa0/2 to Fa0/1)

### Step 3: Connect Laptops
- Laptop0 → Switch0 (Fa0/1)
- Laptop1 → Switch1 (Fa0/2)
- Laptop2 → Switch2 (Fa0/2)
- Laptop3 → Switch3 (Fa0/2)

### Step 4: Configure IP Addresses
On each laptop:
Desktop → IP Configuration → Set IP as per table

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

### Laptop0 → Laptop1
- 0% packet loss  
- time <1ms  

### Laptop0 → Laptop3
- First ping timeout (ARP)  
- Remaining replies successful (~1ms)  

---

## Packet Tracer Simulation

- Use **Add Simple PDU (envelope icon)**  
- Source: Laptop0  
- Destination: Laptop3  
- Switch to Simulation Mode  

This shows packet traversal through each switch along the backbone.

---

## Observation

Bus topology was implemented using 4 laptops and 4 switches connected in a linear backbone.  
All devices were configured in the same subnet (192.168.1.0/24).  
Ping tests confirmed connectivity across the network.  
Simulation mode demonstrated that packets travel sequentially through each switch, representing bus behavior.  
Initial packet loss was due to ARP resolution, which is expected.

---

## Key Learning
- Bus topology uses a single shared backbone  
- Data travels sequentially across nodes  
- ARP causes initial delay in communication  
