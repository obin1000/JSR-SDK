//! Finds the JSR-SDK DLLs and puts them where programs built by Cargo find
//! them: in a folder Cargo adds to PATH for `cargo run` and `cargo test`, and
//! next to the programs and examples. Needs JSR-SDK 1.1.0 or later, whose
//! JSR-SDK.dll finds its .NET assemblies next to itself.
//!
//! - With `JSR_SDK_DIR` set to an unpacked JSR-SDK release (or a checkout of
//!   JSR-SDK after building the DLL), it uses that.
//! - Otherwise it downloads the release below from GitHub, checks its SHA-256
//!   and unpacks it into `OUT_DIR`. It needs `curl` and `tar`, which Windows 10
//!   and later include.
//!
//! The bindings link `JSR-SDK.dll` by name (raw-dylib), so nothing here is
//! needed to compile; the DLLs only have to be there at run time.
use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::process::Command;

/// The release downloaded without `JSR_SDK_DIR`, and the SHA-256 of its zip.
const SDK_RELEASE: &str = "1.0.0";
const SDK_SHA256: &str = "c70094eb6eb54763cf77db024d620c817e84a029f99ef5436373cdc7ac64e497";
const RELEASES_URL: &str = "https://github.com/obin1000/JSR-SDK/releases/download";

fn main() {
    println!("cargo:rerun-if-changed=build.rs");
    println!("cargo:rerun-if-env-changed=JSR_SDK_DIR");

    // lib.rs stops the build with a clear message on other systems.
    let windows = env::var("CARGO_CFG_TARGET_OS").as_deref() == Ok("windows");
    // docs.rs builds without network access, and documentation needs no DLLs.
    if !windows || env::var_os("DOCS_RS").is_some() {
        println!("cargo:rustc-env=JSR_SDK_RELEASE={SDK_RELEASE}");
        return;
    }

    let (root, release) = match env::var_os("JSR_SDK_DIR") {
        Some(dir) => (PathBuf::from(dir), "local".to_string()),
        None => (download(), SDK_RELEASE.to_string()),
    };
    println!("cargo:rustc-env=JSR_SDK_RELEASE={release}");
    // Dependents' build scripts see this as DEP_JSR_SDK_ROOT.
    println!("cargo:root={}", root.display());

    let arch = match env::var("CARGO_CFG_TARGET_ARCH").unwrap().as_str() {
        "x86_64" => "x64",
        "x86" => "Win32",
        other => panic!("JSR-SDK has no DLLs for the {other} architecture"),
    };
    let files = runtime_files(&root, arch);

    // 1. A folder inside the target directory, announced as a link-search
    //    path: Cargo puts such folders on PATH for `cargo run` and
    //    `cargo test`, wherever its layout puts the programs. JSR-SDK.dll
    //    (1.1.0 and later) finds its .NET assemblies next to itself.
    let dll_dir = PathBuf::from(env::var("OUT_DIR").unwrap()).join("dlls");
    copy_all(&files, &dll_dir);
    println!("cargo:rustc-link-search=native={}", dll_dir.display());

    // 2. Next to the programs and examples, so they also start outside
    //    Cargo. Tests can't rely on this: newer Cargo runs them from a
    //    folder per build.
    if let Some(profile) = profile_dir() {
        copy_all(&files, &profile);
        copy_all(&files, &profile.join("examples"));
    }
}

fn copy_all(files: &[PathBuf], dir: &Path) {
    fs::create_dir_all(dir).unwrap();
    for file in files {
        copy_if_changed(file, &dir.join(file.file_name().unwrap()));
    }
}

/// JSR-SDK.dll, JSRDotNETSDK.dll and the plugin DLLs of the release.
fn runtime_files(root: &Path, arch: &str) -> Vec<PathBuf> {
    let dynamic = root.join("lib").join(arch).join("dynamic");
    let sdk = dynamic.join("JSR-SDK.dll");
    if !sdk.is_file() {
        panic!(
            "{} is missing. Set JSR_SDK_DIR to an unpacked JSR-SDK release, or to \
             a JSR-SDK checkout after building the DLL with \
             msbuild JSR-SDK.vcxproj -p:Configuration=dynamic -p:Platform={arch}",
            sdk.display()
        );
    }
    let mut files = vec![sdk, dynamic.join("JSRDotNETSDK.dll")];
    let plugins = root.join("lib").join("plugins");
    let mut plugin_dlls: Vec<PathBuf> = fs::read_dir(&plugins)
        .unwrap_or_else(|e| panic!("cannot read {}: {e}", plugins.display()))
        .map(|entry| entry.unwrap().path())
        .filter(|path| {
            path.extension()
                .is_some_and(|ext| ext.eq_ignore_ascii_case("dll"))
        })
        .collect();
    plugin_dlls.sort();
    files.extend(plugin_dlls);
    for file in &files {
        println!("cargo:rerun-if-changed={}", file.display());
    }
    files
}

/// target/<profile>, where Cargo puts programs: the parent of the `build`
/// folder that holds OUT_DIR, in the old layout
/// (build/<package>-<hash>/out) and the new one (build/<package>/<hash>/out).
fn profile_dir() -> Option<PathBuf> {
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    let build = out_dir
        .ancestors()
        .find(|dir| dir.file_name().is_some_and(|name| name == "build"))?;
    build.parent().map(Path::to_path_buf)
}

fn copy_if_changed(from: &Path, to: &Path) {
    let same = match (fs::metadata(from), fs::metadata(to)) {
        (Ok(a), Ok(b)) => a.len() == b.len() && a.modified().ok() <= b.modified().ok(),
        _ => false,
    };
    if !same {
        fs::copy(from, to)
            .unwrap_or_else(|e| panic!("cannot copy {} to {}: {e}", from.display(), to.display()));
    }
}

/// Downloads, checks and unpacks the release into OUT_DIR, once.
fn download() -> PathBuf {
    let out_dir = PathBuf::from(env::var("OUT_DIR").unwrap());
    let dir = out_dir.join(format!("JSR-SDK-v{SDK_RELEASE}"));
    let done = dir.join(".unpacked");
    if done.is_file() {
        return dir;
    }

    let zip = out_dir.join(format!("JSR-SDK-v{SDK_RELEASE}.zip"));
    let url = format!("{RELEASES_URL}/v{SDK_RELEASE}/JSR-SDK-v{SDK_RELEASE}.zip");
    run(
        Command::new("curl")
            .args(["--fail", "--location", "--silent", "--show-error"])
            .args(["--retry", "3", "--output"])
            .arg(&zip)
            .arg(&url),
        &format!("downloading {url}"),
    );
    let actual = sha256_hex(&fs::read(&zip).unwrap());
    if actual != SDK_SHA256 {
        let _ = fs::remove_file(&zip);
        panic!(
            "{url} has SHA-256 {actual}, expected {SDK_SHA256}. Not using it. \
             Set JSR_SDK_DIR to an unpacked release to build without the download."
        );
    }

    let _ = fs::remove_dir_all(&dir);
    fs::create_dir_all(&dir).unwrap();
    run(
        Command::new("tar").arg("-xf").arg(&zip).arg("-C").arg(&dir),
        &format!("unpacking {}", zip.display()),
    );
    fs::write(&done, SDK_SHA256).unwrap();
    let _ = fs::remove_file(&zip);
    dir
}

fn run(command: &mut Command, what: &str) {
    let status = command
        .status()
        .unwrap_or_else(|e| panic!("{what} failed to start: {e}"));
    if !status.success() {
        panic!(
            "{what} failed ({status}). Set JSR_SDK_DIR to an unpacked JSR-SDK \
             release to build without the download."
        );
    }
}

/// SHA-256 (FIPS 180-4) of data as lowercase hex, so the build script needs
/// no dependencies.
fn sha256_hex(data: &[u8]) -> String {
    const K: [u32; 64] = [
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4,
        0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe,
        0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f,
        0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
        0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
        0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
        0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116,
        0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7,
        0xc67178f2,
    ];
    let mut h: [u32; 8] = [
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab,
        0x5be0cd19,
    ];
    let mut message = data.to_vec();
    message.push(0x80);
    while message.len() % 64 != 56 {
        message.push(0);
    }
    message.extend_from_slice(&((data.len() as u64) * 8).to_be_bytes());

    for block in message.chunks_exact(64) {
        let mut w = [0u32; 64];
        for (i, word) in block.chunks_exact(4).enumerate() {
            w[i] = u32::from_be_bytes([word[0], word[1], word[2], word[3]]);
        }
        for i in 16..64 {
            let s0 = w[i - 15].rotate_right(7) ^ w[i - 15].rotate_right(18) ^ (w[i - 15] >> 3);
            let s1 = w[i - 2].rotate_right(17) ^ w[i - 2].rotate_right(19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16]
                .wrapping_add(s0)
                .wrapping_add(w[i - 7])
                .wrapping_add(s1);
        }
        let [mut a, mut b, mut c, mut d, mut e, mut f, mut g, mut hh] = h;
        for i in 0..64 {
            let s1 = e.rotate_right(6) ^ e.rotate_right(11) ^ e.rotate_right(25);
            let ch = (e & f) ^ (!e & g);
            let t1 = hh
                .wrapping_add(s1)
                .wrapping_add(ch)
                .wrapping_add(K[i])
                .wrapping_add(w[i]);
            let s0 = a.rotate_right(2) ^ a.rotate_right(13) ^ a.rotate_right(22);
            let maj = (a & b) ^ (a & c) ^ (b & c);
            let t2 = s0.wrapping_add(maj);
            hh = g;
            g = f;
            f = e;
            e = d.wrapping_add(t1);
            d = c;
            c = b;
            b = a;
            a = t1.wrapping_add(t2);
        }
        for (state, value) in h.iter_mut().zip([a, b, c, d, e, f, g, hh]) {
            *state = state.wrapping_add(value);
        }
    }
    h.iter().map(|word| format!("{word:08x}")).collect()
}
