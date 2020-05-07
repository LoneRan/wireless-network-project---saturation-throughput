# wireless-network-project---saturation-throughput

Introduction

IEEE 802.11 networks utilize CSMA/CA with binary exponential backoﬀ to avoid collisions. This mechanism is referred to as distributed coordinated function or DCF. For this project, we aim to validate the analytical model presented by Bianchi [1] for computing the saturation throughput performance of the 802.11 protocol.

Simulation

You will use two tools to simulate the performance of 802.11, namely NS-3 and MATLAB. The goal is to generate plots similar to Figure 4 from Bianchi’s paper [1]. For this project, we will focus on the basic mode of 802.11 (without RTS/CTS). You should conﬁgure your simulations to match Biachi’s setup as closely as possible. Also, make sure to use appropriate values for W and m.

You can find bianchi's paper in this link: https://pdfs.semanticscholar.org/4a5c/ f874e9469815113c7ea93ff97317bdb52a90.pdf, doi:10.1109/4234.736171

Implementation: 

Ns-3 simulation code file: ee597project.cc
OS: Ubuntu 18.04
NS-3 edition: 3.29

No need to edit source file.

Configuration options:
-nWifi: number of stations, default to be 10. You can configure it by type on terminal: ./waf --run "scrach/ee597proj --nWifi=20"
-cwmin: number of minimum contention window, default to be 31. You can configure it by type on terminal: ./waf --run "scrach/ee597proj --cwmin=31"
-cwmax: number of maximum contention window, default to be 255. You can configure it by type on terminal: ./waf --run "scrach/ee597proj --cwmax=255"

For example, you can type ./waf --run "scratch/ee597proj --nWifi=30 --cwmin=31 --cwmax=255" 

Output: For each station, the program should output the throughput monitored by flow monitor, total throughput and stations number and contention window size.

>>Matlab simulation
Tools: Matlab 2017
Matlab files: ee597project1.m, ee597project2.m

Output: The simulation file will output a plot which contains both three situation of W and m, which is 1)W=32 m=3; 2)W=32 m=5; 3)W=128 m=3.
