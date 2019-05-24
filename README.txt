

          ===================================================
          = autower 2.0 (C) Christoph Langguth, 2006 - 2010 =
          ===================================================


This software is distributed under the terms of the GNU General Public
License, version 2. Please see LICENSE.txt for the full text of the
license.



INTRODUCTION
============

Have you ever been annoyed by the fact that when you want to see this perfect
touchdown you just made from an outside perspective, the tower is anywhere
but where you want it to be? Got frustrated by trying to get the tower where
you think it is in reality, or at least somewhere where it fits well?

If the answer is yes, then take a look at autower. Designed to operate in
the background without any user intervention, it automatically moves the tower
to the airport where you want it to be, to the spot where the tower sits in
reality (well, at least somewhere close).



FEATURES
========

autower has a single, well-defined purpose: it continuously scans your current
aircraft position and sets the tower to the most suitable airport close to it.
By default, it also takes into account your active COM radio frequency, locking
the tower to the airport you're currently approaching.

autower comes in two variants to choose from:

* an executable that you can run outside of Flight Simulator, or via WideFS
* a DLL that will be automatically loaded by FS.

Both variants share the exact same codebase, so from the functionality point
of view they are identical. Please see the installation notes below for further
discussion.

Nearly all of autower's features are further customizable (with reasonable
defaults, of course.)



KNOWN ISSUES AND LIMITATIONS
============================

* If FS is started in full screen mode when autower rebuilds its datafile,
the flight simulator window temporarily loses focus and the desktop is displayed.
It should be restored after autower finished building the datafile. If it's not,
just click (possibly twice) on the FS task menu entry and you will return to FS.

* autower relies on the information that Flight Simulator provides about the
airports. Since not all (actually only very few) airports include detailed
information about the tower position (and others do not have a tower at all),
in many cases autower has to make the best out of the few information that FS
does provide. It will, for example, have to determine by itself the height of
the tower, and in some cases it tries to adjust the position so that it does not
sit on a runway. This may turn out not to be an ideal situation for every
airport out there -- however, I believe that it does a relatively good job, and
if you're not satisfied, you can always tweak it by changing its configuration.



REQUIREMENTS
============

* Microsoft Flight Simulator 2004. autower ONLY works with FS2004, older and
  newer versions are NOT supported. By the way, I heard FSX has a similar
  functionality built in.
  
* FSUIPC >= 3.71 (unregistered is ok). autower will NOT work with unregistered
  versions of FSUIPC prior to 3.71. It should work with decently new registered
  versions, though. At the time of releasing version 2.0.0, the most recent
  version proven to be compatible was 3.98, however newer versions should
  normally work as well.



INSTALLATION AND CONFIGURATION
==============================

For those of you in a hurry, here it goes:

=> EXPRESS INSTALLATION
-----------------------

	* copy autower.dll to your flight simulator \modules directory.
	* run FS2004
	* That's it.



=> THOUGHTFUL INSTALLATION :-)
------------------------------

As mentioned above, autower comes in two flavors: a stand-alone executable and a
DLL. Ultimately, I guess most everyone will be using the DLL for everyday usage,
because one doesn't have to care about starting autower manually each time FS is
started, and because the black command line box is usually just disturbing.
Nevertheless, I recommend starting with the command-line version.

Here's why: the major advantage of the command-line version is that you can
start and stop it while FS is running. You might want to do this in order to
test out various configurations to find the one that suits your taste. You can
also have the command-line version display more verbose information, by
adjusting the log level, in case you want or need to do so. Although you can
adjust the log level in the same way for the DLL, I *strongly* recommend against
it. The simple reason is that the DLL pops up a message box whenever some log
message is to be displayed to the user -- you will very quickly realize that
it's not fun to click away thousands of message boxes... you have been warned ;-)

So, the recommended way is the following:

Start the command-line application (while FS is running and ready to fly, i.e.,
not paused in any menu or in the game itself) and change to tower view. Go to
different airports and see if you like how it behaves. If it does, perfect --
just install the DLL and forget about the command-line version. If it doesn't,
take a look at the autower.ini parameters (they're all thoroughly documented)
and fiddle with them. This is where the executable version has its strength --
just restart it after changing the configuration, and it will use the new
settings. To achieve the same results with the DLL variant, you'd have to
restart FS. Once you have found the settings you like, just copy the DLL along
with your modified configuration file (autower.ini) to the FS9\modules folder.

Note that both the DLL and the exe will run without autower.ini being present,
but then you will be bound to the default settings.



QUESTIONS AND ANSWERS
=====================

Q: What are these autower.dat and autower.ini files?
A: autower.ini is the configuration file. autower.dat -- the datafile --
   contains all the information that autower needs about the airports. The
   datafile is reconstructed from the existing scenery whenever it is missing,
   or when your scenery has changed.
   
Q: Where are these files located?
A: For the executable variant, in the current working directory. For the DLL,
   in the FS9\modules folder (same folder that autower.dll resides in.)

Q: Can I re-use the configuration/data files between the DLL and the exe?
A: Of course! Just pay attention to the loglevel when using the DLL...

Q: Can I run the DLL and the executable at the same time?
A: This means: Can I run the executable while the DLL is loaded in Flight
   Simulator? Yes, in principle you can; but since both don't know about each
   other at runtime, they might be playing ping-pong with the tower positions
   they set. While that scenario usually doesn't make sense anyway, nothing
   tragic should happen.
   
Q: I have updated my scenery to include new AFCADs. How does autower handle
   this?
A: Since version 2, autower detects changes to your scenery (once FS
   was restarted and rebuilt the indexes) and rebuilds its database by itself.

Q: What if I'm running the DLL version, but don't have administrator rights so
   I can't write to the FS directory?
A: If your scenery doesn't change and you have an up-to-date datafile, no
   problem at all. If the datafile doesn't exist, or if it's not up-to-date
   anymore, autower will try to recreate it at every startup, failing to do so
   because it can't get write access. However, this is an error that is handled
   gracefully, and autower will still work. It will simply take longer to start,
   because it needs to scan for the airports every time, instead of being able
   to use its own index.

Q: autower seems to do something wrong for a particular airport. Is there a way
   to find out why this is happening?
A: Yes. You have to use the executable version for this. Start it, passing the
   ICAO code for the airport that you want to analyze as parameter, for example
   "autower lfsb". It will re-scan your scenery, and while doing so, will
   display any information relevant to that airport. Note that some poorly
   designed AFCADs may result in wrong information, but autower has no way to
   detect that this is not intended and thus obeys it. However, this mechanism
   should at least give you the possibility to identify faulty AFCADs.

Q: I can't seem to find a configuration that I like - what should I do?
Q: I'm missing feature xyz. What should I do?
A: Hmmm. Although autower tries to make some intelligent decisions, it is of
   course limited to the information it gets from FS, and to the logic that I
   programmed. If you have some programming skills, you may just get the source
   code, take a look at it, and modify it to suit your needs. Needless to say,
   if you make some improvements to the code, I'd be delighted to hear about it.
   Oh, please adhere to the license if you plan to redistribute your changes.

   If you're not a programmer, you can just contact me, and we'll probably
   manage to figure something out. Just please bear in mind the intention of the
   program -- it has a very specific purpose, and I don't want to deviate too
   much from it. There's already plenty of programs out there which try to do
   everything, and autower should not become one of these.

Q: Back to programming: where's the source?
A: It's available at autower's homepage -- see the address at the end of this
   file.

Q: What tools did you use? What do I need to compile it?
A: Since version 2, autower is developed using Eclipse CDT and MinGW32. All of
   these tools are available for free and open-source. The source archive
   contains instructions on how to set up the build environment.
   
Q: Why GPL?
A: Philosophical question, eh? Quite simply, because I believe that knowledge
   and information, for the benefit of all of us, should be shared, not hidden.

Q: I have some other problem, or a question which is not covered here.
A: Please check the homepage first, maybe your issue has been addressed already.
   Otherwise, feel free to contact me! Just be precise, please. Of course, any
   other feedback is welcome -- preferrably positive feedback, though ;-)

Q: Is it really called autower, or did you just misspell autotower?
A: Even though -- by now -- I get roughly the same amount of hits for the latter
   as for the former, it initially was a conscient decision to "pull together"
   the common suffix and prefix. I have no clue why, but in the end it's fun -
   I mean, even I don't have an idea about how to correctly pronounce it. If you
   can't convince, confuse ;-)



VERSION HISTORY
===============

1.0 (Nov 28, 2006) - first public release
1.01 (Nov 28, 2006) - fixed a nasty bug that caused the command-line version not
                      to read the configuration. Oops, sorry. Also adjusted the
                      default values and made some changes to autower.ini
1.02 (Nov 29, 2006) - fixed the behavior of the DLL. "Unable to connect to
                      FSUIPC" errors should now be gone.
1.03 (Dec 03, 2006) - a couple of minor enhancements, and two major ones: a
                      graphical display of the progress of datafile creation,
                      and some fixes for better tower positioning on small
                      airports.
1.04 (Dec 09, 2006) - better human-readable error messages and slightly more
                      logging. No fixes or improvements to the actual
                      functionality.
                      
2.0 (April 21, 2010)

* Major rewrite of almost everything. The code has undergone very heavy
  refactoring, resulting in dramatically improved cleanliness and readability.
  Hey, it's been 3.5 years of learning since the first version! ;-)

* Combining the airport information in roughly the same way as FS itself does;
  autower now "refines" the airport information starting from the lowest layer,
  instead of taking whichever AFCAD was topmost and ignoring all others. This
  should give results that are way more correct for heavily modded airports.

* Database building sped up by several orders of magnitude. The biggest gain
  comes from the (partial) understanding and using of the scenery.dat indexes,
  instead of brute-force scanning every available BGL file.

* Automatic detection of scenery changes, which triggers an automatic rebuild
  of the data file. Change detection is done through MD5 sums.

* several smaller fixes to address various minor issues:
  - new option to forcibly set the tower position on every iteration. The most
    frequent use will probably be to work around other add-ons that muck around
    with the tower position, like FSHotSFX.
  - avoid having to use UNC paths by translating them to local paths. This
    handles situations where the "Server" service is unavailable better (for
    example, AlacrityPC shuts down that service), generally seems faster, and is
    also less prone to weird "permission denied" errors when requesting write
    access via UNC (which, in contrast, is perfectly fine via a local path)
  - different handling of slow FS startups, now progressively gives FS more time
    to get initialized, and keeps asking whether it should continue.



ACKNOWLEDGEMENTS
================

I'd like to thank all the people who have been directly or indirectly involved
in making this software possible, or allowing for its evolution.

In terms of code, that specifically means:

    Winfried Ortman for his analysis and description of the BGL file format
    
    Pete Dowson, the creator of FSUIPC
    
    The Stony Brook Algorithm Repository, for the kd-tree implementation

    Peter Deutsch and Aladdin Enterprises, for the MD5 implementation

    All of the Eclipse, MinGW, and all the other open-source-tools-I-use-
    developers, who even made it possible to compile this in the first place.

Finally, thanks to all the people who spared their time for in-depth
discussions and testing -- you know who you are :-)



-------------------------------------------------------------------------------
Christoph Langguth <christoph@rosenkeller.org>, 2006 - 2010
http://christoph.rosenkeller.org/fs/autower/
