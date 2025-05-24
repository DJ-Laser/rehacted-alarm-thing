{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = {
    self,
    nixpkgs,
  } @ inputs: let
    system = "x86_64-linux";
    pkgs = import nixpkgs {inherit system;};
    lib = pkgs.lib;
  in {
    devShells.${system}.default = pkgs.mkShell {
      buildInputs = with pkgs; [arduino-cli alejandra];
    };
  };
}
