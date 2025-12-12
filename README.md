# M-Step: A Single-Stepping Framework for Side-Channel Analysis on TrustZone-M

This is the repository aggregating the artifacts of the USENIX Sec'26 paper: **M-Step: A Single-Stepping Framework for Side-Channel Analysis on TrustZone-M**.

**Note:** The terminology in this repository may not always match that used in the paper.
However, we are committed to addressing this for artifact evaluation (Functional and Reproduced badges), ensuring consistency between the paper and the artifacts.
Furthermore, we strongly believe this framework will become widely used—similar to SGX-Step for Intel—so we will also provide comprehensive documentation to describe proper usage and ensure maintainability.

## Structure

This repo is organized as follows:
- **[copilot](copilot/):** Contains scripts and configuration files for setting up the M-Step development environment.
- **[evaluation](evaluation/):** Contains the datasets and scripts required to reproduce the figures and tables presented in the paper’s evaluation (**Section 6**). 
- **[m-step](m-step/):** Contains the source code for the M-Step framework, including the core mechanisms (**Section 4**), infrastructure (**Section 5**), and evaluation code (**Section 6**).
- **[ns-world-bare](ns-world-bare/):** Contains a Baremetal NS runtime enviroment.
- **[ns-world-rtos](ns-world-rtos/):** Contains a  RTOS-based NS runtime enviroment.
- **[s-world](s-world/):** Contains the Secure world runtime environment and third-party modules, managed as Git submodules (e.g., TF-M, Mbed TLS, MCUboot, etc).
  
## Reproduction

If you are familiar with Nix, setup should be straightforward.
A Nix shell/dev environment (`flake.nix`) is provided to fetch all dependencies needed to build and deploy the experiments/software.

To enter the development environment in each folder, run:

```bash
nix develop
```

### Prerequisites

These steps were tested on Ubuntu 24.04.

1. Install the Nix package manager and add your user to the `nix-users` group (you will need to log out and back in):

    ```shell
    sudo apt install nix-bin
    sudo adduser <your-username> nix-users
    ```

2. Install the STM32 programmer from:  

    https://www.st.com/en/development-tools/stm32cubeprog.html#get-softwar (You will need to log in or create an account.) 

    Ensure the `STM32_Programmer_CLI` binary is in your PATH.

3. Open the development environment with all necessary dependencies:

    ```shell
    nix --extra-experimental-features nix-command --extra-experimental-features flakes develop
    ```

4. You now have a shell with all required programs and tools to run the code.

**Note:** If needed, press the reset button on the board. You should see output on the serial port:

```shell
# To see connected boards and ports, use:
STM32_Programmer_CLI -l

# To connect to the serial port:
sudo minicom -D /dev/ttyACM0 -b 115200
# or
screen /dev/ttyACM0 115200
```
