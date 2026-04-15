## Lab 12(b): Configuring VoIP using Packet Tracer

---

### Aim
Configure VoIP and make a call from IP Phone0 to IP Phone1 using a Cisco router and switch.

---

### Topology
```
  [IP Phone0]
       \
        \
      [Switch0] ── [Router0]
        /            2811
       /
  [IP Phone1]
```

---

### Apparatus
- Cisco Packet Tracer
- 1 × 2811 Router
- 1 × 2950-24 Switch
- 2 × Cisco 7960 IP Phones

---

### Procedure

**Step 1:** Place Router0 (2811), Switch0, IP Phone0 and IP Phone1 on workspace. Connect all with straight-through cables.

**Step 2:** Configure Router via CLI:

```
Router>enable
Router#configure terminal
Router(config)#hostname Router0

Router0(config)#ip dhcp pool VOICE
Router0(dhcp-config)#network 192.168.1.0 255.255.255.0
Router0(dhcp-config)#default-router 192.168.1.1
Router0(dhcp-config)#option 150 ip 192.168.1.1
Router0(dhcp-config)#exit

Router0(config)#ip dhcp excluded-address 192.168.1.1

Router0(config)#interface fastethernet 0/0
Router0(config-if)#ip address 192.168.1.1 255.255.255.0
Router0(config-if)#no shutdown
Router0(config-if)#exit

Router0(config)#telephony-service
Router0(config-telephony)#max-dn 5
Router0(config-telephony)#max-ephones 5
Router0(config-telephony)#ip source-address 192.168.1.1 port 2000
Router0(config-telephony)#auto assign 1 to 5
Router0(config-telephony)#exit

Router0(config)#ephone-dn 1
Router0(config-ephone-dn)#number 1001
Router0(config-ephone-dn)#exit

Router0(config)#ephone-dn 2
Router0(config-ephone-dn)#number 1002
Router0(config-ephone-dn)#exit

Router0#write memory
```

**Step 3:** Configure Switch voice VLAN:

Double Check Ethernet connections, since 0/1 would usually be put to router giving IP phone 0/2 and 0/3

```
Switch>enable
Switch#configure terminal

Switch(config)#interface fastethernet 0/1
Switch(config-if)#switchport mode access
Switch(config-if)#switchport voice vlan 1
Switch(config-if)#exit

Switch(config)#interface fastethernet 0/2
Switch(config-if)#switchport mode access
Switch(config-if)#switchport voice vlan 1
Switch(config-if)#exit
```

**Step 4:** Wait ~30 seconds for IP Phones to boot and register — they will auto-receive IPs via DHCP and display their assigned extensions:
- IP Phone0 displays: **1001**
- IP Phone1 displays: **1002**

---

### Making a Call

1. Click **IP Phone0**
2. Go to **GUI** tab — shows phone keypad
3. Pick up handset — click the phone icon
4. Dial **1002** using the keypad
5. IP Phone1 will ring
6. Click IP Phone1 → GUI → pick up to answer

---

### Output

```
IP Phone0 display: Calling 1002...
IP Phone1 display: Ringing... 1001 calling
After answer: Connected
```

---

### Result
VoIP was successfully configured using Cisco 2811 router as the call manager. IP Phone0 (ext. 1001) successfully called IP Phone1 (ext. 1002) confirming VoIP operation over the LAN.

---

### Observation
The 2811 router was configured as a Cisco Unified Communications Manager using the `telephony-service` command. DHCP pool with `option 150` was set to point phones to the router as the TFTP/call manager source. Two ephone-dn entries were created assigning extensions 1001 and 1002. The switch ports were configured with voice VLAN 1. Both IP phones registered automatically and a successful call was established between them, demonstrating basic VoIP functionality over a LAN.
