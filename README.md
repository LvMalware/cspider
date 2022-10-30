# cspider
> A fast webcrawler/spider written in C

## Installation

First, we need a few libraries to compile it. On a debian-based system, you can install them with:

```bash
sudo apt install -y libcurl4-openssl-dev libxml2-dev gcc make
```

Them, cd into cspider directory and compile:

```bash
cd cspider #or whatever the directory is named in your system
make clean
make
```

Now if you want, you can install it as a system command with:

```bash
sudo cp cspider /usr/local/bin/
```

## Usage

The help message can be consulted by running with the `-h` flag

```
cspider - Webcrawler/spider for security tests

Usage: cspider [option(s)]
Options:
   -h          Show this help message and exit.
   -i          Input file with URLs
   -s          Add domain to scope
   -d          Maximum depth to crawl
   -o          Output file to log results
   -t          Number of concurrent threads

```

When started, cspider will read urls from stdin (default) or from an input file specified with `-i` and crawl them up to a maximum depth that can be specified with the `-d` flag. The input urls have a depth of 0, links found on them have a depth of 1, and so on.

By default, it will follow every link it finds. To avoid going to third-party domains, you can use the `-s` flag to specify domains in scope. Note that all the subdomains of each scoped domain will be included. 

The number of concurrent threads is specified with `-t` and defaults to 16. To print results to a file instead of stdout, you can use the `-o` flag.
