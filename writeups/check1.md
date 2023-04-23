Checkpoint 1 Writeup
====================

My name: Yuxin Pei

My SUNet ID: yuxinp

I collaborated with: Xiyu Zhang

I would like to thank/reward these classmates for their help: [list sunetids here]

This lab took me about 5 hours to do. I [did/did not] attend the lab session.

Program Structure and Design of the Reassembler:
I kept a vector of intervals with substrings, each time a bytestream comes, the interval and substring is merged into the vector. Each time a bytestream arrives and the start index of the first merged interval is the current offset, send all bytestreams.

Implementation Challenges:
The merged interval algorithm and how to truncate strings and merge strings.

Remaining Bugs:
too slow

- Optional: I had unexpected difficulty with: [describe]

- Optional: I think you could make this lab better by: [describe]

- Optional: I was surprised by: [describe]

- Optional: I'm not sure about: [describe]
