# FileBrowser Example

This example demonstrates a simple file browser using the am-ui library for AmigaOS.

## Features

- Browse directories on your Amiga system
- Navigate using ".." button to go to parent directory
- Click on directories to open them
- Displays files in a vertical list

## Building

Make sure you have the amlc.jar compiler in this directory, then run:

```bash
java -jar amlc.jar . -acmd -bt amigaos_docker_unix-x64
```

Or use the Makefile:

```bash
make build
```

The binary will be created in `builds/bin/amigaos/app`.

## Running

Copy the `app` file to your Amiga system and run it. The file browser will start in the `ram:` directory.
