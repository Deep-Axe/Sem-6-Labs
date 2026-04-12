# NTP Lab Reference — Cisco Packet Tracer

## Overview

This lab configures a simple NTP (Network Time Protocol) setup where **Server0** acts as the NTP master (stratum 1) and **Router0** syncs its clock from it over UDP port 123.

**Topology:** Router0 ↔ Switch0 ↔ Server0

---

## Device Roles

| Device  | Role            | Stratum | Description                          |
|---------|-----------------|---------|--------------------------------------|
| Server0 | NTP Master      | 1       | Provides authoritative time          |
| Router0 | NTP Client      | 2       | Syncs its clock from Server0         |
| Switch0 | Layer 2 switch  | —       | Connects Router0 and Server0         |

---

## IP Addressing

| Device  | IP Address     | Subnet Mask     | Default Gateway |
|---------|----------------|-----------------|-----------------|
| Server0 | 192.168.1.10   | 255.255.255.0   | 192.168.1.1     |
| Router0 | 192.168.1.1    | 255.255.255.0   | —               |

---

## Step 1 — Configure Server0 (GUI)

1. Click **Server0** → **Services** → **NTP**
2. Toggle NTP: **On**
3. Set a time/date if prompted
4. Note the server IP: `192.168.1.10`

> Server0 in Packet Tracer acts as a stratum 1 source using its internal clock (`127.127.1.1`).

---

## Step 2 — Configure IP Addresses

### Server0 (Desktop → IP Configuration)

Set these values in the GUI:

```
IP Address   : 192.168.1.10
Subnet Mask  : 255.255.255.0
Gateway      : 192.168.1.1
```

### Router0 (CLI)

```
Router0# conf t
Router0(config)# interface GigabitEthernet0/0/0
Router0(config-if)# ip address 192.168.1.1 255.255.255.0
Router0(config-if)# no shutdown
Router0(config-if)# end
```

| Command | Purpose |
|---|---|
| `conf t` | Enter global configuration mode |
| `interface GigabitEthernet0/0/0` | Select the interface facing Switch0 |
| `ip address 192.168.1.1 255.255.255.0` | Assign IP and subnet mask |
| `no shutdown` | Bring the interface up (disabled by default) |
| `end` | Return to privileged EXEC mode |

---

## Step 3 — Configure Router0 as NTP Client

```
Router0# conf t
Router0(config)# ntp server 192.168.1.10
Router0(config)# clock timezone IST 5 30
Router0(config)# end
Router0# write memory
```

| Command | Purpose |
|---|---|
| `ntp server 192.168.1.10` | Point the router to Server0 as its NTP source |
| `clock timezone IST 5 30` | Set timezone to IST (UTC +5:30) — optional |
| `end` | Exit config mode |
| `write memory` | Save config to NVRAM (survives reboot) |

---

## Step 4 — Verification Commands

### `show ntp associations`

Shows all configured NTP peers and their sync status.

```
address         ref clock    st   when   poll   delay   offset   disp
*192.168.1.10   127.127.1.1  1    10     64     1.0     0.5      0.1
```

| Symbol | Meaning |
|--------|---------|
| `*`    | Currently selected/active NTP master |
| `+`    | Candidate peer (valid but not selected) |
| `-`    | Peer rejected by the NTP algorithm |

| Column | Meaning |
|--------|---------|
| `address` | IP of the NTP server |
| `ref clock` | Server0's own reference (`127.127.1.1` = local clock) |
| `st` | Stratum level of the server |
| `when` | Seconds since last packet received |
| `poll` | Poll interval in seconds |
| `delay` | Round-trip delay in milliseconds |
| `offset` | Time difference between router and server (ms) |
| `disp` | Dispersion — clock accuracy estimate |

---

### `show ntp status`

Shows whether the router's clock is synchronized.

```
Clock is synchronized, stratum 2, reference is 192.168.1.10
```

| Field | Meaning |
|-------|---------|
| `synchronized` | Clock is successfully synced to the NTP server |
| `stratum 2` | Router is one hop away from the stratum 1 source |
| `reference is 192.168.1.10` | The IP of the active NTP master |

> If it says `unsynchronized`, wait 1–2 minutes and run the command again. Packet Tracer can be slow to converge NTP.

---

### `show clock`

Displays the router's current clock value.

```
*15:30:00.000 IST Sat Apr 5 2025
```

The `*` prefix means the clock is **not yet authoritative** (still syncing). Once sync completes, it disappears.

---

## How NTP Works (Quick Reference)

```
Server0 (Stratum 1)
  Internal clock → 127.127.1.1
        │
        │  UDP 123
        ▼
Router0 (Stratum 2)
  Syncs from 192.168.1.10
```

- NTP uses **UDP port 123**
- Stratum indicates distance from a reference clock — lower = more accurate
- The router polls Server0 every `poll` seconds (default starts at 64s)
- After a few exchanges the router's offset converges and the clock is marked synchronized

---

## Common Issues

| Symptom | Likely Cause | Fix |
|---|---|---|
| `show ntp status` shows unsynchronized | Not enough time elapsed | Wait 1–2 min and retry |
| No `*` in `show ntp associations` | Wrong server IP configured | Re-check `ntp server` command |
| Interface down | `no shutdown` not run | `Router0(config-if)# no shutdown` |
| Config lost after reload | `write memory` not run | `Router0# write memory` |
