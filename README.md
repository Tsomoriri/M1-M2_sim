# M1-M2_sim
M1+M2/C/C sim analysis
# Computer Modelling and Simulation 

## Dependencies

- g++ /gcc 
    - `<iostream>`
    - `<vector>`
    - `<random>`
    - `<cmath>`
    - `<fstream>`
- python3
    - pandas
    - numpy
    - matplotlib.pyplot
    - os

## IMPORTANT

- Please make sure to run a new terminal and delete all previous instances of terminal before running.cpp files.
- If there are previous instances of terminal, the random function will not be able to reproduce results.

## M_M_C_C where c=16

- Run the mmcc.cpp 
- specify whether to loop arrival time or show arrival answer when blocking probability <0.1
- Run plot(mmcc).py to plot the results
    - It stores the plots in folder (plots) in the current working directory.

## M1+M2_c_c where c =16

- Run the m1+m2cc.cpp
- Set handover rate to find the value of the arrival rate when abp <0.01
- Set new call rate to find the value of the arrival rate when abp <0.01.
- Run plot(m1+m2cc).py to plot the results.
    - It stores the plots in folder (plots) in the current working directory.
