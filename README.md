## Wireworld

**Note: This requires the Ncurses library in order to display the graphics.**

**In Linux, this can be done via: sudo apt install libncurses - dev**

gcc wireworld.c neillncurses.c -Wall -Wfloat-equal -Wextra -O2 -pedantic -ansi -lncurses -lm

./a.out wirewcircuit1.txt

Below is an example of what the program should output for the wirewcircuit1.txt (though the actual output should be animated).

![alt text](./Images/1.png "Example Image")

