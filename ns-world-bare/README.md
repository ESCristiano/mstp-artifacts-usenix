# Non-Secure (NS) world runtime environment, baremetal

This folder provides the Non-Secure (NS) world runtime environment, without an RTOS (baremetal application). 
This folder provide the necessary means to run the experiments presented in **Sections 6.1**, **6.2**, and **6.3** in a baremetal setting. 

There is three scripts wich provide end-to-end workflow:
- `0_config`: Configures the NS and S runtime environments.  
- `1_compile`: Compiles NS and S runtime environments as well as the Secure Bootloader.
- `2_deploy`: Deploys the compiled binaries to the target board.


