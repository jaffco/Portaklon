#include "rtaudio/RtAudio.h"
#include "../Portaklon.hpp"
#include <iostream>
#include <cstdlib>
#include <signal.h>

// Platform-dependent sleep routines.
#if defined( WIN32 )
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

// Interrupt handler function
bool done;
static void finish( int /*ignore*/ ){ done = true; }

typedef float MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32

unsigned int sampleRate = 48000;
unsigned int inputChannels = 1;
unsigned int outputChannels = 2;

int audioCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
                   double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
  Portaklon *portaklon = (Portaklon *) data;
  static float monoOutput[4096]; // Pre-allocated buffer for mono processing
  portaklon->process( (float *) inputBuffer, monoOutput, nBufferFrames );

  // Interleave mono output to stereo
  float *out = (float *) outputBuffer;
  for (unsigned int i = 0; i < nBufferFrames; ++i) {
    out[2 * i] = monoOutput[i];     // Left channel
    out[2 * i + 1] = monoOutput[i]; // Right channel
  }
  return 0;
}

void errorCallback( RtAudioErrorType /*type*/, const std::string &errorText )
{
  std::cerr << "\nerrorCallback: " << errorText << "\n\n";
}

int main( int argc, char *argv[] )
{
  if ( argc > 1 ) sampleRate = atoi( argv[1] );

  RtAudio dac( RtAudio::UNSPECIFIED, &errorCallback );

  if ( dac.getDeviceCount() < 1 ) {
    std::cout << "\nNo audio devices found!\n";
    exit( 1 );
  }

  RtAudio::StreamParameters iParams, oParams;
  iParams.deviceId = dac.getDefaultInputDevice();
  iParams.nChannels = inputChannels;
  iParams.firstChannel = 0;
  oParams.deviceId = dac.getDefaultOutputDevice();
  oParams.nChannels = outputChannels;
  oParams.firstChannel = 0;

  unsigned int bufferFrames = 128;

  Portaklon portaklon;
  portaklon.init( sampleRate, bufferFrames );

  try {
    dac.openStream( &oParams, &iParams, FORMAT, sampleRate, &bufferFrames, &audioCallback, &portaklon );
    dac.startStream();
  }
  catch ( RtAudioErrorType &error ) {
    errorCallback( error, dac.getErrorText() );
    exit( EXIT_FAILURE );
  }

  std::cout << "\nPortaklon audio processor running at " << sampleRate << " Hz.\n";
  std::cout << "Press Ctrl-C to quit.\n";

  done = false;
  (void) signal(SIGINT, finish);

  while ( !done && dac.isStreamRunning() ) {
    SLEEP( 100 );
  }

  try {
    dac.stopStream();
  }
  catch ( RtAudioErrorType &error ) {
    errorCallback( error, dac.getErrorText() );
  }

  if ( dac.isStreamOpen() ) dac.closeStream();

  return 0;
}