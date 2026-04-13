# Lab 12(b): Configuring VoIP using Packet Tracer

## Aim
Configure VoIP and make a call from IP Phone0 to IP Phone1 using a Cisco router and switch.

---

## Topology

IP Phone0 ── Switch ── Router ── Switch ── IP Phone1

---

## Equipment Required
- 1 × Cisco Router  
- 1 × Switch  
- 2 × IP Phones  

---

## Procedure

### Step 1: Connect Devices
- Connect IP Phones to switch  
- Connect switch to router  

---

### Step 2: Configure Router

```
enable
configure terminal

interface fa0/0
ip address 192.168.1.1 255.255.255.0
no shutdown

telephony-service
max-ephones 2
max-dn 2
ip source-address 192.168.1.1 port 2000

ephone-dn 1
number 1001

ephone-dn 2
number 1002
```

---

### Step 3: Configure Phones
- IP Phone0 → number 1001  
- IP Phone1 → number 1002  

---

## Verification

Dial from IP Phone0:
```
1002
```

Expected:
- Call connects successfully  

---

## Result

VoIP was successfully configured and calls were established.

---

## Key Learning
- VoIP uses IP networks for voice communication  
- Routers can act as call managers  
