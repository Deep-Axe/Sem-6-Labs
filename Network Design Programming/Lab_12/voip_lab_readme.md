# Lab 12(b) — VoIP Configuration (Packet Tracer)

---

## Aim

Configure VoIP using a Cisco 2811 router as the call manager and make a call from IP Phone0 (ext. 1001) to IP Phone1 (ext. 1002) over a LAN.

---

## Topology Diagram

```
                          +----------------+
                          |  IP Phone0     |
                          |  Cisco 7960    |
                          |  ext. 1001     |
                          +-------+--------+
                                  |
                                  | Straight-Through
                                  | Switch Fa0/2 → Phone SW port
                                  |
+-------------------+     +-------+--------+
|   Router0         |     |   Switch0      |
|   Cisco 2811      |     |   Cisco 2950-24|
|                   |     |                |
|   Fa0/0           +-----+   Fa0/1        |
|   192.168.1.1     |     |                |
|   DHCP Server     |     |   Fa0/3        |
|   Call Manager    |     +-------+--------+
|   TFTP (opt 150)  |             |
+-------------------+             | Straight-Through
                                  | Switch Fa0/3 → Phone SW port
                                  |
                          +-------+--------+
                          |  IP Phone1     |
                          |  Cisco 7960    |
                          |  ext. 1002     |
                          +----------------+

Cable type used throughout: Copper Straight-Through
```

---

## Apparatus

| Device   | Model         | Quantity | Where to find in Packet Tracer       |
| -------- | ------------- | -------- | ------------------------------------ |
| Router   | Cisco 2811    | 1        | Network Devices → Routers → 2811     |
| Switch   | Cisco 2950-24 | 1        | Network Devices → Switches → 2950-24 |
| IP Phone | Cisco 7960    | 2        | End Devices → IP Phones → 7960       |

---

## Cable Connections

> Always use **Copper Straight-Through** cable for all connections here.
> The IP Phone has two ports — **SW** (connects to switch) and **PC** (connects to a computer). Always use the **SW port** when connecting to the switch.

| From Device | From Port | To Device | To Port |
| ----------- | --------- | --------- | ------- |
| Router0     | Fa0/0     | Switch0   | Fa0/1   |
| Switch0     | Fa0/2     | IP Phone0 | SW      |
| Switch0     | Fa0/3     | IP Phone1 | SW      |

---

## Step 1 — Place and Connect Devices

1. Open Packet Tracer and go to an empty area of the canvas (away from your existing wireless topology if present)
2. Drag in the devices from the bottom panel as listed in the apparatus table above
3. Name them **Router0, Switch0, IP Phone0, IP Phone1** to avoid confusion with other devices
4. Connect them using **Copper Straight-Through** cables as per the cable table above

---

## Step 2 — Configure Router0 via CLI

Click **Router0 → CLI tab**. Press Enter if prompted, then enter the following:

### Set hostname

```
Router>enable
Router#configure terminal
Router(config)#hostname Router0
```

### Configure DHCP pool

This creates a DHCP pool called VOICE that will hand out IPs in the 192.168.1.0/24 range.
`option 150` is critical — it tells the IP phones where the call manager (TFTP server) is, which is the router itself at 192.168.1.1.

```
Router0(config)#ip dhcp pool VOICE
Router0(dhcp-config)#network 192.168.1.0 255.255.255.0
Router0(dhcp-config)#default-router 192.168.1.1
Router0(dhcp-config)#option 150 ip 192.168.1.1
Router0(dhcp-config)#exit
```

### Exclude router IP from DHCP

This prevents the router from accidentally assigning its own IP (192.168.1.1) to a phone.

```
Router0(config)#ip dhcp excluded-address 192.168.1.1
```

### Configure the router's Fa0/0 interface

This brings up the interface and gives it the gateway IP that the phones will use.

```
Router0(config)#interface fastethernet 0/0
Router0(config-if)#ip address 192.168.1.1 255.255.255.0
Router0(config-if)#no shutdown
Router0(config-if)#exit
```

### Configure telephony service

This turns the router into a basic Cisco Unified Communications Manager (call manager).

- `max-dn 5` — allows up to 5 directory numbers (extensions)
- `max-ephones 5` — allows up to 5 IP phones to register
- `ip source-address 192.168.1.1 port 2000` — tells phones to register to this IP on port 2000
- `auto assign 1 to 5` — automatically assigns ephone-dn entries to phones as they register

```
Router0(config)#telephony-service
Router0(config-telephony)#max-dn 5
Router0(config-telephony)#max-ephones 5
Router0(config-telephony)#ip source-address 192.168.1.1 port 2000
Router0(config-telephony)#auto assign 1 to 5
Router0(config-telephony)#exit
```

### Assign extensions to phones

`ephone-dn` stands for ephone directory number — each one is an extension.

- ephone-dn 1 → extension 1001 (will be assigned to IP Phone0)
- ephone-dn 2 → extension 1002 (will be assigned to IP Phone1)

```
Router0(config)#ephone-dn 1
Router0(config-ephone-dn)#number 1001
Router0(config-ephone-dn)#exit

Router0(config)#ephone-dn 2
Router0(config-ephone-dn)#number 1002
Router0(config-ephone-dn)#exit
```

### Save the configuration

```
Router0(config)#exit
Router0#write memory
```

---

## Step 3 — Configure Switch0 via CLI

Click **Switch0 → CLI tab**. The switch ports need to be set to access mode with voice VLAN 1 so that VoIP traffic is handled correctly.

```
Switch>enable
Switch#configure terminal
```

### Port Fa0/1 — facing Router0

```
Switch(config)#interface fastethernet 0/1
Switch(config-if)#switchport mode access
Switch(config-if)#switchport voice vlan 1
Switch(config-if)#exit
```

### Port Fa0/2 — facing IP Phone0

```
Switch(config)#interface fastethernet 0/2
Switch(config-if)#switchport mode access
Switch(config-if)#switchport voice vlan 1
Switch(config-if)#exit
```

### Port Fa0/3 — facing IP Phone1

```
Switch(config)#interface fastethernet 0/3
Switch(config-if)#switchport mode access
Switch(config-if)#switchport voice vlan 1
Switch(config-if)#exit
```

---

## Step 4 — Wait for Phones to Register

- Wait **30–60 seconds** in Packet Tracer for the phones to boot
- Each phone will automatically:
  1. Pull an IP address from the DHCP pool (e.g. 192.168.1.2, 192.168.1.3)
  2. Use option 150 to find the call manager at 192.168.1.1
  3. Register with the telephony service
  4. Display its assigned extension on screen

**Expected result:**

- IP Phone0 display shows: **1001**
- IP Phone1 display shows: **1002**

> **If phones don't register after 60 seconds**, go to Router0 CLI and run:
>
> ```
> Router0#show ephone
> ```
>
> This shows which phones have registered. If none appear, double check your cable connections and that `no shutdown` was run on Fa0/0.

---

## Step 5 — Make a Call from IP Phone0 to IP Phone1

1. Click **IP Phone0 → GUI tab** — you will see the phone keypad
2. Click the **handset icon** on the left to pick up the line
3. Dial **1 0 0 2** using the on-screen keypad buttons one at a time
4. IP Phone1 will begin ringing
5. Click **IP Phone1 → GUI tab**
6. Click the **handset icon** to answer the call
7. The call is now connected ✓

---

## Expected Output

```
IP Phone0 display:   Calling... 1002
IP Phone1 display:   Ringing... 1001 calling
After answer:        Connected
```

---

## Troubleshooting

| Problem                   | Likely cause                     | Fix                                                 |
| ------------------------- | -------------------------------- | --------------------------------------------------- |
| Phone shows no extension  | Not registered with call manager | Check `option 150` is set and Fa0/0 is up           |
| Phone shows no IP         | DHCP not working                 | Check `ip dhcp pool VOICE` and excluded address     |
| Call doesn't connect      | ephone-dn not assigned           | Re-check `auto assign 1 to 5` and ephone-dn numbers |
| Ports not passing traffic | Switch VLAN not set              | Re-run `switchport voice vlan 1` on all ports       |

---

## Quick Reference Summary

| Item              | Value                       |
| ----------------- | --------------------------- |
| Router interface  | Fa0/0                       |
| Router IP         | `192.168.1.1`               |
| Subnet Mask       | `255.255.255.0`             |
| DHCP pool name    | `VOICE`                     |
| DHCP network      | `192.168.1.0 255.255.255.0` |
| TFTP option 150   | `192.168.1.1`               |
| Call manager port | `2000`                      |
| Max phones        | `5`                         |
| Max extensions    | `5`                         |
| Phone0 extension  | `1001`                      |
| Phone1 extension  | `1002`                      |
| Voice VLAN        | `1`                         |
| Cable type        | Copper Straight-Through     |

---

## Result

VoIP was successfully configured using the Cisco 2811 router as the call manager. IP Phone0 (ext. 1001) successfully called IP Phone1 (ext. 1002), confirming VoIP operation over the LAN.

---

## Observation

The 2811 router was configured as a basic Cisco Unified Communications Manager using the `telephony-service` command. A DHCP pool named VOICE was created for the 192.168.1.0/24 subnet, with `option 150` pointing phones to the router at 192.168.1.1 as the TFTP/call manager source. Two `ephone-dn` entries were created assigning extensions 1001 and 1002, and `auto assign` was used to automatically map them to registering phones. Switch ports were configured in access mode with `switchport voice vlan 1` to correctly handle VoIP traffic. Both IP phones registered automatically upon boot, received their extensions, and a successful call was established between them — demonstrating basic VoIP functionality over a LAN.
