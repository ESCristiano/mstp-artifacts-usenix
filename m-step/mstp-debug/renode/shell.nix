{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # C build system
    gcc gnumake cmake
    autoconf automake libtool

    # Mono and .NET
    mono gtk-sharp-3_0
    dotnet-sdk_8  # optional, for .NET builds

    # GTK + UI deps
    gtk3
    pkg-config

    # Polkit (optional, for some GUI interactions)
    polkit

    # Python (used for scripting in Renode)
    python3

    # Optional: libGL, libuuid, audio support
    libGL libuuid libpulseaudio
  ];

  shellHook = ''
    echo "Renode dev environment loaded."
    export TARGETS="arm"
  '';
}

