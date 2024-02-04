# ganajxt

**I only tested this on Void Linux. It's currently in an *"It works on my machine"* state**

You can use ganajxt to *compile*
```
ganajxt -c input.txt [optinal:output.gxt]
```
and to *decompile*
```
ganajxt -d input.gxt [optional:output.txt]
```
**G**TA Te**XT** files.

**Please don't mess around with trying to decompile .txt files. It will get stuck in an infinite loop. And I'm okay with that behaviour.
Also: don't try to compile .gxt files. Just use it as intended. I take no responsibility for any corrupt game files.**

The program has no external dependencies besides the standard C library.

Compilation:
```
cc -o ganajxt ganajxt.c
```

This creates a path-independent executable so you can just
```
sudo cp ganajxt /usr/bin/
```
to install it.

Run the command without arguments to display the usage information.
