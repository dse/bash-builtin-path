# bash-builtin-path

The `path` GNU Bash builtin makes manipulating PATH and similarly
constructed environment variables easier.

It also serves as a real-world example of how to construct a Bash
builtin in C.

## Usage

To list the contents of a path:

    path PATH
    path MANPATH

To check if a directory is in your PATH:

    path -c PATH /usr/local/bin

To prepend a directory:

    path PATH /usr/local/bin

To prepend a directory, even if it's already in your path:

    path -f PATH /usr/local/bin

To append a directory:

    path -a PATH /usr/local/bin

To remove a directory:

    path -d PATH /usr/local/bin

## Other Examples

-   https://github.com/bminor/bash/tree/master/examples/loadables
-   https://stackoverflow.com/questions/37360273/how-to-embed-a-command-into-bash-which-act-as-built-in
-   https://blog.dario-hamidi.de/a/build-a-bash-builtin
-   https://github.com/dhamidi/bash-plugin-hello

## Why?

Because the shell function I wrote that meets my requirements is kind
of slow, and I needed a hobby.

Yes, I'm fully aware that Bash scripting is the worst possible hobby.

I'm not switching to `zsh` or whatever.
