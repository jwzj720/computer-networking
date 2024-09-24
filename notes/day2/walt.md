# Day 2
## Layers
### Physical Layer
* The actual wire 
* Photons
### Link Layer
### Network Layer 
### Transport Layer
* "fix" some things about the network
### Application Layer
* The protocals for things like sending "chat messages"
* This should be easy - bundle some data, maybe print some stuff out
* Real time chat?

## Goals
* 6 units communicating 
* Real time chat application layer
* Stretch goal, be able to transfer files in the background 

## Research Topics
* Circut switching
* Packet switching 
* NTP (network time protocol)
    * Our computers all do this in the background
    * "Stratum of time servers"
        * Layers of this - they all stem from "atomic" time
        * Every once in a while, they will fetch from the layer above them
            * Higher layer = more precise
* How do we deal with clock drift?
    * I think the easiest thing would be to start every message with the time?
    * Also we can look into parellel transmission 
