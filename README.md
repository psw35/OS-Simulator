# OS-Simulator
Operating system simulator written in C. I developed the main simulator, which is designed to simulate a variety of process scheduling algorithms, along with memory management. Credit goes to Professor Michael Leverington for various utility files.

# Instructions for Running:
Step 1: Enter the directory and run the command
### make -f simulator_mf
Step 2: Make any desired changes to the config file, config0.cnf such as memory management on/off, log to an output(monitor, file, or both), or one of the scheduling algorithm codes:
### FCFS-N is first come first serve, nonpreemptive
### FCFS-P is first come first serve, preemptive
### SJF-N is shortest job first, nonpreemptive
### SRTF-P is shortest remaining time first, preemptive
### RR-P is round-robin, preemptive
Step 3: Run the command
### ./sim04 -rs config0.cnf
Step 4: The simulator should now be running!
Note: Preemptive may have some bugs, but nonpreemptive should be accurate every time.
