The main esterel code is in ballcollector.strl




Steps to follow for generating C code from esterel:
===================================================

1. Startup the virtual box, run the guest OS (ubuntu).
2. run the mymount.sh file to share a folder between windows and ubuntu. The folder is located at the following locations-

windows- C:vboxfold
Ubuntu-  /home/erts/shd

3. Yo have to convert your esterel program into C, before executing it. For this purpose, there are four files provided to you - 
buildhash.pl, getmodulename.pl,firebird-gen.sh, firebird_winavr.h

4. First write down an esterel program, then open the terminal, go to the folder your files are in and type the command -
./firebird-gen <esterel file name>

5. This will convert your  esterel program into C program, you may get an error message, just ignor it.

6. For the valid input-output port names, function names that you can define, see the buildhash.sh file, if you want to change the 
names or want to use different naming scheme, please make sure to make suitable changes into the buildhash.pl and firebird_winavr.h file too.  

7. Copy this file into the shared folder, now use the AVR studio, to program your bot.
(you can not replace files in this folder, you have to delet the old file in order to copy the new file in it)


