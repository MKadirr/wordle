{ pkgs ? import <nixpkgs> {}}:

pkgs.mkShell {
    buildInputs = with pkgs; [
        libpng
        gnumake
        gcc
    ];

    shellHook = ''

    '';
}
