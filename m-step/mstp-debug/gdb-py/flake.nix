{
  description = "Embedded ARM dev shell with Python packages";

  inputs = {
    # We need nixpkgs 22.11 for Python 3.8 because arm-none-eabi-gdb-py just
    # works with Python 3.8 and we need nixpkgs 24.11 for the latest 
    # gcc-arm-embedded package. For example, the version from nixpkgs 22.11
    # doen't have support for gdb-py.
    nixpkgs_nixos_22_11.url = "github:NixOS/nixpkgs/nixos-22.11";
    nixpkgs_nixos_24_11.url = "github:NixOS/nixpkgs/nixos-24.11";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs_nixos_22_11, nixpkgs_nixos_24_11, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs_22 = import nixpkgs_nixos_22_11 {
          inherit system;
        };

        pkgs_24 = import nixpkgs_nixos_24_11 {
          inherit system;
        };
    
        dependencies = [
          pkgs_24.cmake
          pkgs_24.gcc-arm-embedded-13
          pkgs_24.mcuboot-imgtool
        ];

        # Python packages
        pythonEnv = pkgs_22.python38.withPackages (py: [
          py.numpy
        ]);
 
      in {
        devShells.default = pkgs_24.mkShell {
          name = "m-step-debug-shell";

          buildInputs = dependencies ++ [ pythonEnv ];

          shellHook = ''
            echo "ARM dev environment ready."

            # Add Python 3.8 environment to PATH
            export PATH=${pythonEnv}/bin:$PATH

            # Set PYTHONHOME and PYTHONPATH for Python 3.8
            export PYTHONHOME=${pythonEnv}
            export PYTHONPATH=${pythonEnv}/lib/python3.8/site-packages

            # If fish exists, open it
            if command -v fish &> /dev/null; then
              exec fish
            fi
          '';
        };
      }
    );
}
