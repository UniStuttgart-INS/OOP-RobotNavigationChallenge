{
  description = "OOP - Robot Navigation Task";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs =
    { self, nixpkgs }:
    let
      forAllSystems = nixpkgs.lib.genAttrs [
        "x86_64-linux"
        "aarch64-linux"
      ];
    in
    {
      devShells = forAllSystems (
        system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs = with pkgs; [
              cmake
              conan
              gdb
              lldb
              mold
              llvmPackages_19.clang-tools # clang-format, clang-tidy
              gcc13
              gcc13Stdenv

              ccache
              ccacheStdenv
              llvmPackages_19.libcxxClang
            ];
            buildInputs = with pkgs; [
              xorg.libX11.dev
              glfw
              gl3w
              libGLU
              libunwind
            ];
            LD_LIBRARY_PATH =
              with pkgs;
              "${libGL}/lib"
              + ":${glfw}/lib"
              + ":${libGLU}/lib"
              + ":${stdenv.cc.cc.lib}/lib"
              + ":${llvmPackages_19.libcxx}/lib";
          };
        }
      );
    };
}
