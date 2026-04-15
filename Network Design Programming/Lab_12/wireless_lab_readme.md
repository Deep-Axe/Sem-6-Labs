# Lab 12: Wireless Network Configuration
### Cisco Packet Tracer — Linksys WRT300N + 3 PCs

---

## Aim
Configure a wireless network with 3 PCs connected to a Linksys WRT300N wireless router with static IPs, custom SSID, and WAP security.

---

## Apparatus
| Item | Quantity |
|---|---|
| Cisco Packet Tracer | 1 |
| Linksys-WRT300N Wireless Router | 1 |
| PC-PT | 3 |

---

## Topology
```
   PC0          PC1          PC2
10.0.0.2     10.0.0.3     10.0.0.4
    \             |             /
     \            |            /
      \      (wireless)       /
       \          |          /
        \         |         /
         [Linksys WRT300N]
           Wireless Router0
             IP: 10.0.0.1
          SSID: MotherNetwork
         Security: WPA Personal
```

---

## Network Configuration Table
| Device | IP Address | Subnet Mask | Gateway |
|---|---|---|---|
| Router0 | 10.0.0.1 | 255.255.255.0 | — |
| PC0 | 10.0.0.2 | 255.255.255.0 | 10.0.0.1 |
| PC1 | 10.0.0.3 | 255.255.255.0 | 10.0.0.1 |
| PC2 | 10.0.0.4 | 255.255.255.0 | 10.0.0.1 |

---

## Wireless Settings
| Parameter | Value |
|---|---|
| SSID | MotherNetwork |
| Security Mode | WPA Personal |
| Passphrase | Manipal@123 |
| Mode | Infrastructure |
| DHCP | Disabled (Static IPs) |
| Router Login | admin / admin |

---

## Step-by-Step Procedure

### Step 1 — Place Devices
- Drag **Linksys-WRT300N** from Wireless Devices onto workspace
- Drag **3 × PC-PT** from End Devices onto workspace
- Label them PC0, PC1, PC2

---

### Step 2 — Install Wireless Module on Each PC
> Repeat for PC0, PC1, PC2

1. Click PC → **Physical** tab
2. Click **Power button** to turn PC OFF
3. Drag out the existing **FastEthernet (wired)** module from its slot
4. From module list, drag **WMP300N** into the empty slot
5. Click **Power button** to turn PC ON
6. Close window

---

### Step 3 — Configure Router IP and Disable DHCP
1. Click **Router0** → **GUI** tab
2. Login: username `admin`, password `admin`
3. Go to **Setup** → **Basic Setup**
4. Under **Router IP** set:
   - IP Address: `10.0.0.1`
   - Subnet Mask: `255.255.255.0`
5. Under **DHCP Server** → select **Disabled**
6. Click **Save Settings**

---

### Step 4 — Change SSID
1. Still in Router GUI → go to **Wireless** tab
2. Click **Basic Wireless Settings**
3. Change **Network Name (SSID)** from `Default` to `MotherNetwork`
4. Click **Save Settings**

---

### Step 5 — Configure WAP Security
1. Still in Router GUI → **Wireless** tab
2. Click **Wireless Security**
3. Set **Security Mode** → `WPA Personal`
4. Set **Encryption** → `AES`
5. Set **Passphrase** → `Manipal@123`
6. Click **Save Settings**
7. Close router window

---

### Step 6 — Set Static IP on PC0
1. Click **PC0** → **Desktop** → **IP Configuration**
2. Select **Static**
3. Enter:
   - IP: `10.0.0.2`
   - Mask: `255.255.255.0`
   - Gateway: `10.0.0.1`

---

### Step 7 — Connect PC0 to Wireless Network
1. Click **PC0** → **Desktop** → **PC Wireless**
2. Go to **Connect** tab
3. Click **Refresh**
4. Select **MotherNetwork** from list
5. Click **Connect**
6. Enter passphrase: `Manipal@123`
7. Click **Connect**
8. Verify under **Link Information** → status shows Connected

---

### Step 8 — Repeat for PC1
- IP: `10.0.0.3` | Mask: `255.255.255.0` | Gateway: `10.0.0.1`
- Connect via PC Wireless → MotherNetwork → passphrase: `Manipal@123`

---

### Step 9 — Repeat for PC2
- IP: `10.0.0.4` | Mask: `255.255.255.0` | Gateway: `10.0.0.1`
- Connect via PC Wireless → MotherNetwork → passphrase: `Manipal@123`

---

## Verification — Ping Tests

From **PC0 → Desktop → Command Prompt**:

```
ping 10.0.0.1
```
Expected output:
```
Pinging 10.0.0.1 with 32 bytes of data:
Reply from 10.0.0.1: bytes=32 time=1ms TTL=128
Reply from 10.0.0.1: bytes=32 time=1ms TTL=128
Reply from 10.0.0.1: bytes=32 time=1ms TTL=128
Reply from 10.0.0.1: bytes=32 time=1ms TTL=128
Packets: Sent=4, Received=4, Lost=0 (0% loss)
```

```
ping 10.0.0.3
```
Expected output:
```
Pinging 10.0.0.3 with 32 bytes of data:
Reply from 10.0.0.3: bytes=32 time=1ms TTL=128
Reply from 10.0.0.3: bytes=32 time=1ms TTL=128
Reply from 10.0.0.3: bytes=32 time=1ms TTL=128
Reply from 10.0.0.3: bytes=32 time=1ms TTL=128
Packets: Sent=4, Received=4, Lost=0 (0% loss)
```

```
ping 10.0.0.4
```
Expected output:
```
Pinging 10.0.0.4 with 32 bytes of data:
Reply from 10.0.0.4: bytes=32 time=1ms TTL=128
Reply from 10.0.0.4: bytes=32 time=1ms TTL=128
Reply from 10.0.0.4: bytes=32 time=1ms TTL=128
Reply from 10.0.0.4: bytes=32 time=1ms TTL=128
Packets: Sent=4, Received=4, Lost=0 (0% loss)
```

---

## Link Information (PC Wireless Tab)
```
Connection Status : Connected
SSID              : MotherNetwork
IP Address        : 10.0.0.2
Signal Strength   : Excellent
Link Quality      : Excellent
Security          : WPA Personal
```

---

## Result
Wireless network configured successfully. All 3 PCs installed with WMP300N modules connected to MotherNetwork using WPA passphrase. Static IPs assigned manually. Ping tests between all devices returned 0% packet loss confirming full wireless connectivity.

---

## Observation
The Linksys WRT300N was configured via its GUI (admin/admin). DHCP was disabled and static IPs were assigned. SSID was changed from Default to MotherNetwork. WPA Personal security was enabled with passphrase Manipal@123. Each PC had its wired NIC replaced with a WMP300N wireless module. All three PCs successfully associated with the router using the WAP key. Ping tests confirmed successful communication between all PCs through the wireless router, validating correct configuration of the wireless network with security enabled.

---
