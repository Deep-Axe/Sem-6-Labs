# Lab — OSPF, DHCP, VoIP, SSH Network Configuration (Packet Tracer)
### Section B — Batch 2

---

## Aim
Configure a full network with:
- **OSPF** routing across all routers with modified invalid timer (70 seconds)
- **DHCP Server** on Router1 (Manipal) serving all other routers
- **VoIP**: IP Phone0 (Bengaluru) calls IP Phone1 (Manipal)
- **DNS + HTTP**: Test connectivity via `www.exam.org` from Laptop0 to Server0
- **SSH**: PC1 at Manipal connects to Router1 via SSH

---

## Full Network Topology (ASCII)

```
BENGALURU LOCATION                          OSPF BACKBONE                    MANIPAL LOCATION
════════════════════                   ══════════════════════              ══════════════════════

[IP Phone0]   [PC0]                                                      [IP Phone1]  [Server0]
 7960          PC-PT                                                       7960         Server-PT
 Fa (SW)       Fa0                                                         Fa (SW)      Fa0
    |            |                                                              |            |
    |            |                                                              |            |
 Fa0/1        Fa0/2                                                          Fa0/2        Fa0/1 (Fa0)
    \            /                                                               \          /
     [Switch1 — 2960-24TT]                                              [Switch0 — 2960-24TT]
      Fa0/3                                                                Fa0/1
        |                                                                    |
        | 172.19.3.0/24                                                      | 172.19.3.8/24
        |                                                                    |
      Fa0/0                                                                Fa0/3
    [Router0 — 2811]                                               [Router1 — 2811]  ←── DHCP Server
      Fa0/1                                                                Fa0/0
        |                                                                    |
        | 192.168.3.28/24                                                    | 192.168.3.20/24
        |                                                                    |
      Fa0/2                                                                Fa0/0 (to PC1 via cyan link)
    [Switch2 — 2960-24TT]                                                   |
      Fa0/1        Fa0/3                                              [PC1 — PC-PT]
        |              |                                              192.168.3.20 subnet
        |              |
      Fa0           Gig0/0
  [Laptop0]       [Router2 — 2911]
   Laptop-PT        Se0/0/0 ←─────────────── Se0/0/0
                    172.19.3.16/24 ←── OSPF ──→ 172.19.3.16
                                            [Router1 Se0/0/0]

Serial link (RED):  Router2 Se0/0/0 ←——→ Router1 Se0/0/0  (172.19.3.16/24)
```

### Cleaner connection view

```
[IP Phone0]──Fa──[Switch1]──Fa0/3──[Router0]──Fa0/1──[Switch2]──Fa0/3──[Router2]
                    |                                      |
                  Fa0/2                                  Fa0/1
                    |                                      |
                  [PC0]                               [Laptop0]

[Router2] Gig0/0 ←── 192.168.3.28 ──→ (Switch2 Fa0/2)
[Router2] Se0/0/0 ←── 172.19.3.16 ──→ Se0/0/0 [Router1]
[Router1] Fa0/3 ←── 172.19.3.8 ──→ Fa0/1 [Switch0]
[Router1] Fa0/0 ←── 192.168.3.20 ──→ [PC1]  (SSH + DHCP management)

[Switch0] Fa0/2 ──→ [Server0]
[Switch0] Fa0/1(Fa) ──→ [IP Phone1]
```

---

## IP Address Reference Table

| Device | Interface | IP Address | Subnet Mask | Gateway | Notes |
|---|---|---|---|---|---|
| Router0 | Fa0/0 | 172.19.3.1 | 255.255.255.0 | — | Faces Switch1 |
| Router0 | Fa0/1 | 192.168.3.1 | 255.255.255.0 | — | Faces Switch2 |
| Router2 | Gig0/0 | 192.168.3.28 | 255.255.255.0 | — | Faces Switch2 |
| Router2 | Se0/0/0 | 172.19.3.16 | 255.255.255.0 | — | Serial to Router1 (DCE) |
| Router1 | Se0/0/0 | 172.19.3.17 | 255.255.255.0 | — | Serial to Router2 (DTE) |
| Router1 | Fa0/3 | 172.19.3.8 | 255.255.255.0 | — | Faces Switch0 |
| Router1 | Fa0/0 | 192.168.3.20 | 255.255.255.0 | — | Faces PC1 (SSH/DHCP mgmt) |
| PC0 | Fa0 | DHCP | 255.255.255.0 | 172.19.3.1 | Gets IP from Router1 DHCP |
| PC1 | Fa0 | 192.168.3.21 | 255.255.255.0 | 192.168.3.20 | Static — SSH management |
| Laptop0 | Fa0 | DHCP | 255.255.255.0 | 192.168.3.1 | Gets IP from Router1 DHCP |
| Server0 | Fa0 | 192.168.3.10 | 255.255.255.0 | 172.19.3.8 | Hosts www.exam.org |
| IP Phone0 | SW | DHCP | 255.255.255.0 | 172.19.3.1 | ext. 1001 |
| IP Phone1 | SW | DHCP | 255.255.255.0 | 172.19.3.8 | ext. 1002 |

---

## Cable Connection Table

| From | Port | To | Port | Cable |
|---|---|---|---|---|
| Router0 | Fa0/0 | Switch1 | Fa0/3 | Straight-Through |
| Router0 | Fa0/1 | Switch2 | Fa0/2 | Straight-Through |
| Switch1 | Fa0/1 | IP Phone0 | SW | Straight-Through |
| Switch1 | Fa0/2 | PC0 | Fa0 | Straight-Through |
| Switch2 | Fa0/1 | Laptop0 | Fa0 | Straight-Through |
| Switch2 | Fa0/3 | Router2 | Gig0/0 | Straight-Through |
| Router2 | Se0/0/0 | Router1 | Se0/0/0 | Serial (DCE→Router2) |
| Router1 | Fa0/3 | Switch0 | Fa0/1 | Straight-Through |
| Router1 | Fa0/0 | PC1 | Fa0 | Straight-Through |
| Switch0 | Fa0/2 | Server0 | Fa0 | Straight-Through |
| Switch0 | Fa (Fa0/1) | IP Phone1 | SW | Straight-Through |

> **Note:** Router2 is a 2911 — serial port is `Se0/0/0`. Add **HWIC-2T** module if serial ports are missing.

---

## STEP 1 — Configure Router0 (Cisco 2811 — Bengaluru)

Click **Router0 → CLI**:

```
enable
configure terminal
hostname Router0

! Interface facing Switch1 (IP Phone0 + PC0)
interface fastEthernet 0/0
 ip address 172.19.3.1 255.255.255.0
 no shutdown
exit

! Interface facing Switch2 (Laptop0 + Router2)
interface fastEthernet 0/1
 ip address 192.168.3.1 255.255.255.0
 no shutdown
exit

! OSPF — advertise both connected networks
router ospf 1
 network 172.19.3.0 0.0.0.255 area 0
 network 192.168.3.0 0.0.0.255 area 0
exit

! Set OSPF invalid timer to 70 seconds (default is 90)
! Invalid timer is set per interface using dead-interval
interface fastEthernet 0/0
 ip ospf dead-interval 70
exit
interface fastEthernet 0/1
 ip ospf dead-interval 70
exit

end
write memory
```

---

## STEP 2 — Configure Router2 (Cisco 2911 — Core/Middle)

> Add **HWIC-2T** module first if Se0/0/0 is not visible (Physical tab → power off → add module → power on)

Click **Router2 → CLI**:

```
enable
configure terminal
hostname Router2

! Interface facing Switch2
interface gigabitEthernet 0/0
 ip address 192.168.3.28 255.255.255.0
 no shutdown
exit

! Serial interface facing Router1 — Router2 is DCE so needs clock rate
interface serial 0/0/0
 ip address 172.19.3.16 255.255.255.0
 clock rate 64000
 no shutdown
exit

! OSPF
router ospf 1
 network 192.168.3.0 0.0.0.255 area 0
 network 172.19.3.0 0.0.0.255 area 0
exit

! Set OSPF invalid timer to 70 seconds on all interfaces
interface gigabitEthernet 0/0
 ip ospf dead-interval 70
exit
interface serial 0/0/0
 ip ospf dead-interval 70
exit

end
write memory
```

---

## STEP 3 — Configure Router1 (Cisco 2811 — Manipal) — DHCP Server + OSPF

Router1 is the **central DHCP server** for the entire network. All other devices get IPs from it.

Click **Router1 → CLI**:

```
enable
configure terminal
hostname Router1

! Interface facing Router2 via serial
interface serial 0/0/0
 ip address 172.19.3.17 255.255.255.0
 no shutdown
exit

! Interface facing Switch0 (IP Phone1 + Server0)
interface fastEthernet 0/3
 ip address 172.19.3.8 255.255.255.0
 no shutdown
exit

! Interface facing PC1 (SSH management port)
interface fastEthernet 0/0
 ip address 192.168.3.20 255.255.255.0
 no shutdown
exit

! ── DHCP SERVER CONFIGURATION ──────────────────────────────────────
! Exclude all router interface IPs and static device IPs from pools

ip dhcp excluded-address 172.19.3.1
ip dhcp excluded-address 172.19.3.8
ip dhcp excluded-address 172.19.3.16
ip dhcp excluded-address 172.19.3.17
ip dhcp excluded-address 192.168.3.1
ip dhcp excluded-address 192.168.3.10
ip dhcp excluded-address 192.168.3.20
ip dhcp excluded-address 192.168.3.21
ip dhcp excluded-address 192.168.3.28

! Pool for Bengaluru LAN (PC0 + IP Phone0) — 172.19.3.0/24
ip dhcp pool BENGALURU_LAN
 network 172.19.3.0 255.255.255.0
 default-router 172.19.3.1
 dns-server 192.168.3.10
 option 150 ip 172.19.3.1
exit

! Pool for Switch2 LAN (Laptop0) — 192.168.3.0/24
ip dhcp pool SWITCH2_LAN
 network 192.168.3.0 255.255.255.0
 default-router 192.168.3.1
 dns-server 192.168.3.10
exit

! Pool for Manipal LAN (IP Phone1) — 172.19.3.8/24 subnet
ip dhcp pool MANIPAL_LAN
 network 172.19.3.0 255.255.255.0
 default-router 172.19.3.8
 dns-server 192.168.3.10
 option 150 ip 172.19.3.8
exit

! ── OSPF ────────────────────────────────────────────────────────────
router ospf 1
 network 172.19.3.0 0.0.0.255 area 0
 network 192.168.3.0 0.0.0.255 area 0
exit

! Set OSPF invalid timer to 70 seconds on all interfaces
interface serial 0/0/0
 ip ospf dead-interval 70
exit
interface fastEthernet 0/3
 ip ospf dead-interval 70
exit
interface fastEthernet 0/0
 ip ospf dead-interval 70
exit

! ── SSH CONFIGURATION ───────────────────────────────────────────────
ip domain-name exam.org
crypto key generate rsa
! When prompted: enter 1024
username admin privilege 15 secret Manipal123

line vty 0 4
 login local
 transport input ssh
exit

ip ssh version 2

end
write memory
```

---

## STEP 4 — Configure ip helper-address on Router0 and Router2

Since Router1 is the central DHCP server but devices are on different routers, each router needs to **relay DHCP requests** to Router1 using `ip helper-address`.

### On Router0 — relay DHCP for PC0 and IP Phone0

```
enable
configure terminal

interface fastEthernet 0/0
 ip helper-address 172.19.3.17
exit

interface fastEthernet 0/1
 ip helper-address 172.19.3.17
exit

end
write memory
```

### On Router2 — relay DHCP for Laptop0

```
enable
configure terminal

interface gigabitEthernet 0/0
 ip helper-address 172.19.3.17
exit

end
write memory
```

> `ip helper-address` forwards DHCP broadcast requests as unicast to the DHCP server (Router1 at 172.19.3.17). Without this, devices on other subnets cannot reach Router1's DHCP service.

---

## STEP 5 — Configure Switch1 (Bengaluru)

Click **Switch1 → CLI**:

```
enable
configure terminal

! Port facing Router0
interface fastEthernet 0/3
 switchport mode access
 switchport voice vlan 1
exit

! Port facing IP Phone0
interface fastEthernet 0/1
 switchport mode access
 switchport voice vlan 1
exit

! Port facing PC0
interface fastEthernet 0/2
 switchport mode access
exit

end
```

---

## STEP 6 — Configure Switch2 (Middle/Core)

Click **Switch2 → CLI**:

```
enable
configure terminal

! Port facing Router0
interface fastEthernet 0/2
 switchport mode access
exit

! Port facing Laptop0
interface fastEthernet 0/1
 switchport mode access
exit

! Port facing Router2
interface fastEthernet 0/3
 switchport mode access
exit

end
```

---

## STEP 7 — Configure Switch0 (Manipal)

Click **Switch0 → CLI**:

```
enable
configure terminal

! Port facing Router1
interface fastEthernet 0/1
 switchport mode access
 switchport voice vlan 1
exit

! Port facing IP Phone1
interface fastEthernet 0/1
 switchport mode access
 switchport voice vlan 1
exit

! Port facing Server0
interface fastEthernet 0/2
 switchport mode access
exit

end
```

---

## STEP 8 — Configure Server0 (www.exam.org)

Click **Server0** → **Desktop** → **IP Configuration** → Static:

```
IP Address      : 192.168.3.10
Subnet Mask     : 255.255.255.0
Default Gateway : 172.19.3.8
DNS Server      : 192.168.3.10
```

### Enable DNS service on Server0

Click **Server0** → **Services** tab → **DNS**:
- DNS Service: **On**
- Add record:
  - Name: `www.exam.org`
  - Type: `A`
  - Address: `192.168.3.10`
- Click **Add**

### Enable HTTP service on Server0

Click **Server0** → **Services** tab → **HTTP**:
- HTTP: **On**
- HTTPS: **On**

---

## STEP 9 — Configure PC1 (Static IP — SSH management)

Click **PC1** → **Desktop** → **IP Configuration** → Static:

```
IP Address      : 192.168.3.21
Subnet Mask     : 255.255.255.0
Default Gateway : 192.168.3.20
DNS Server      : 192.168.3.10
```

---

## STEP 10 — Configure VoIP (IP Phone0 → IP Phone1)

VoIP is split across two routers:
- **IP Phone0** registers with **Router0** (Bengaluru call manager)
- **IP Phone1** registers with **Router1** (Manipal call manager)
- Calls route over OSPF

### On Router0 — call manager for IP Phone0

```
enable
configure terminal

ip dhcp excluded-address 172.19.3.1
ip dhcp pool VOICE_BLR
 network 172.19.3.0 255.255.255.0
 default-router 172.19.3.1
 option 150 ip 172.19.3.1
exit

telephony-service
 max-dn 5
 max-ephones 5
 ip source-address 172.19.3.1 port 2000
 auto assign 1 to 5
exit

ephone-dn 1
 number 1001
exit

end
write memory
```

### On Router1 — call manager for IP Phone1

```
enable
configure terminal

telephony-service
 max-dn 5
 max-ephones 5
 ip source-address 172.19.3.8 port 2000
 auto assign 1 to 5
exit

ephone-dn 2
 number 1002
exit

end
write memory
```

### Dial peer — so Router0 knows how to reach ext. 1002 on Router1

On **Router0**:
```
enable
configure terminal

dial-peer voice 1 voip
 destination-pattern 1002
 session target ipv4:172.19.3.8
exit

end
write memory
```

On **Router1**:
```
enable
configure terminal

dial-peer voice 2 voip
 destination-pattern 1001
 session target ipv4:172.19.3.1
exit

end
write memory
```

---

## STEP 11 — Display OSPF Timer (Question 1)

On any router, run:

```
show ip ospf
```

Look for the line:
```
  Routing Invalid after 70 seconds
```

This confirms the invalid timer is set to 70 seconds.

Also run:
```
show ip ospf interface fastEthernet 0/0
```
To see per-interface timer details.

---

## STEP 12 — Test Laptop0 → www.exam.org (Question 2)

First confirm Laptop0 got an IP via DHCP:

On **Laptop0** → Desktop → Command Prompt:
```
ipconfig
```
Should show an IP in the `192.168.3.0/24` range.

Then test HTTP connectivity:
```
ping www.exam.org
```

Or use the **Web Browser** on Laptop0:
- Desktop → **Web Browser**
- URL: `http://www.exam.org`
- Should load the default Cisco PT server page

---

## STEP 13 — Display DHCP Bindings (Question 4)

### View from Router1 (DHCP server)

On **Router1 → CLI**:
```
show ip dhcp binding
```

Expected output shows all leased IPs:
```
IP address       Client-ID               Lease expiration        Type
172.19.3.100     0010.1187.xxxx          --                      Automatic
192.168.3.100    0010.1187.xxxx          --                      Automatic
```

```
show ip dhcp pool
```
Shows pool names, ranges, and lease counts.

### View from PC0 (Bengaluru) — confirm Router2's DHCP lease

On **PC0** → Desktop → Command Prompt:
```
ipconfig /all
```
Shows IP address, subnet, gateway, and DHCP server address.

---

## STEP 14 — SSH from PC1 to Router1

On **PC1** → Desktop → **Command Prompt**:

```
ssh -l admin 192.168.3.20
```

When prompted, enter password: `Manipal123`

You should get a Router1 CLI prompt, confirming SSH is working.

---

## STEP 15 — Make VoIP Call (IP Phone0 → IP Phone1)

1. Wait 30–60 seconds for phones to register
2. Click **IP Phone0** → GUI → check it shows **1001**
3. Click **IP Phone1** → GUI → check it shows **1002**
4. On IP Phone0 → click handset → dial `1002`
5. IP Phone1 rings → click handset to answer
6. Call connected ✓

---

## Verification Commands — Full Reference

### On any router

```
show ip route                ! Full routing table (O = OSPF, C = connected)
show ip ospf                 ! OSPF process info + timers
show ip ospf neighbor        ! Neighbor adjacency (look for FULL state)
show ip ospf interface       ! Per-interface OSPF details including timers
show ip protocols            ! Routing protocol summary
show ip dhcp binding         ! All DHCP leases (run on Router1)
show ip dhcp pool            ! Pool stats
show ip interface brief      ! All interfaces and their status
show ephone                  ! VoIP phone registration status
show ephone-dn               ! Directory number assignments
```

---

## Troubleshooting Reference

| Problem | Likely Cause | Fix |
|---|---|---|
| PC0/Laptop0 not getting IP | `ip helper-address` missing | Add `ip helper-address 172.19.3.17` on Router0/Router2 interfaces |
| OSPF routes not appearing | Routers not in same area | All must be `area 0` in `router ospf 1` |
| IP Phone not registering | Wrong `ip source-address` | Must match the router interface IP on same subnet as phone |
| VoIP call not crossing routers | Missing dial-peer | Add `dial-peer voice` on both routers pointing to each other |
| www.exam.org not resolving | DNS not configured on Server0 | Add A record in Services → DNS on Server0 |
| SSH not working | RSA key not generated | Run `crypto key generate rsa` and choose 1024 |
| Serial link down | Missing clock rate | Add `clock rate 64000` on Router2 Se0/0/0 (DCE side) |
| OSPF timer not showing 70s | Dead-interval set but not invalid | Invalid timer = dead-interval in Packet Tracer — verify with `show ip ospf` |

---

## Quick Reference Summary

```
BENGALURU (Router0 — 2811):
  Fa0/0  : 172.19.3.1/24   → Switch1 (IP Phone0 + PC0)
  Fa0/1  : 192.168.3.1/24  → Switch2 (Laptop0 + Router2)
  VoIP   : source 172.19.3.1 port 2000, ext 1001
  Relay  : ip helper-address 172.19.3.17 on both interfaces

CORE (Router2 — 2911):
  Gi0/0  : 192.168.3.28/24 → Switch2
  Se0/0/0: 172.19.3.16/24  → Router1 (DCE, clock rate 64000)
  Relay  : ip helper-address 172.19.3.17 on Gi0/0

MANIPAL (Router1 — 2811) — DHCP SERVER:
  Se0/0/0: 172.19.3.17/24  → Router2 (DTE)
  Fa0/3  : 172.19.3.8/24   → Switch0 (IP Phone1 + Server0)
  Fa0/0  : 192.168.3.20/24 → PC1 (SSH)
  VoIP   : source 172.19.3.8 port 2000, ext 1002
  SSH    : domain exam.org, RSA 1024, user admin

SERVER0:
  IP     : 192.168.3.10
  DNS    : www.exam.org → 192.168.3.10
  HTTP   : Enabled

PC1 (SSH client):
  IP     : 192.168.3.21
  GW     : 192.168.3.20
  SSH to : 192.168.3.20 (Router1 Fa0/0)

OSPF:
  Process ID : 1
  Area       : 0 (all routers)
  Invalid timer (dead-interval) : 70 seconds on all interfaces

VoIP dial-peers:
  Router0 → 1002 via 172.19.3.8
  Router1 → 1001 via 172.19.3.1
```
