# Translation Lookaside Buffer

## Gruppenmitglieder

- Lukas Roland Greinwald 
- Enes Rafioglu
- Rubin Raithel

### Why do I get `Warning: (W518) stack setup failed: unsuccessful stack protection ignored: Cannot allocate memory, address=0x, enable=true`?

The warning message you're seeing is triggered during the memory allocation process for SystemC threads in the prepare_to_simulate() function. This involves allocating memory mappings for all SystemC threads, which our TLB implementation requires.

Our TLB uses three registers to hold the data for each cache line. Each of these registers needs to run its own thread to detect write events, resulting in a large number of threads—typically three times the TLB size.

Upon investigation, we found the root cause: a limit on the number of memory mappings in Linux. This limit is determined by the value in /proc/sys/vm/max_map_count.

  - On LXHalle, this value is set to the default of 65,530.
  - On our testing machine (Apple M2 with 64GB RAM, running in Docker), this value is set to the default of 262,144.

The higher limit on our testing machine allows the simulation to run successfully, while the lower limit on LXHalle causes the memory allocation to fail, leading to the warning.

We confirmed this by inspecting the output of info proc mappings, which showed that the memory mapping limit was indeed the issue preventing the simulation from running on LXHalle.

While this is not a limitation of our implementation—simplifying the storage would go against the spirit of our project—you can easily fix this issue if you encounter it by increasing the maximum number of memory mappings with the following command:

```sh
sudo sysctl -w vm.max_map_count=262144
```

This adjustment should allow the simulation to run smoothly on systems with lower default limits.
