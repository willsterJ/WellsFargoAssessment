## Run instructions:

This project was built using Visual Studio 2022. Build and Run will create a console output application.

## Problem 1 design
For Problem 1, I used the producer/consumer approach whereby
**Timer** produces **Events** that are then added to a queue (**EventQueue**) 
while **Worker** consumes from it. **Timer** and **Worker** both operate
in their own respective thread, and while **Worker** waits for
**Timer** to produce an **Event**, **Worker** starts accumulating **Jobs** (functions to
be executed). Once an **Event** has been produced whose execution
time is closest to current run time, **Worker** will then proceed
to execute all the **Jobs** it had accumulated.
