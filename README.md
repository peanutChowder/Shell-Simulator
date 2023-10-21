# Shell-Simulator

A unix shell simulator focused on concurrent process management. A list of commands are found below:

---

`exit` exit the shell

`jobs` Display the list of running processes, their running time and process IDs, and a summary of the total system and user time taken for completed processes. 

`kill <pid>`  Kill the process with process ID 'pid'.

`suspend <pid>` Suspend process 'pid' until resumed.

`resume <pid>` Resume the suspended process 'pid'. 

`sleep <int>` Force the shell to sleep for 'int' time.

`wait <pid>` Wait until process 'pid' has completed execution.

---

**Running unix commands**

`<cmd> <args> <shell args>` Input a unix command and its arguments. E.g. `ls -al`

- Shell args
  - `&` Run the given cmd in the background, i.e. create a new process that runs the command.
  - `<file` Pipe the file with name 'file' as the stdin to the 'cmd'
  - `>file` Pipe the file with the name 'file' as the stdout from the 'cmd'
    - e.g. `ls -al >output.txt &` runs the linux list directory command in the background and outputs the results into 'output.txt'.

---

