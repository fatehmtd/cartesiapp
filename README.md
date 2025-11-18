# CartesiaPP

C++ wrapper for the Cartesia.io audio processing API. Supports text-to-speech synthesis and speech-to-text transcription via REST and WebSocket connections.

## Features

- **Text-to-Speech**: Generate audio from text with various voice options and output formats
- **Speech-to-Text**: Transcribe audio files or byte streams to text with word-level timing
- **Voice Management**: List and retrieve available voices with filtering options
- **Multiple Output Formats**: WAV, MP3, PCM with configurable sample rates and encodings
- **Streaming Support**: Real-time audio processing via WebSocket connections

## Build

Requires CMake 3.16+ and vcpkg for dependencies.

```bash
git clone https://github.com/fatehmtd/cartesiapp.git
cd cartesiapp
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build
```

## Usage

Set your API key as an environment variable:

```bash
export CARTESIA_API_KEY=your_api_key_here
```

### Basic Text-to-Speech (Bytes)

```cpp
#include <cartesiapp.hpp>

cartesiapp::Cartesia client(apiKey);

// Get available voices
cartesiapp::request::VoiceListRequest voiceRequest;
voiceRequest.gender = cartesiapp::request::voice_gender::FEMININE;
auto voices = client.getVoiceList(voiceRequest);

// Configure TTS request
cartesiapp::request::TTSBytesRequest ttsRequest;
ttsRequest.transcript = "Hello world, this is a test message.";
ttsRequest.voice.id = voices.voices[0].id;
ttsRequest.output_format.container = cartesiapp::request::container::WAV;
ttsRequest.output_format.encoding = cartesiapp::request::tts_encoding::PCM_S16LE;
ttsRequest.output_format.sample_rate = cartesiapp::request::sample_rate::SR_44100;

// Generate audio
std::string audioData = client.ttsBytes(ttsRequest);

// Save to file
std::ofstream outFile("output.wav", std::ios::binary);
outFile.write(audioData.data(), audioData.size());
```

### Streaming Text-to-Speech

```cpp
#include <streaming_tts.hpp>

class MyTTSListener : public cartesiapp::TTSResponseListener {
public:
    void onAudioChunkReceived(const cartesiapp::response::tts::AudioChunkResponse& response) override {
        // Process audio chunks in real-time
        processAudioChunk(response.data);
    }
    
    void onDoneReceived(const cartesiapp::response::tts::DoneResponse& response) override {
        spdlog::info("TTS complete");
    }
};

auto listener = std::make_shared<MyTTSListener>();
auto websocketClient = std::make_unique<cartesiapp::TTSWebsocketClient>(apiKey, apiVersion);
websocketClient->registerTTSListener(listener);
websocketClient->connectAndStart();

cartesiapp::request::tts::GenerationRequest request;
request.transcript = "Your text here";
request.voice.id = "voice-id";
websocketClient->requestTTS(request);
```

### Speech-to-Text (File)

```cpp
cartesiapp::request::stt::BatchRequest sttRequest;
auto response = client.sttWithFile("audio.mp3", sttRequest);

std::cout << "Transcribed: " << response.text << std::endl;
std::cout << "Duration: " << response.duration << " seconds" << std::endl;

// Word-level timestamps
for (const auto& word : response.words) {
    std::cout << word.word << " [" << word.start << "-" << word.end << "ms]" << std::endl;
}
```

### Streaming Speech-to-Text

```cpp
#include <streaming_stt.hpp>

class MySTTListener : public cartesiapp::STTResponseListener {
public:
    void onTranscriptionReceived(const cartesiapp::response::stt::TranscriptionResponse& response) override {
        std::cout << "Partial: " << response.text << std::endl;
        if (response.is_final) {
            std::cout << "Final: " << response.text << std::endl;
        }
    }
};

cartesiapp::STTWebsocketClient sttClient(apiKey, model, "en", encoding, sampleRate);
auto listener = std::make_shared<MySTTListener>();
sttClient.registerSTTListener(listener);
sttClient.connectAndStart();

// Stream audio data
sttClient.writeAudioBytes(audioBuffer.data(), audioBuffer.size());
```

## Sample Applications

The `samples/` directory contains working examples demonstrating all library features:

### Text-to-Speech Samples

- **`sample-tts-bytes.cpp`** - Basic TTS with byte output
  - Demonstrates voice listing and selection
  - Configurable audio formats (WAV, MP3, PCM)
  - Emotion and speed control
  - Saves generated audio to file

- **`sample-tts-streaming.cpp`** - Real-time TTS streaming
  - WebSocket-based streaming audio generation
  - Custom response listener implementation
  - Real-time audio chunk processing
  - Performance metrics (time to first byte)

### Speech-to-Text Samples

- **`sample-stt-file.cpp`** - Batch STT from audio files
  - Supports MP3, WAV, and other audio formats
  - Word-level timestamp extraction
  - Language detection and configuration

- **`sample-stt-streaming.cpp`** - Real-time STT streaming
  - WebSocket-based streaming transcription
  - Live audio processing with chunked data
  - Partial and final transcription results
  - Custom audio encoding support

### Running the Samples

```bash
cd build/samples/Debug
./CartesiaPP_Sample_TTS_Bytes.exe
./CartesiaPP_Sample_TTS_Streaming.exe
./CartesiaPP_Sample_STT_File.exe
./CartesiaPP_Sample_STT_Streaming.exe
```

Each sample includes detailed logging and error handling to help you understand the API workflow.

## API Feature Coverage

CartesiaPP currently implements a subset of the full Cartesia API. Here's what's supported:

### âœ… Implemented Features

#### Core API

- âœ… API status and version checking
- âœ… Authentication via API keys
- âœ… Multiple API version support (2024-06-10, 2024-11-13, 2025-04-16)

#### Text-to-Speech (TTS)

- âœ… Byte-based synthesis (REST API)
- âœ… Real-time streaming synthesis (WebSocket)
- âœ… Voice selection and management
- âœ… Multiple output formats (WAV, MP3, PCM)
- âœ… Audio encoding options (PCM_S16LE, PCM_F32LE, PCM_MULAW, PCM_ALAW)
- âœ… Sample rate configuration (8kHz to 48kHz)
- âœ… Emotion control (50+ emotions supported)
- âœ… Speed and volume adjustment
- âœ… Context management for streaming
- âœ… Word and phoneme timestamp support
- âœ… Multiple TTS models (Sonic-3, Sonic-2)

#### Speech-to-Text (STT)

- âœ… Batch transcription from files
- âœ… Batch transcription from byte arrays
- âœ… Real-time streaming transcription (WebSocket)
- âœ… Multiple audio formats support
- âœ… Word-level timestamps
- âœ… Language specification
- âœ… Audio encoding configuration
- âœ… INK-Whisper model support

#### Voice Management

- âœ… List available voices with filtering
- âœ… Voice details retrieval
- âœ… Gender-based filtering
- âœ… Owner/starred voice filtering
- âœ… Pagination support

### âŒ Missing Features (TODO)

#### Voice Cloning & Management

- âŒ Voice cloning from audio samples
- âŒ Custom voice creation and upload
- âŒ Voice deletion
- âŒ Voice sharing and privacy controls
- âŒ Voice embedding management

#### Advanced TTS Features

- âŒ Server-Sent Events (SSE) streaming
- âŒ Pronunciation dictionary support
- âŒ Voice embedding mode
- âŒ Advanced generation controls (pitch, emphasis)
- âŒ Multi-speaker synthesis

#### Advanced STT Features

- âŒ Language auto-detection
- âŒ Custom vocabulary/models
- âŒ Confidence scores
- âŒ Speaker diarization
- âŒ Advanced audio preprocessing

#### Conversation Agents

- âŒ Agent creation and management
- âŒ Agent deployment
- âŒ Phone number integration
- âŒ Webhook configuration
- âŒ Conversation metrics and analytics
- âŒ Git repository integration

#### Enterprise Features

- âŒ Usage analytics and metrics
- âŒ Rate limiting information
- âŒ Billing and quota management
- âŒ Team/organization management
- âŒ Audit logging

### Roadmap

#### Phase 1: Voice Management

- [ ] Implement voice cloning API
- [ ] Add voice deletion functionality
- [ ] Support voice privacy controls

#### Phase 2: Advanced TTS

- [ ] Add Server-Sent Events (SSE) support
- [ ] Implement pronunciation dictionary features
- [ ] Support voice embedding mode

#### Phase 3: Enterprise Features

- [ ] Add usage metrics and analytics
- [ ] Implement rate limiting support
- [ ] Add billing/quota information APIs

#### Phase 4: Conversation Agents

- [ ] Basic agent management
- [ ] Agent deployment support
- [ ] Webhook integration

Contributions are welcome! See the missing features above for areas where help is needed.

## License

See [LICENSE](LICENSE) file for details.
