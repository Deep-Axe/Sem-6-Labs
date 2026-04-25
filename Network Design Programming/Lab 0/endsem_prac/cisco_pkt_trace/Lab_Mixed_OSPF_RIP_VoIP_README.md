
---

## Aim
Configure a mixed network with:
- **Yellow region** (OSPF): Wireless Router, Laptop0, Smartphone0, IP Phone0, PC1, Laptop1 — all via Router2
- **White region** (RIP): Router1, Switch0, IP Phone1, Server0
- **VoIP**: IP Phone0 (ext. 1001) calls IP Phone1 (ext. 1002)
- **Connectivity test**: Laptop0 → Server0

---

## Full Network Topology (ASCII)

```
                        ╔══════════════════════════════════════════════════════════════╗
                        ║                  YELLOW REGION — OSPF                        ║
                        ║                                                              ║
                        ║   [Smartphone0]        [Laptop0]                             ║
                        ║        \  (WiFi)      /  (WiFi)                             ║
                        ║         \            /                                       ║
                        ║       [Wireless Router0 — WRT300N]                          ║
                        ║         LAN: 192.168.0.1                                    ║
                        ║         WAN: 10.10.10.2                                     ║
                        ║         DHCP pool: 192.168.0.100-150                        ║
                        ║              |                                               ║
                        ║              | Internet port → Fa0/0                        ║
                        ║              | (Straight-Through)                           ║
                        ║              |                                               ║
                        ║         [Router2 — Cisco 1841]                              ║
                        ║          Fa0/0      : 10.10.10.1  ← WAN link to WiFi router ║
                        ║          Vlan1      : 192.172.18.1 ← HWIC-4ESW ports        ║
                        ║          Se0/1/0    : 172.0.0.1   ← Serial to Router1       ║
                        ║          Modules: HWIC-2T, HWIC-4ESW                        ║
                        ║         /        |        \                                  ║
                        ║        /         |         \                                 ║
                        ║  Fa0/0/0    Fa0/0/1    Fa0/0/2                              ║
                        ║ (SW port)  (NIC)       (NIC)                                ║
                        ║     |          |            |                                ║
                        ║ [IP Phone0] [PC1]       [Laptop1]                           ║
                        ║ ext.1001   192.172.18.2  192.172.18.3                       ║
                        ║                                                              ║
                        ╚══════════════════════════════════════════════════════════════╝
                                          |
                                          | Se0/1/0 ←——→ Se0/3/0
                                          | Serial Cable
                                          | 172.0.0.0/24
                                          |
                        ╔══════════════════════════════════════════════════════════════╗
                        ║                  WHITE REGION — RIP                          ║
                        ║                                                              ║
                        ║         [Router1 — Cisco 2941]                              ║
                        ║          Se0/3/0    : 172.0.0.2   ← Serial to Router2       ║
                        ║          Gi0/0      : 192.192.18.1 ← Link to Switch0        ║
                        ║              |                                               ║
                        ║              | Gi0/0 → Fa0/1                                ║
                        ║              | (Straight-Through)                           ║
                        ║              |                                               ║
                        ║         [Switch0 — Cisco 2950-24]                           ║
                        ║          Fa0/1 → Router1 Gi0/0                              ║
                        ║          Fa0/2 → IP Phone1 (SW port)                        ║
                        ║          Fa0/3 → Server0 (NIC)                              ║
                        ║         /              \                                     ║
                        ║     Fa0/2            Fa0/3                                  ║
                        ║  (SW port)           (NIC)                                  ║
                        ║      |                  |                                    ║
                        ║ [IP Phone1]          [Server0]                              ║
                        ║  ext.1002            192.192.18.2                           ║
                        ║                                                              ║
                        ╚══════════════════════════════════════════════════════════════╝
```

---

## IP Address Reference Table

| Device | Interface | IP Address | Subnet Mask | Gateway | Notes |
|---|---|---|---|---|---|
| Wireless Router0 | LAN | 192.168.0.1 | 255.255.255.0 | — | DHCP for wireless clients |
| Wireless Router0 | WAN/Internet | 10.10.10.2 | 255.255.255.0 | 10.10.10.1 | Faces Router2 Fa0/0 |
| Laptop0 | WiFi | 192.168.0.100 (DHCP) | 255.255.255.0 | 192.168.0.1 | Wireless client |
| Smartphone0 | WiFi | 192.168.0.101 (DHCP) | 255.255.255.0 | 192.168.0.1 | Wireless client |
| Router2 | Fa0/0 | 10.10.10.1 | 255.255.255.0 | — | Faces Wireless Router WAN |
| Router2 | Vlan1 | 192.172.18.1 | 255.255.255.0 | — | HWIC-4ESW switch ports |
| Router2 | Se0/1/0 | 172.0.0.1 | 255.255.255.0 | — | DCE — needs clock rate |
| IP Phone0 | SW port | 192.172.18.x (DHCP) | 255.255.255.0 | 192.172.18.1 | Gets IP from Router2 DHCP |
| PC1 | NIC | 192.172.18.2 | 255.255.255.0 | 192.172.18.1 | Static |
| Laptop1 | NIC | 192.172.18.3 | 255.255.255.0 | 192.172.18.1 | Static |
| Router1 | Se0/3/0 | 172.0.0.2 | 255.255.255.0 | — | DTE — no clock rate |
| Router1 | Gi0/0 | 192.192.18.1 | 255.255.255.0 | — | Faces Switch0 |
| IP Phone1 | SW port | 192.192.18.x (DHCP) | 255.255.255.0 | 192.192.18.1 | Gets IP from Router1 DHCP |
| Server0 | NIC | 192.192.18.2 | 255.255.255.0 | 192.192.18.1 | Static |

---

## Cable Connection Table

| From Device | From Port | To Device | To Port | Cable Type |
|---|---|---|---|---|
| Wireless Router0 | Internet (WAN) | Router2 | Fa0/0 | Straight-Through |
| Router2 | Fa0/0/0 | IP Phone0 | SW | Straight-Through |
| Router2 | Fa0/0/1 | PC1 | NIC | Straight-Through |
| Router2 | Fa0/0/2 | Laptop1 | NIC | Straight-Through |
| Router2 | Se0/1/0 | Router1 | Se0/3/0 | Serial (DCE→Router2) |
| Router1 | Gi0/0 | Switch0 | Fa0/1 | Straight-Through |
| Switch0 | Fa0/2 | IP Phone1 | SW | Straight-Through |
| Switch0 | Fa0/3 | Server0 | NIC | Straight-Through |

> **IP Phone port note:** IP Phones have two ports — always use the **SW** port (not the PC port) when connecting to a switch or router.

---

## Module Installation

### Router2 (1841) — add two modules BEFORE connecting cables

**Power off Router2 first (Physical tab → click power button)**

| Module | Slot | Gives you |
|---|---|---|
| HWIC-2T | HWIC slot 0 | Se0/0/0, Se0/0/1 — BUT on this router serial appears as Se0/1/0 |
| HWIC-4ESW | HWIC slot 1 | Fa0/0/0, Fa0/0/1, Fa0/0/2, Fa0/0/3 |

Steps:
1. Click Router2 → **Physical** tab
2. Click power button to **power off**
3. Find **HWIC-2T** in left module panel → drag into first HWIC slot
4. Find **HWIC-4ESW** in left module panel → drag into second HWIC slot
5. Click power button to **power on**
6. Verify with `show ip interface brief` — you should see Fa0/0/0 through Fa0/0/3 and Se0/1/0

### Router1 (2941) — add HWIC-2T for serial

1. Click Router1 → **Physical** tab
2. **Power off**
3. Drag **HWIC-2T** into HWIC slot
4. **Power on**
5. Serial port will appear as `Se0/3/0` (verify with `show ip interface brief`)

---

## STEP 1 — Configure Wireless Router0 (GUI)

Click **Wireless Router0 → GUI tab** → log in (admin/admin if prompted)

### Setup → Basic Setup

```
Internet Connection Type : Static IP
  Internet IP Address    : 10.10.10.2
  Subnet Mask            : 255.255.255.0
  Default Gateway        : 10.10.10.1

Network Setup:
  Router IP              : 192.168.0.1
  Subnet Mask            : 255.255.255.0

DHCP Server Settings:
  DHCP Server            : Enabled
  Start IP               : 100    (gives 192.168.0.100)
  Max Users              : 50     (pool: .100 to .150)
```
Click **Save Settings**

### Wireless → Basic Wireless Settings

```
SSID                     : Homenet
Network Mode             : Mixed
Radio Band               : Auto
```
Click **Save Settings**

### Wireless → Wireless Security

```
Security Mode            : Disabled
```
Click **Save Settings**

---

## STEP 2 — Configure Wireless Clients

### Laptop0 — add wireless module first

1. Click **Laptop0** → **Physical** tab
2. **Power off**
3. Remove existing wired NIC module
4. Drag in **WPC300N** module
5. **Power on**
6. Desktop → **PC Wireless** → Connect tab → Refresh
7. Select `Homenet` → **Connect**
8. Desktop → **IP Configuration** → **DHCP**
9. Should receive `192.168.0.100`

### Smartphone0

1. Click **Smartphone0** → **Config** tab → **Wireless0**
2. Set SSID: `Homenet`
3. **IP Configuration** → **DHCP**
4. Should receive `192.168.0.101`

---

## STEP 3 — Configure Router2 (Cisco 1841)

Click **Router2 → CLI tab**

### 3a — Basic interface config

```
enable
configure terminal
hostname Router2

! Fa0/0 faces Wireless Router0 WAN port
interface fastEthernet 0/0
ip address 10.10.10.1 255.255.255.0
no shutdown
exit

! Vlan1 is the gateway for ALL HWIC-4ESW ports (Fa0/0/0, Fa0/0/1, Fa0/0/2, Fa0/0/3)
! These ports act as a layer-2 switch internally — you cannot assign IPs per port
! Assign one IP to Vlan1 instead
interface vlan 1
ip address 192.172.18.1 255.255.255.0
no shutdown
exit

! Serial link to Router1
interface serial 0/1/0
ip address 172.0.0.1 255.255.255.0
clock rate 64000
no shutdown
exit
```

### 3b — DHCP for IP Phone0 (VoIP)

```
! Exclude router's own IP from pool
ip dhcp excluded-address 192.172.18.1
ip dhcp excluded-address 192.172.18.2
ip dhcp excluded-address 192.172.18.3

! DHCP pool — phones use option 150 to find call manager
ip dhcp pool VOICE
 network 192.172.18.0 255.255.255.0
 default-router 192.172.18.1
 option 150 ip 192.172.18.1
exit
```

### 3c — Telephony service (Router2 as call manager)

```
telephony-service
 max-dn 5
 max-ephones 5
 ip source-address 192.172.18.1 port 2000
 auto assign 1 to 5
exit

ephone-dn 1
 number 1001
exit

ephone-dn 2
 number 1002
exit
```

### 3d — OSPF (yellow region)

```
router ospf 1
 network 10.10.10.0 0.0.0.255 area 0
 network 192.168.0.0 0.0.0.255 area 0
 network 192.172.18.0 0.0.0.255 area 0
 network 172.0.0.0 0.0.0.255 area 0
exit
```

### 3e — RIP (advertise serial link into white region)

```
router rip
 network 172.0.0.0
exit
```

### 3f — Change Hello Interval to 20 seconds (part d of question)

```
interface serial 0/1/0
 ip ospf hello-interval 20
 ip ospf dead-interval 80
exit
```

> Dead interval must always be 4× the hello interval. Default hello = 10s, dead = 40s. New: hello = 20s, dead = 80s.

### 3g — Save

```
end
write memory
```

---

## STEP 4 — Configure Router1 (Cisco 2941)

Click **Router1 → CLI tab**

```
enable
configure terminal
hostname Router1

! Serial link back to Router2
interface serial 0/3/0
 ip address 172.0.0.2 255.255.255.0
 no shutdown
exit

! Gigabit link to Switch0
interface gigabitEthernet 0/0
 ip address 192.192.18.1 255.255.255.0
 no shutdown
exit

! DHCP for IP Phone1
ip dhcp excluded-address 192.192.18.1
ip dhcp excluded-address 192.192.18.2

ip dhcp pool VOICE2
 network 192.192.18.0 255.255.255.0
 default-router 192.192.18.1
 option 150 ip 192.192.18.1
exit

! RIP for white region
router rip
 network 172.0.0.0
 network 192.192.18.0
exit

end
write memory
```

---

## STEP 5 — Configure Switch0 (Cisco 2950-24)

Click **Switch0 → CLI tab**

```
enable
configure terminal

! Port facing Router1
interface fastEthernet 0/1
 switchport mode access
 switchport voice vlan 1
exit

! Port facing IP Phone1
interface fastEthernet 0/2
 switchport mode access
 switchport voice vlan 1
exit

! Port facing Server0
interface fastEthernet 0/3
 switchport mode access
exit

end
```

---

## STEP 6 — Set Static IPs on End Devices

### PC1 — Desktop → IP Configuration → Static

```
IP Address      : 192.172.18.2
Subnet Mask     : 255.255.255.0
Default Gateway : 192.172.18.1
```

### Laptop1 — Desktop → IP Configuration → Static

```
IP Address      : 192.172.18.3
Subnet Mask     : 255.255.255.0
Default Gateway : 192.172.18.1
```

### Server0 — Desktop → IP Configuration → Static

```
IP Address      : 192.192.18.2
Subnet Mask     : 255.255.255.0
Default Gateway : 192.192.18.1
```

---

## STEP 7 — Wait for IP Phones to Register

- Wait **30–60 seconds** after all config is done
- IP Phone0 should display **1001** on screen
- IP Phone1 should display **1002** on screen

If phones don't register after 60 seconds, check on Router2:
```
show ephone
```
And on Router1:
```
show ephone
```

---

## STEP 8 — Verification

### Show routing table on Router2

```
Router2#show ip route
```

Look for:
- `C` = directly connected
- `O` = OSPF learned routes
- `R` = RIP learned routes (172.0.0.0 from Router1)

Expected output should include entries for:
- `10.10.10.0` (C)
- `192.172.18.0` (C)
- `172.0.0.0` (C)
- `192.192.18.0` (R — learned via RIP from Router1)
- `192.168.0.0` (O — via OSPF)

### Check OSPF neighbors

```
Router2#show ip ospf neighbor
```

### Check RIP

```
Router2#show ip protocols
```

### Check interfaces are all up

```
Router2#show ip interface brief
```

All configured interfaces should show `up/up`.

---

## STEP 9 — Test Connectivity (Laptop0 → Server0)

On **Laptop0** → Desktop → Command Prompt:

```
ping 192.192.18.2
```

Expected:
```
Pinging 192.192.18.2 with 32 bytes of data:
Reply from 192.192.18.2: bytes=32 time=Xms TTL=XXX
Reply from 192.192.18.2: bytes=32 time=Xms TTL=XXX
```

If ping fails, work step by step:
```
ping 192.168.0.1      ← can Laptop0 reach Wireless Router LAN?
ping 10.10.10.1       ← can it reach Router2 Fa0/0?
ping 192.172.18.1     ← can it reach Router2 Vlan1?
ping 172.0.0.2        ← can it reach Router1 serial?
ping 192.192.18.1     ← can it reach Router1 Gi0/0?
ping 192.192.18.2     ← can it reach Server0?
```
The first ping that fails tells you exactly where the problem is.

---

## STEP 10 — Make VoIP Call (IP Phone0 → IP Phone1)

1. Click **IP Phone0** → **GUI** tab
2. Click the **handset icon** to pick up
3. Dial **1 0 0 2** on the keypad
4. IP Phone1 will ring
5. Click **IP Phone1** → **GUI** tab → click handset to answer
6. Call connected ✓

---

## Troubleshooting Reference

| Problem | Likely Cause | Fix |
|---|---|---|
| Wireless devices not getting IP | Wrong Router IP on Wireless Router | Set LAN IP to 192.168.0.1, WAN to 10.10.10.2 |
| HWIC ports not accepting IP | Ports are layer-2, can't have individual IPs | Use `interface vlan 1` for the IP |
| IP Phone not registering | DHCP option 150 missing or wrong | Check `ip dhcp pool VOICE` has `option 150 ip 192.172.18.1` |
| Serial link down | Missing clock rate on DCE | Add `clock rate 64000` on Router2 Se0/1/0 |
| No RIP routes on Router2 | RIP not advertising serial network | Add `network 172.0.0.0` under `router rip` |
| Ping across regions fails | OSPF/RIP not redistributing | Check both routing protocols have the 172.0.0.0 network |
| `show ephone` shows no phones | Telephony service misconfigured | Re-check `ip source-address` matches Vlan1 IP |
| Dead interval mismatch warning | Hello changed without updating dead | Set `ip ospf dead-interval 80` alongside hello 20 |

---

## Quick Reference Summary

```
OSPF Region (Yellow):
  Wireless Router LAN  : 192.168.0.1
  Wireless Router WAN  : 10.10.10.2
  Router2 Fa0/0        : 10.10.10.1
  Router2 Vlan1        : 192.172.18.1
  Router2 Se0/1/0      : 172.0.0.1  (DCE, clock rate 64000)
  IP Phone0 ext        : 1001
  PC1                  : 192.172.18.2
  Laptop1              : 192.172.18.3

RIP Region (White):
  Router1 Se0/3/0      : 172.0.0.2  (DTE)
  Router1 Gi0/0        : 192.192.18.1
  IP Phone1 ext        : 1002
  Server0              : 192.192.18.2

Serial link (connects regions):
  172.0.0.0/24
  Router2 Se0/1/0 ←——→ Router1 Se0/3/0

OSPF Hello Interval  : 20 seconds
OSPF Dead Interval   : 80 seconds
VoIP Call Manager    : Router2 at 192.172.18.1 port 2000
```
