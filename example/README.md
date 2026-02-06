# Portaklon Audio Example

This is a simple command-line audio application that processes audio input through the Portaklon amplifier model using RtAudio.

## Building

Run `make` to build the `portaklon_audio` executable in the `build/` directory.

## Usage

```bash
./build/portaklon_audio [sample_rate]
```

- `sample_rate`: Sample rate in Hz (default: 44100)

The app will open the default input and output audio devices, process the input through Portaklon, and output the result.

Press Ctrl-C to quit.

## Dependencies

- RtAudio (included as submodule)
- CoreAudio framework (macOS)
- Portaklon source code (in parent directory)