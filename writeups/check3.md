Checkpoint 3 Writeup
====================

My name: Yuxin Pei

My SUNet ID: yuxinp

I collaborated with: xyz99

I would like to thank/reward these classmates for their help: [list sunetids here]

This checkpoint took me about 10 hours to do. I did attend the lab session.

Program Structure and Design of the TCPSender:
I kept two message priority queues (new messages and outstanding messages). The min priority queue is orered by key sequence number, such that each time the message with the smallest sequence number is popped.

The retransmission timer is implemented with an integer. -1 means the timer is stopped, positive number signifies the time the timer is started. When tick is called, we can compare the time elapsed since the retransmission timer is set to see if needed to retransmit.



Implementation Challenges:
[]

Remaining Bugs:
none

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
