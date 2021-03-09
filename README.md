# Condition variables and mutexes server.

### Mutexes

* `cs_lock(int mutex_id)` - tries to obtain mutex with ID passed as an argument. 
If mutex is free, then it is assigned to the calling process and `0` (success) is returned. 
Otherwise, the calling process will remain suspended, until the mutex can be assigned to it. After that `0` (success) is returned. 
Process asking for a lock that it already owns will be ignored.
* `cs_unlock(int mutex_id)` - frees mutex with given ID. 
If calling process owns this mutex, then `0` (success) is returned and mutex is assigned to the next process in the queue. 
Otherwise `-1` is returned and `errno` is set to `EPERM`.

Processes waiting for the same mutex are waiting in queue (FIFO).

### Condition variables

* `cs_wait(int cond_var_id, int mutex_id)` - suspends current process, waiting for event `cond_var_id`. 
Process calling this function should be in possession of mutex identified by `mutex_id`, otherwise `-1` is returned and `errno` is set to `EINVAL`.
Function unlocks given mutex and waits until given event is announced by some other process using `cs_broadcast` function. 
After that, process waits in the queue to regain its mutex and then returns `0`. 
*	`cs_broadcast(int cond_var_id)` - announces event identified by `cond_var_id`.

## Signals
Incoming signals are immediately handled in compliancy with registered by process handling procedures. 
If process was waiting for a mutex, then after handling the signal it will resume waiting. 
If it was waiting for an event, then it ignores whether the event happened or not and waits only for mutex to be free (spurious wakeup).
Mutexes owned by dying processes are instantly unlocked.


## Repository structure
- `src/` reflects a part of MINIX source files. 
To use these functionalities one has to copy the content of `src` directory into `/usr/src` in MINIX and install includes, recompile and install standard library and new server and also recompile the system image. 
After rebooting the system, to enable the server the command `service up /usr/sbin/cvs` is used.
