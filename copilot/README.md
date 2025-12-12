# Build and Deploy Script (`copilot.sh`)

This script automates the configuration, build, and deployment of both the Non-Secure world ([baremetal](../ns-world-bare/) or [RTOS-based](../ns-world-rtos/)) and the Secure world ([S Runtime](../s-world/)).

## Usage

```bash
./copilot.sh [OPTIONS]
```

## Options

-   `-c, --config <s|ns>`: Configure the build system for the Secure (`s`) or Non-Secure (`ns`) world.
-   `-b, --build <s|ns|ns_costum>`: Build the S image (`s`), the default NS image (`ns`), or the custom NS image (`ns_costum`).
-   `-p, --profile <bare|crypto|mstp>`: Select the build profile. Default is `bare`.
-   `-t, --target <BoardName>`: Scify the target board (e.g., `STM32L5`, `STM32U5`). Default is `STM32L5`.
-   `-d, --deploy`: Deploy the built images to the connected target board.

## Examples

A typical workflow involves configuring and building the secure world first, then the non-secure world, and finally deploying.

1.  **Configure and build the Secure World:**
    ```bash
    ./copilot.sh -c s -p mstp
    ./copilot.sh -b s -p mstp
    ```

2.  **Configure and build the Non-Secure World:**
    ```bash
    # For the default RTOS-based NS app
    ./copilot.sh -c ns -p mstp
    ./copilot.sh -b ns -p mstp

    # For the custom bare-metal NS app
    ./copilot.sh -b ns_costum -p mstp
    ```

3.  **Deploy to target:**
    ```bash
    ./copilot.sh -d -p mstp
    ```
