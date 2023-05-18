Checkpoint 2 Writeup
====================

My name: Yuxin Pei

My SUNet ID: yuxinp

I collaborated with: xyz99

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about [8] hours to do. I did attend the lab session.

Program Structure and Design of the TCPReceiver and wrap/unwrap routines:
    Given checkpoint = X * (2^32) + Y and the raw value raw, there are 3 possible values (X + raw + MAX32, X + raw, X + raw - MAX32). I compared their differences and chose the closest.

    I kept extra parameter re_index to keep track of the current acked index. str_bytes to track the current index of data that needs to be insert into the reassembler.
    

Implementation Challenges:
[]

Remaining Bugs:
none

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]

- Optional: I made an extra test I think will be helpful in catching bugs: [describe where to find]
