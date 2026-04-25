# Lab 12 — Wireless Topology Setup (Packet Tracer)

## Step 1 — Wireless Security

1. Select the Wireless from Network Devices and then bottom row has multiple options select apropriate from there
2. In the router GUI, go to **Wireless → Wireless Security**
3. Set **Security Mode** to `Disabled`
4. Click **Save Settings**

---

## Step 2 — Configure Each PC to Use DHCP

Repeat for **PC0, PC1, and PC2**:

1. Replace Ethernet Module with WMP 3000N Module in Physical of Computer
2. Click the PC → **Desktop** tab → **IP Configuration**
3. Select **DHCP**
4. Wait a few seconds — the PC should automatically receive an IP address in the range `192.168.0.100` to `192.168.0.150`

---

## Step 3 — Verify Wireless Connection

For each PC:

1. Click the PC → **Desktop** tab → **PC Wireless**
2. Confirm the SSID `Default` is listed and status shows **Connected**
3. If not connected, click **Refresh** — the PC should auto-associate since security is disabled

---

## Step 4 — Test Connectivity

1. Click any PC → **Desktop** tab → **Command Prompt**
2. Ping another PC using its DHCP-assigned IP:

```
ping 192.168.0.101
```

3. You should receive replies — this confirms the wireless topology is working correctly

---

## Step 5 — Save Your Work

Press **Ctrl + S** to save your Packet Tracer file.

---

## Summary of Lab Configuration

| Setting             | Value                        |
|---------------------|------------------------------|
| Router IP           | 192.168.0.1                  |
| Subnet Mask         | 255.255.255.0                |
| DHCP Start IP       | 192.168.0.100                |
| DHCP Max Users      | 50 (pool: .100 to .150)      |
| SSID                | Default                      |
| Security            | Disabled                     |
| Network Mode        | Mixed                        |
| Topology Mode       | Infrastructure               |
| Wireless Module     | WMP300N (on each PC)         |
