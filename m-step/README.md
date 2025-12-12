# M-Step Framework & Experimental Setups 

This folder contains the source code for the M-Step framework, including the core mechanisms (**Section 4**), infrastructure (**Section 5**), and evaluation code (**Section 6**), and is organized as follow:
- **`mstp`**: Implements the core M-Step mechanisms, including the main algorithm, dynamic timer adjustment, and target-specific configurations (`mstp.h`).
- **`mstp-victims`**: Contains the victim code and Trusted Applications used in the experiments for **Sections 6.1** and **6.2**.
- **`mstp-eval`**: Provides the evaluation infrastructure to configure and orchestrate the tests presented in **Sections 6.1** and **6.2**.
- **`mstp-poc`**: Contains the code for running the end-to-end attacks described in **Section 6.3**.
- **`mstp-debug`**: Includes scripts and configurations for debugging M-Step and performing *post-mortem* trace analysis using an MCU emulator (Renode).
- **`mstp-visualizer`**: Provides visualization tools and scripts for analyzing M-Step traces and extracting RSA keys.
- **`traces`**: Contains raw M-Step traces used for the end-to-end attacks in **Section 6.3**.
- **`Drivers`**: Contains third-party drivers for the target MCUs.

**Note:** Not all M-Step plugins described in the paper map directly to a dedicated folder. For example, Mstp-Busted, Mstp-Zoom, and Mstp-Cache are implemented in `mstp.c` and enabled via conditional compilation or runtime settings. The table below maps the plugins from Table 4 to their corresponding locations in the artifact package.


| Paper Plugin   	| Artifacts Location		|  
| :--- 				| :--- 						| 
| Mstp-Nemesis    	| `mstp/src/mstp.c`			| 
| Mstp-Cache  		| `mstp/src/mstp.c`			| 
| Mstp-BUSted  		| `mstp/src/mstp.c`			|
| Mstp-Zoom  		| `mstp/src/mstp.c`			|
| Mstp-Production  	| `mstp/src/mstp.c`			|
| Mstp-Debug  		| `mstp-debug` && `mstp/src/mstp.c` |
| Mstp-Metrics   	| `mstp/src/mstp_metrics.c` |
| Mstp-Emulator   	| `mstp-debug/renode`   	|
| Mstp-Visualizer  	| `mstp-visualizer`   		|
| Mstp-Test   		| `mstp-eval`   			|