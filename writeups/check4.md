Checkpoint 4 Writeup
====================

My name: Yuxin Pei

My SUNet ID: yuxinp

I collaborated with: xyz99

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 5 hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the NetworkInterface:
I kept 2 mappings (valid ip -> ethernet addresses | ip addr -> arp request) and 3 queues (ip address, queue time | arp rqst ip, queue time | msg)

Update mappings when datagram is sent / time ticks; the first 2 queues are kept such that the first msg popped is first in queue time, eabling faster updates.

Implementation Challenges:
Changed my design a couple times during implementation.

Remaining Bugs:
N/A

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
