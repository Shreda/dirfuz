# dirfuz

dirfuz is a cross-platform, **single** threaded, directory brute force tool. As long as you have libcurl installed, you will be able to compile the code. This tool was written on Windows using Visual Studio Community 2019. However, you can also develop and compile on Linux easily

## Compiling

- Install libcurl (refer to libcurl documentation)
- Follow the guide for Windows or Linux

### Windows

- Open the project in Visual Studio Community 2019
- Select **Build Solution**
- Donerinoo

### Linux

- `cd` into the project
- Run the command below

```
g++ difuz.cpp -lcurl -o dirfuz
```

- You probably want the binary on on your $PATH so move it somewhere. I usually do:

```
sudo mv dirfuz /usr/local/bin/
```

## Usage

```
$ dirfuz -h                                                                                         
dirfuz - Simple and fast directory brute forcing.
Usage:
  dirfuz [OPTION...]

  -u, --url arg          Base URL, for example https://www.example.com
  -w, --wordlist arg     Wordlist to perform the brute force with
  -m, --maxparralel arg  Maximum number of parallel requests to send at one
                         time (default: 20)
  -h, --help             Print usage
```
