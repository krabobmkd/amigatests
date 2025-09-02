
create a sound mixing thread with AHI.
It's stereo 16b signed at the moment.
you send write() on the main thread/
the nice thing is:
 if write() streaming is too slow, sound will keep continuity
 if write are given too fast, sound will be mised to look good.
 It uses a round buffer internally and detect cases.
 
 