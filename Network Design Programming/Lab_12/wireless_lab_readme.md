# Lab No. 12: Configuring Wireless Network using Packet Tracer

## Aim
To configure a wireless network with 3 PCs connected to a Linksys WRT300N wireless router, set static IPs, change SSID, configure security, and verify connectivity.

---

## Apparatus / Software Required
- Cisco Packet Tracer  
- 1 × Linksys WRT300N Wireless Router  
- 3 × PC-PT (with WMP300N wireless module)  

---

## Theory

A wireless network in infrastructure mode uses a central wireless router (access point).  
- SSID identifies the network  
- WPA provides security using a passkey  
- Router handles wireless access and routing  

---

## Topology

        PC0     PC1     PC2
          \      |      //
             Wireless Router

---

## Procedure

### Step 1: Configure Router
- Go to GUI tab  
- Change SSID (e.g., `MyWiFi`)  
- Enable WPA2 security  
- Set passphrase (e.g., `12345678`)  

---

### Step 2: Configure PCs
- Add wireless module (WMP300N)  
- Desktop → PC Wireless → Connect to SSID  
- Enter WPA password  

---

### Step 3: Assign IP Addresses (Static)

| Device | IP Address | Subnet Mask | Gateway |
|--------|-----------|-------------|---------|
| PC0 | 192.168.1.2 | 255.255.255.0 | 192.168.1.1 |
| PC1 | 192.168.1.3 | 255.255.255.0 | 192.168.1.1 |
| PC2 | 192.168.1.4 | 255.255.255.0 | 192.168.1.1 |

---

## Verification

From PC0:
```
ping 192.168.1.3
ping 192.168.1.4
```

Expected:
- Successful replies  
- 0% packet loss  

---

## Result

Wireless network was successfully configured.  
All PCs connected using SSID and communicated securely.

---

## Key Learning
- SSID identifies wireless network  
- WPA secures wireless communication  
- Devices connect via wireless NIC  
