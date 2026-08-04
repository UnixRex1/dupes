# dupes

`dupes` is a fast command-line utility for finding duplicate files in a directory tree. It recursively scans a filesystem, groups files by size, and then performs byte-by-byte comparisons to identify files that are identical or nearly identical within a user-defined tolerance.

The primary goal of the project is to provide an efficient duplicate finder that avoids unnecessary comparisons by eliminating impossible matches early.

## Features

- Recursive directory traversal
- Fast size-based pre-filtering
- Byte-for-byte file comparison
- Configurable tolerance for differing bytes
- Minimum file size threshold
- Optional deletion of duplicate files
- Simple, dependency-free C implementation
- Linux and macOS compatible

## Building

### Using Make

```bash
make
```

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
```

The resulting executable will be:

```text
dupes
```

## Usage

```text
dupes [minimum_size] [tolerance] <directory> [--delete]
```

### Arguments

| Argument | Description |
|----------|-------------|
| `minimum_size` | Ignore files smaller than this many bytes. |
| `tolerance` | Maximum number of differing bytes allowed before two files are considered different. A value of `0` requires identical files. |
| `directory` | Root directory to scan recursively. |
| `--delete` | Delete duplicate files after they are identified. The first copy found is retained. |

## Examples

Find exact duplicates larger than 1 KB:

```bash
dupes 1024 0 ~/Downloads
```

Allow up to 10 differing bytes:

```bash
dupes 1024 10 ~/Pictures
```

Delete duplicates after verifying the results:

```bash
dupes 1024 0 ~/Downloads --delete
```

## How It Works

The program uses a multi-stage comparison process to minimize expensive file reads.

1. Recursively enumerate all files.
2. Ignore files below the minimum size threshold.
3. Group candidate files by size.
4. Compare only files of identical size.
5. Perform a byte-by-byte comparison.
6. Stop comparing once the configured tolerance has been exceeded.
7. Report (or optionally delete) duplicates.

Because files of different sizes can never be identical, the size filter dramatically reduces the number of comparisons required.

## Exit Status

| Code | Meaning |
|------|---------|
| `0` | Success |
| Non-zero | An error occurred. |

## Safety

When using `--delete`, duplicate files are **permanently removed**.

It is strongly recommended to perform a normal scan first to verify the reported duplicates before enabling automatic deletion.

## License

This project is currently unlicensed.

Choose a license before publishing (MIT, BSD-2-Clause, BSD-3-Clause, Apache-2.0, GPL, etc.).
