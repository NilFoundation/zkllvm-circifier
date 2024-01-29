{
  description = "zkLLVM compiler";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs }: {
    packages.x86_64-linux = {
      zkllvm = nixpkgs.legacyPackages.x86_64-linux.llvmPackages.stdenv.mkDerivation {
        pname = "zkllvm";
        version = "1.0";

        src = self;

        nativeBuildInputs = with nixpkgs.legacyPackages.x86_64-linux; [ cmake ninja clang llvm ];

        buildInputs = []; # Add your dependencies here

        cmakeFlags = [
          "-DLLVM_ENABLE_PROJECTS=clang"
          "-DLLVM_TARGETS_TO_BUILD=EVM"
          "-DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD=EVM"
          "-DLLVM_CCACHE_BUILD=ON"
          "-DLLVM_ENABLE_LLD=ON"
          "-DLLVM_INSTALL_TOOLCHAIN_ONLY=ON"
          "-DDEFAULT_SYSROOT=/usr"
          "-DEVM_HOST=ON"
          "-DLLVM_INCLUDE_TESTS=OFF"
          "-DLLVM_DISTRIBUTION_COMPONENTS='clang;evm-stdlib;evm-linker;zkllvm-cxx-headers;zkllvm-c-headers'"
        ];
      };
    };

    defaultPackage.x86_64-linux = self.packages.x86_64-linux.zkllvm;
  };
}
