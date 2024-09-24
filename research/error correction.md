## Notes on error detection/correction codes.
* This research stems from the paper ["Error detecting and error correction codes"](https://vtda.org/pubs/BSTJ/vol29-1950/articles/bstj29-2-147.pdf)

## Summary
* Sometimes machines mess up, and need a form of correction to make sure a binary stream of bits are all correct. We can do this by using _systematic codes_. If a message has n binary digits where _m_ of those are the information to be conveyed, then there are _k=n-m_ bits used for error correction. 
* One example would be a single error detecting code, where after n-1 digits of information, the nth digit is either a 0 or 1 such that there are an even number of 1's. This way, if the total number of 1's ends up being odd, there has been some sort of data reading error.
