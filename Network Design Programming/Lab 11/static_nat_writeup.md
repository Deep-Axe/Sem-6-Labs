# Static NAT Configuration — Lab Writeup

## Objective

Configure Static NAT on a two-router Cisco Packet Tracer topology so that a PC on a private LAN can communicate with a server on a separate network, using Network Address Translation to map the private IP to a public IP.

---

## Network Topology

```
PC0 (192.168.1.2)  ──┐
                      ├── Switch0 ── Router0 ══(Serial)══ Router1 ── Switch1 ──┬── PC2/PC3/PC4
PC1 (192.168.1.3)  ──┘              (NAT)                                      └── Server0 (192.168.2.10)
```

### IP Address Scheme

| Device   | Interface          | IP Address       | Role               |
|----------|--------------------|------------------|--------------------|
| PC0      | NIC                | 192.168.1.2      | NAT inside host    |
| PC1      | NIC                | 192.168.1.3      | LAN host           |
| Router0  | GigabitEthernet0/0/0 | 192.168.1.1    | LAN gateway (inside NAT) |
| Router0  | Serial0/1/0        | 10.0.0.1         | WAN interface (outside NAT) |
| Router1  | Serial0/1/0        | 10.0.0.2         | WAN interface      |
| Router1  | GigabitEthernet0/0/0 | 192.168.2.1    | Right LAN gateway  |
| Server0  | NIC                | 192.168.2.10     | Destination server |
| PC0 (public) | —              | 10.0.0.5         | Static NAT mapping |

### Cable Used
A **Serial DCE/DTE cable** connects Router0 and Router1 via `Serial0/1/0` on both sides. The DCE end requires `clock rate 64000`.

> **Note:** The ISR4331 uses `GigabitEthernet0/0/0` (not FastEthernet) and `Serial0/1/0` (not Serial0/0/0). These were discovered during configuration when the expected interface names returned `%Invalid interface type and number`.

---

## Configuration

### Router0 (NAT Router)

```
Router0# configure terminal

! Step 1: Define the static NAT translation
Router0(config)# ip nat inside source static 192.168.1.2 10.0.0.5

! Step 2: Configure LAN interface as NAT inside
Router0(config)# interface GigabitEthernet0/0/0
Router0(config-if)# ip address 192.168.1.1 255.255.255.0
Router0(config-if)# ip nat inside
Router0(config-if)# no shutdown

! Step 3: Configure WAN interface as NAT outside
Router0(config-if)# interface Serial0/1/0
Router0(config-if)# ip address 10.0.0.1 255.255.255.252
Router0(config-if)# ip nat outside
Router0(config-if)# no shutdown

! Step 4: Add static route to right LAN
Router0(config-if)# exit
Router0(config)# ip route 192.168.2.0 255.255.255.0 10.0.0.2
Router0(config)# end
Router0# write memory
```

### Router1

```
Router1# configure terminal

! Step 1: Configure LAN interface
Router1(config)# interface GigabitEthernet0/0/0
Router1(config-if)# ip address 192.168.2.1 255.255.255.0
Router1(config-if)# no shutdown

! Step 2: Configure WAN interface (DCE side — clock rate accepted here)
Router1(config-if)# interface Serial0/1/0
Router1(config-if)# ip address 10.0.0.2 255.255.255.252
Router1(config-if)# clock rate 64000
Router1(config-if)# no shutdown

! Step 3: Route back to left LAN
Router1(config-if)# exit
Router1(config)# ip route 192.168.1.0 255.255.255.0 10.0.0.1

! Step 4: Host route for PC0's public NAT IP (critical for return traffic)
Router1(config)# ip route 10.0.0.5 255.255.255.255 10.0.0.1
Router1(config)# end
Router1# write memory
```

### PC and Server Configuration (Packet Tracer GUI)

| Device  | IP Address    | Subnet Mask     | Default Gateway |
|---------|---------------|-----------------|-----------------|
| PC0     | 192.168.1.2   | 255.255.255.0   | 192.168.1.1     |
| PC1     | 192.168.1.3   | 255.255.255.0   | 192.168.1.1     |
| PC2     | 192.168.2.2   | 255.255.255.0   | 192.168.2.1     |
| PC3     | 192.168.2.3   | 255.255.255.0   | 192.168.2.1     |
| PC4     | 192.168.2.4   | 255.255.255.0   | 192.168.2.1     |
| Server0 | 192.168.2.10  | 255.255.255.0   | 192.168.2.1     |

> Switches require no configuration in Packet Tracer.

---

## Verification

### Check NAT translation table (Router0)

```
Router0# show ip nat translations

Pro  Inside global  Inside local   Outside local  Outside global
---  10.0.0.5       192.168.1.2    ---            ---
```

This confirms the static binding between PC0's private IP and its public IP.

### Check routing tables

```
Router0# show ip route
! Expected entries:
! C  10.0.0.0/30 via Serial0/1/0
! C  192.168.1.0/24 via GigabitEthernet0/0/0
! S  192.168.2.0/24 via 10.0.0.2

Router1# show ip route
! Expected entries:
! C  10.0.0.0/30 via Serial0/1/0
! S  10.0.0.5/32 via 10.0.0.1      <-- host route for NAT return traffic
! S  192.168.1.0/24 via 10.0.0.1
! C  192.168.2.0/24 via GigabitEthernet0/0/0
```

---

## Testing and Results

### Test 1 — PC0 pings Server0
```
C:\> ping 192.168.2.10
Reply from 192.168.2.10: bytes=32 time=1ms TTL=126   ✔ SUCCESS
```
Full end-to-end path works. NAT translates PC0's source IP from `192.168.1.2` to `10.0.0.5` before forwarding across the WAN.

### Test 2 — Server0 pings PC0's private IP
```
C:\> ping 192.168.1.2
Request timed out.                                    ✖ EXPECTED FAILURE
```
The private IP `192.168.1.2` is not routable from outside the LAN. Router1 has no route to the `192.168.1.0/24` subnet from Server0's perspective — and this is by design.

### Test 3 — Server0 pings PC0's public NAT IP
```
C:\> ping 10.0.0.5
Reply from 10.0.0.5: bytes=32 time=2ms TTL=126       ✔ SUCCESS
```
Server0 can reach PC0 only via its mapped public IP. Router0 intercepts the packet, translates the destination from `10.0.0.5` to `192.168.1.2`, forwards it to PC0, and performs reverse translation on the reply.

---

## Troubleshooting Encountered

| Issue | Cause | Fix |
|-------|-------|-----|
| `%Invalid interface type and number` on `FastEthernet0/0` | ISR4331 uses GigabitEthernet, not FastEthernet | Used `GigabitEthernet0/0/0` |
| `%Invalid interface type and number` on `Serial0/0/0` | ISR4331 serial port is `Serial0/1/0` | Used `Serial0/1/0` |
| `clock rate` rejected on Router0 | Router0 was on the DTE end of the serial cable | Moved `clock rate 64000` to Router1 (DCE side) |
| Ping from PC0 to Server0 timed out despite correct routes | Missing host route `10.0.0.5/32` on Router1 | Added `ip route 10.0.0.5 255.255.255.255 10.0.0.1` on Router1 so return packets could be forwarded |
| Serial interface `down/down` | `no shutdown` not yet applied or cable not connected | Verified cable connection and re-ran `no shutdown` |

---

## Key Inference

Pinging the **private IP (192.168.1.2) from Server0 fails** because private RFC 1918 addresses are not routable on public networks. Router1 has no knowledge of the `192.168.1.0/24` subnet from the outside — this is the fundamental security property of NAT. The inside network remains hidden; only the mapped public IP (`10.0.0.5`) is reachable from external hosts.

The **host route (`10.0.0.5/32`) on Router1** was a critical step that is easy to miss. Without it, outgoing packets from PC0 would reach Server0 successfully (since NAT translates the source), but the return packets would have no route back to `10.0.0.5` and would be dropped, causing a one-way communication failure seen as timeouts.
