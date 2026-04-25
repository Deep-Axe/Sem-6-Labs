# Lab 12 — Static IP & WPA Configuration (Packet Tracer)

## (a) Configure Static IP on PC and Wireless Router

**On the Router:**

1. Router GUI → **Setup → Basic Setup**
2. Under **Network Setup**, set Router IP to `10.0.0.1`
3. Under **DHCP Server Settings** → set to **Disabled** (since we're using static IPs now)
4. Click **Save Settings**

**On each PC:**

1. Click the PC → **Desktop** tab → **IP Configuration**
2. Select **Static**
3. Fill in the IP details manually (see part c below)

---

## (b) Change SSID to MotherNetwork

1. Router GUI → **Wireless → Basic Wireless Settings**
2. Change the SSID field from `Default` to `MotherNetwork`
3. Click **Save Settings**

---

## (c) Set Static IP Addresses

**Router:**

| Field       | Value           |
|-------------|-----------------|
| Router IP   | `10.0.0.1`      |
| Subnet Mask | `255.255.255.0` |

GUI → Setup → Basic Setup → Router IP → Save Settings

---

**PC0:**

| Field           | Value           |
|-----------------|-----------------|
| IP Address      | `10.0.0.2`      |
| Subnet Mask     | `255.255.255.0` |
| Default Gateway | `10.0.0.1`      |

---

**PC1:**

| Field           | Value           |
|-----------------|-----------------|
| IP Address      | `10.0.0.3`      |
| Subnet Mask     | `255.255.255.0` |
| Default Gateway | `10.0.0.1`      |

---

**PC2:**

| Field           | Value           |
|-----------------|-----------------|
| IP Address      | `10.0.0.4`      |
| Subnet Mask     | `255.255.255.0` |
| Default Gateway | `10.0.0.1`      |

---

## (d) Secure Network with WPA Key on Router

1. Router GUI → **Wireless → Wireless Security**
2. Set **Security Mode** to `WPA2 Personal`
3. Set **Encryption** to `AES`
4. Set **Passphrase** to a key of your choice, e.g. `Manipal123`
5. Click **Save Settings**

---

## (e) Connect PCs Using WPA Key

Repeat for **PC0, PC1, and PC2**:

1. Click the PC → **Desktop** tab → **PC Wireless**
2. Go to the **Connect** tab → click **Refresh**
3. Select `MotherNetwork` from the list → click **Connect**
4. Enter the WPA passphrase you set (e.g. `Manipal123`)
5. Click **Connect** — status should show **Connected**

**Verify with a ping:**

```
ping 10.0.0.3
```

You should get replies confirming everything works.

---

## Summary of Configuration

| Setting         | Value           |
|-----------------|-----------------|
| Router IP       | `10.0.0.1`      |
| PC0 IP          | `10.0.0.2`      |
| PC1 IP          | `10.0.0.3`      |
| PC2 IP          | `10.0.0.4`      |
| Subnet Mask     | `255.255.255.0` |
| Default Gateway | `10.0.0.1`      |
| SSID            | `MotherNetwork` |
| Security Mode   | WPA2 Personal   |
| Encryption      | AES             |
| Passphrase      | `Manipal123`    |
| DHCP            | Disabled        |

> **Note:** Always change the router IP and disable DHCP first, then set static IPs on the PCs, then change the SSID, then configure WPA security, and finally reconnect each PC using the WPA key.
