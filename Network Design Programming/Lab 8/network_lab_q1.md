# Lab: Network Setup with 4 Switches and 8 PCs

## Topology
Connect 2 PCs to each switch, then daisy-chain all 4 switches together in a line:

PC0 ─┐                          ┌─ PC6  
PC1 ─┤Switch0 ── Switch1 ── Switch2 ── Switch3├─ PC7  
PC2 ─┘    |          |                    |  
PC3 ──────┘     PC4 ─┘                PC5 ─┘  

More clearly:
- Switch0 → PC0, PC1  
- Switch1 → PC2, PC3  
- Switch2 → PC4, PC5  
- Switch3 → PC6, PC7  
- Switch0 ↔ Switch1 ↔ Switch2 ↔ Switch3 (trunk line)

---

## IP Address Table

| Device | IP Address     | Subnet Mask     | Default Gateway |
|--------|---------------|-----------------|-----------------|
| PC0    | 192.168.1.1   | 255.255.255.0   | —               |
| PC1    | 192.168.1.2   | 255.255.255.0   | —               |
| PC2    | 192.168.1.3   | 255.255.255.0   | —               |
| PC3    | 192.168.1.4   | 255.255.255.0   | —               |
| PC4    | 192.168.1.5   | 255.255.255.0   | —               |
| PC5    | 192.168.1.6   | 255.255.255.0   | —               |
| PC6    | 192.168.1.7   | 255.255.255.0   | —               |
| PC7    | 192.168.1.8   | 255.255.255.0   | —               |

> No gateway needed — all PCs are on the same subnet.

---

## Equipment Required
- 4 × 2960 Switches (or any switch in Packet Tracer)
- 8 × PC-PT
- Straight-through cables (PC to switch)
- Crossover cables (switch to switch) *(Auto works in Packet Tracer)*

---

## Step-by-Step Setup in Packet Tracer

### Step 1: Place Devices
- Drag 4 switches onto the workspace
- Drag 8 PCs onto the workspace

### Step 2: Connect PCs to Switches
- PC0 → Switch0 (Fa0/1)
- PC1 → Switch0 (Fa0/2)
- PC2 → Switch1 (Fa0/1)
- PC3 → Switch1 (Fa0/2)
- PC4 → Switch2 (Fa0/1)
- PC5 → Switch2 (Fa0/2)
- PC6 → Switch3 (Fa0/1)
- PC7 → Switch3 (Fa0/2)

### Step 3: Connect Switches
- Switch0 (Fa0/3) → Switch1 (Fa0/3)
- Switch1 (Fa0/4) → Switch2 (Fa0/3)
- Switch2 (Fa0/4) → Switch3 (Fa0/3)

### Step 4: Assign IP Addresses
On each PC:
Desktop → IP Configuration → Set IP and subnet mask

---

## Verification

From PC0 Command Prompt:
```
ping 192.168.1.2
ping 192.168.1.3
ping 192.168.1.4
ping 192.168.1.5
ping 192.168.1.6
ping 192.168.1.7
ping 192.168.1.8
```

---

## Expected Output

### Same Switch (PC0 → PC1)
- 0% packet loss
- time <1ms

### Across Switches (PC0 → PC7)
- First ping may timeout (ARP resolution)
- Remaining replies successful (~1ms)

---

## Observation

All 8 PCs were successfully configured on the 192.168.1.0/24 subnet and connected through 4 daisy-chained switches.  
Ping tests confirmed Layer 2 connectivity across all switches.  
Initial packet loss is due to ARP resolution, which is expected.  
Switches successfully learned MAC addresses and forwarded frames.

---

## Optional Commands (Switch CLI)

```
show mac address-table
show interfaces status
```

These verify MAC learning and port status.
