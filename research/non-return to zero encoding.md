## Notes on non-return to zero encoding.
* First, looking at the Wikipedia page [Non-return-to-zero](https://en.wikipedia.org/wiki/Non-return-to-zero#:~:text=Non%2Dreturn%2Dto%2Dzero%20%2D%20Wikipedia) we see that NRZ is where voltage present represents a 1, and no voltage represents a 0.
* Could also use RTZ (return to zero) encoding, which makes the issue of clocking much easier, but requires 2x bandwidth.
* "Since NRZ is not inherently a self-clocking signal, some additional synchronization technique must be used for avoiding bit slips; examples of such techniques are a run-length-limited constraint and a parallel synchronization signal."
* [Run-length limited](https://en.wikipedia.org/wiki/Run-length_limited) or RLL coding is a line coding technique that is used to send arbitrary data over a communications channel with bandwidth limits

* with Manchester encoding, you can detect energy level changes that occur a fixed time period apart. You can read this encoding by listening for every change in voltage, and then when received reset the waiting period for the next bit. That way, you never drift from the clock rate of supplied bits.