# Protocol Documentation
<a name="top"></a>

## Table of Contents

- [rpc/driver_protocol.proto](#rpc_driver_protocol-proto)
    - [RvDeviceEncoding](#rvpb-RvDeviceEncoding)
    - [RvDeviceInfo](#rvpb-RvDeviceInfo)
    - [RvDeviceList](#rvpb-RvDeviceList)
    - [RvDeviceSelector](#rvpb-RvDeviceSelector)
    - [RvDriverInfo](#rvpb-RvDriverInfo)
    - [RvEndpointInfo](#rvpb-RvEndpointInfo)
    - [RvEndpointRequest](#rvpb-RvEndpointRequest)
    - [RvLogEntry](#rvpb-RvLogEntry)
    - [RvNone](#rvpb-RvNone)
    - [RvPacketEncoding](#rvpb-RvPacketEncoding)
    - [RvReceiverConfig](#rvpb-RvReceiverConfig)
    - [RvSenderConfig](#rvpb-RvSenderConfig)
    - [RvToggleRequest](#rvpb-RvToggleRequest)
  
    - [RvChannelLayout](#rvpb-RvChannelLayout)
    - [RvDeviceType](#rvpb-RvDeviceType)
    - [RvFecEncoding](#rvpb-RvFecEncoding)
    - [RvInterface](#rvpb-RvInterface)
    - [RvLatencyTunerBackend](#rvpb-RvLatencyTunerBackend)
    - [RvLatencyTunerProfile](#rvpb-RvLatencyTunerProfile)
    - [RvLogEntry.Level](#rvpb-RvLogEntry-Level)
    - [RvResamplerBackend](#rvpb-RvResamplerBackend)
    - [RvResamplerProfile](#rvpb-RvResamplerProfile)
    - [RvSampleFormat](#rvpb-RvSampleFormat)
  
    - [RvDriver](#rvpb-RvDriver)
  
- [Scalar Value Types](#scalar-value-types)



<a name="rpc_driver_protocol-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## rpc/driver_protocol.proto



<a name="rvpb-RvDeviceEncoding"></a>

### RvDeviceEncoding
Local encoding of virtual device.
Defines how it is presented to apps.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| sample_rate | [uint32](#uint32) | optional | Virtual device sample rate, in Hertz (e.g. 44100). Keep unset to use default. |
| channel_layout | [RvChannelLayout](#rvpb-RvChannelLayout) | optional | Virtual device channel layout (e.g. stereo). Keep unset to use default. |
| buffer_length | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Virtual device buffer size. Keep unset to use default. |
| track_count    | [uint32](#uint32) | optional | Track count when channel_layout is RV_CHANNEL_LAYOUT_MULTITRACK (e.g. 9).                                                                                |







<a name="rvpb-RvDeviceInfo"></a>

### RvDeviceInfo
Virtual device info.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| type | [RvDeviceType](#rvpb-RvDeviceType) |  | Device type. Each virtual device can be either sender (output device) or receiver (input device). Required field. |
| index | [uint32](#uint32) | optional | Device index identifier. Index is a small numeric value that is reused for new devices after device deletion. When retrieving device info, always present and non-zero. When creating device, keep unset to automaitcally select free index. When set, should not be zero. |
| uid | [string](#string) | optional | Device UID identifier. UID is a long string identifier, unique across all audio devices, and very unlikely to be ever reused. When retrieving device info, always present and non-empty. When creating device, keep unset to generate random UID. When set, should not be empty. |
| name | [string](#string) | optional | Human-readable device name. Device name is shown to the user in UI. When retrieving device info, always present and non-empty. When creating device, keep unset to generate name automatically. When set, should not be empty. |
| enabled | [bool](#bool) | optional | Whether device is active and visible to the user. Device can be disabled to remove it from system without losing configuration, and then re-enabled later. When retrieving device info, always present and non-empty. When creating device, if this field is unset or set to true, devices is enabled emmediately, and if it&#39;s set to false, it&#39;s created disabled. |
| device_encoding | [RvDeviceEncoding](#rvpb-RvDeviceEncoding) |  | Local encoding of device. Parameters of virtual device, as it&#39;s shown to the apps. |
| sender_config | [RvSenderConfig](#rvpb-RvSenderConfig) |  | Configuration for sender device. Should be used if device type is RV_DEVICE_TYPE_SENDER. |
| receiver_config | [RvReceiverConfig](#rvpb-RvReceiverConfig) |  | Configuration for receiver device. Should be used if device type is RV_DEVICE_TYPE_RECEIVER. |
| local_endpoints | [RvEndpointInfo](#rvpb-RvEndpointInfo) | repeated | List of local endpoints on which device is receiving traffic or control requests. Local endpoints can be added intially via add_device() or on fly via bind(). |
| remote_endpoints | [RvEndpointInfo](#rvpb-RvEndpointInfo) | repeated | List of remote endpoints to which device is sending traffic or control requests. Remote endpoints can be added intially via add_device() or on fly via connect(). |






<a name="rvpb-RvDeviceList"></a>

### RvDeviceList
Virtual device list.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| devices | [RvDeviceInfo](#rvpb-RvDeviceInfo) | repeated |  |






<a name="rvpb-RvDeviceSelector"></a>

### RvDeviceSelector
Virtual device selector.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| index | [uint32](#uint32) |  | Select device by index. |
| uid | [string](#string) |  | Select device by UID. |






<a name="rvpb-RvDriverInfo"></a>

### RvDriverInfo
Info about driver.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| version | [string](#string) |  | Driver version (comes from git tag). |
| commit | [string](#string) |  | Driver commit hash (comes from git commit). |






<a name="rvpb-RvEndpointInfo"></a>

### RvEndpointInfo
Endpoint description.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| slot | [uint32](#uint32) | optional | Slot to which this endpoint belongs. Slot is a group of related endpoints, like complementary transport and control stream pair connecting two peers. Multiple slots can be used to connect a sender to a few remote addresses, or to bind a receiver to a few different local addresses. When an endpoint is bound or connected, specified slot is created automatically if it does not exist yet. When retrieving endpoint info, always present. When sending endpoint info, keep unset to use default slot (0). |
| interface | [RvInterface](#rvpb-RvInterface) |  | Interface to which this endpoint attached. Interface defines type of data transfered via endpoint, and list of allowed endpoint protocols (URI schemas). Each slot can have up to one endpoint of every interface type, e.g. one source endpoint and one control endpoint. |
| uri | [string](#string) |  | URI is address associated with endpoint. For local endpoint, URI defines address to which endpoint is bound. For remote endpoint, URI defines address to which endpoint is connected. Allowed URI schemas are defined by endpoint interface. |






<a name="rvpb-RvEndpointRequest"></a>

### RvEndpointRequest
Endpoint bind or connect request.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| device | [RvDeviceSelector](#rvpb-RvDeviceSelector) |  | Virtual device to which request is send. |
| endpoint | [RvEndpointInfo](#rvpb-RvEndpointInfo) |  | New local or remote endpoint to be bound or connected. |






<a name="rvpb-RvLogEntry"></a>

### RvLogEntry
Driver log message.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| time | [google.protobuf.Timestamp](#google-protobuf-Timestamp) |  | Absolute time when message was generated. |
| level | [RvLogEntry.Level](#rvpb-RvLogEntry-Level) |  | Message level. |
| text | [string](#string) |  | Formatted message text (without time and level). |






<a name="rvpb-RvNone"></a>

### RvNone
No data.






<a name="rvpb-RvPacketEncoding"></a>

### RvPacketEncoding
Network packet encoding.
Defines how samples are encoded when sent over network.


| Field          | Type | Label | Description                                                                                                                  |
|----------------| --- | ----- |------------------------------------------------------------------------------------------------------------------------------|
| encoding_id    | [uint32](#uint32) |  | Encoding identifier (arbitrary 8-bit number). You should use the same numbers on sender and receiver to identifiy encodings. |
| sample_rate    | [uint32](#uint32) |  | Sample rate, in Hertz (e.g. 44100).                                                                                          |
| sample_format  | [RvSampleFormat](#rvpb-RvSampleFormat) |  | Sample format (e.g. 16-bit PCM).                                                                                             |
| channel_layout | [RvChannelLayout](#rvpb-RvChannelLayout) |  | Channel layout (e.g. stereo).                                                                                                |
| track_count    | [uint32](#uint32) | optional | Track count when channel_layout is RV_CHANNEL_LAYOUT_MULTITRACK (e.g. 9).                                                                                |






<a name="rvpb-RvReceiverConfig"></a>

### RvReceiverConfig
Parameters of receiver device.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| packet_encodings | [RvPacketEncoding](#rvpb-RvPacketEncoding) | repeated | Non-standard encodings for network packets. If your senders use non-standard packet encodings, you should also list them on receiver using the same encoding identifiers. Multiple encodings can be specified for the case when different senders use different custom encodings. |
| latency_tuner_backend | [RvLatencyTunerBackend](#rvpb-RvLatencyTunerBackend) | optional | Latency tuning algorithm. Keep unset to use default. |
| latency_tuner_profile | [RvLatencyTunerProfile](#rvpb-RvLatencyTunerProfile) | optional | Latency tuning profile. Keep unset to use default. |
| resampler_backend | [RvResamplerBackend](#rvpb-RvResamplerBackend) | optional | Resampling algorithm. Keep unset to use default. |
| resampler_profile | [RvResamplerProfile](#rvpb-RvResamplerProfile) | optional | Resampling quality. Keep unset to use default. |
| target_latency | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Target latency. Keep unset to use default. |
| latency_tolerance | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Maximum deviation of latency from target. Keep unset to disable. |
| no_playback_timeout | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Timeout for the lack of playback. Keep unset to use default. |
| choppy_playback_timeout | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Timeout for choppy playback. Keep unset to use default. |






<a name="rvpb-RvSenderConfig"></a>

### RvSenderConfig
Parameters of sender device.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| packet_encoding | [RvPacketEncoding](#rvpb-RvPacketEncoding) | optional | Non-standard encoding for network packets. If you use non-default device encoding (e.g. change sample rate), you should also explicitly provide packet encoding on both sender and receiver, using the same encoding id. |
| packet_length | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Duration of a single packet. Keep unset to use default. |
| packet_interleaving | [bool](#bool) | optional | Enable packet interleaving. Keep unset to use default. |
| fec_encoding | [RvFecEncoding](#rvpb-RvFecEncoding) | optional | Forward Error Correction encoding. Keep unset to use default. |
| fec_block_source_packets | [uint32](#uint32) | optional | Number of source packets per FEC block. Keep unset to use default. |
| fec_block_repair_packets | [uint32](#uint32) | optional | Number of repair packets per FEC block. Keep unset to use default. |
| latency_tuner_backend | [RvLatencyTunerBackend](#rvpb-RvLatencyTunerBackend) | optional | Latency tuning algorithm. Use only if you need sender-side latency tuning. Keep unset to disable. |
| latency_tuner_profile | [RvLatencyTunerProfile](#rvpb-RvLatencyTunerProfile) | optional | Latency tuning profile. Use only if you need sender-side latency tuning. Keep unset to disable. |
| resampler_backend | [RvResamplerBackend](#rvpb-RvResamplerBackend) | optional | Resampling algorithm. Keep unset to use default. |
| resampler_profile | [RvResamplerProfile](#rvpb-RvResamplerProfile) | optional | Resampling quality. Keep unset to use default. |
| target_latency | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Target latency. Use only if you need sender-side latency tuning. Keep unset to disable. |
| latency_tolerance | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Maximum deviation of latency from target. Use only if you need sender-side latency tuning. Keep unset to disable. |






<a name="rvpb-RvToggleRequest"></a>

### RvToggleRequest
Device enable/disable request.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| device | [RvDeviceSelector](#rvpb-RvDeviceSelector) |  | Virtual device to which request is send. |
| enabled | [bool](#bool) |  | Should device be enabled or disabled. |





 


<a name="rvpb-RvChannelLayout"></a>

### RvChannelLayout
Channel layout.
Defines what channel count and their meaning.

| Name                         | Number | Description                               |
|------------------------------|--------|-------------------------------------------|
| RV_CHANNEL_LAYOUT_MONO       | 0      | One monochromatic channel.                |
| RV_CHANNEL_LAYOUT_STEREO     | 1      | Two channels: left and right.             |
| RV_CHANNEL_LAYOUT_MULTITRACK | 2      | Multi-track audio with up to 1024 tracks. |



<a name="rvpb-RvDeviceType"></a>

### RvDeviceType
Device type.
Defines whether device will be input (receiver) or output (sender).

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_DEVICE_TYPE_SENDER | 0 | Output device that sends sound to remote receiver. |
| RV_DEVICE_TYPE_RECEIVER | 1 | Input device that receives sound from remote sender. |



<a name="rvpb-RvFecEncoding"></a>

### RvFecEncoding
Forward Error Correction encoding.
Defines method for repairing lost packets to improve quality on
unreliable networks.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_FEC_ENCODING_DISABLE | 0 | No FEC encoding. |
| RV_FEC_ENCODING_DEFAULT | 1 | Default FEC encoding. |
| RV_FEC_ENCODING_RS8M | 2 | Reed-Solomon FEC encoding (RFC 6865) with m=8. Good for small block sizes (below 256 packets). |
| RV_FEC_ENCODING_LDPC_STAIRCASE | 3 | LDPC-Staircase FEC encoding (RFC 6816). Good for large block sizes (above 1024 packets). |



<a name="rvpb-RvInterface"></a>

### RvInterface
Endpoint interface type.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_INTERFACE_CONSOLIDATED | 0 | Interface that consolidates all types of streams. |
| RV_INTERFACE_AUDIO_SOURCE | 1 | Interface for audio stream source data. |
| RV_INTERFACE_AUDIO_REPAIR | 2 | Interface for audio stream repair data. |
| RV_INTERFACE_AUDIO_CONTROL | 3 | Interface for audio control messages. |



<a name="rvpb-RvLatencyTunerBackend"></a>

### RvLatencyTunerBackend
Latency tuner backend.
Defines which latency is monitored and adjusted.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_LATENCY_TUNER_BACKEND_DEFAULT | 0 | Select best backend automatically. |
| RV_LATENCY_TUNER_BACKEND_NIQ | 2 | Latency tuning based on network incoming queue length. |



<a name="rvpb-RvLatencyTunerProfile"></a>

### RvLatencyTunerProfile
Latency tuner profile.
Defines whether latency tuning is enabled and which algorithm is used.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_LATENCY_TUNER_PROFILE_DEFAULT | 0 | Select best profile automatically. |
| RV_LATENCY_TUNER_PROFILE_INTACT | 1 | No latency tuning. Expect regular underruns or overruns. |
| RV_LATENCY_TUNER_PROFILE_RESPONSIVE | 2 | Responsive latency tuning. Clock speed is adjusted quickly and accurately. |
| RV_LATENCY_TUNER_PROFILE_GRADUAL | 3 | Gradual latency tuning. Clock speed is adjusted slowly and smoothly. |



<a name="rvpb-RvLogEntry-Level"></a>

### RvLogEntry.Level


| Name | Number | Description |
| ---- | ------ | ----------- |
| CRIT | 0 |  |
| ERROR | 1 |  |
| WARN | 2 |  |
| INFO | 3 |  |
| DEBUG | 4 |  |
| TRACE | 5 |  |



<a name="rvpb-RvResamplerBackend"></a>

### RvResamplerBackend
Resampler backend.
Affects speed and quality.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_RESAMPLER_BACKEND_DEFAULT | 0 | Select best backend automatically. |
| RV_RESAMPLER_BACKEND_BUILTIN | 1 | CPU-demanding good-quality built-in resampler. Works fine with any latency. |
| RV_RESAMPLER_BACKEND_SPEEX | 2 | Fast good-quality resampler from SpeexDSP. Works fine with higher latency. |
| RV_RESAMPLER_BACKEND_SPEEXDEC | 3 | Lower quiality fast resampler combining SpeexDSP and decimation. Works fine with any latency. |



<a name="rvpb-RvResamplerProfile"></a>

### RvResamplerProfile
Resampler profile.
Affects speed and quality.
Each resampler backend treats profile in its own way.

| Name | Number | Description |
| ---- | ------ | ----------- |
| RV_RESAMPLER_PROFILE_DEFAULT | 0 | Select default profile. |
| RV_RESAMPLER_PROFILE_HIGH | 1 | High quality, low speed. |
| RV_RESAMPLER_PROFILE_MEDIUM | 2 | Medium quality, medium speed. |
| RV_RESAMPLER_PROFILE_LOW | 3 | Low quality, high speed. |



<a name="rvpb-RvSampleFormat"></a>

### RvSampleFormat
Sample format.
Defines how audio samples are encoded.

| Name | Number | Description |
| ---- | ------ | ----------- |
| ROC_FORMAT_PCM_SINT16 | 0 | Uncompressed PCM, 16-bit signed integers, network endian. |


 

 


<a name="rvpb-RvDriver"></a>

### RvDriver
RPC interface for Roc Virtual Audio Device driver.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| ping | [RvNone](#rvpb-RvNone) | [RvNone](#rvpb-RvNone) | Check driver presence. This command does nothing and just returns success. |
| driver_info | [RvNone](#rvpb-RvNone) | [RvDriverInfo](#rvpb-RvDriverInfo) | Get driver info. |
| stream_logs | [RvNone](#rvpb-RvNone) | [RvLogEntry](#rvpb-RvLogEntry) stream | Stream driver logs to client. Logs are duplicated to all clients that want to stream them. Logs are also duplicated to syslog. |
| get_all_devices | [RvNone](#rvpb-RvNone) | [RvDeviceList](#rvpb-RvDeviceList) | Get info for all virtual devices. |
| get_device | [RvDeviceSelector](#rvpb-RvDeviceSelector) | [RvDeviceInfo](#rvpb-RvDeviceInfo) | Get info for one virtual device. Device can be selected by index or UID. |
| add_device | [RvDeviceInfo](#rvpb-RvDeviceInfo) | [RvDeviceInfo](#rvpb-RvDeviceInfo) | Create new virtual device. Returns updated device info with all fields set. |
| delete_device | [RvDeviceSelector](#rvpb-RvDeviceSelector) | [RvNone](#rvpb-RvNone) | Delete virtual device. Device can be selected by index or UID. |
| toggle_device | [RvToggleRequest](#rvpb-RvToggleRequest) | [RvNone](#rvpb-RvNone) | Enable or disable device. Device can be selected by index or UID. |
| bind | [RvEndpointRequest](#rvpb-RvEndpointRequest) | [RvEndpointInfo](#rvpb-RvEndpointInfo) | Bind device to local endpoint. |
| connect | [RvEndpointRequest](#rvpb-RvEndpointRequest) | [RvEndpointInfo](#rvpb-RvEndpointInfo) | Connect device to remote endpoint. |

 



## Scalar Value Types

| .proto Type | Notes | C++ | Java | Python | Go | C# | PHP | Ruby |
| ----------- | ----- | --- | ---- | ------ | -- | -- | --- | ---- |
| <a name="double" /> double |  | double | double | float | float64 | double | float | Float |
| <a name="float" /> float |  | float | float | float | float32 | float | float | Float |
| <a name="int32" /> int32 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint32 instead. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="int64" /> int64 | Uses variable-length encoding. Inefficient for encoding negative numbers – if your field is likely to have negative values, use sint64 instead. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="uint32" /> uint32 | Uses variable-length encoding. | uint32 | int | int/long | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="uint64" /> uint64 | Uses variable-length encoding. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum or Fixnum (as required) |
| <a name="sint32" /> sint32 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int32s. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sint64" /> sint64 | Uses variable-length encoding. Signed int value. These more efficiently encode negative numbers than regular int64s. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="fixed32" /> fixed32 | Always four bytes. More efficient than uint32 if values are often greater than 2^28. | uint32 | int | int | uint32 | uint | integer | Bignum or Fixnum (as required) |
| <a name="fixed64" /> fixed64 | Always eight bytes. More efficient than uint64 if values are often greater than 2^56. | uint64 | long | int/long | uint64 | ulong | integer/string | Bignum |
| <a name="sfixed32" /> sfixed32 | Always four bytes. | int32 | int | int | int32 | int | integer | Bignum or Fixnum (as required) |
| <a name="sfixed64" /> sfixed64 | Always eight bytes. | int64 | long | int/long | int64 | long | integer/string | Bignum |
| <a name="bool" /> bool |  | bool | boolean | boolean | bool | bool | boolean | TrueClass/FalseClass |
| <a name="string" /> string | A string must always contain UTF-8 encoded or 7-bit ASCII text. | string | String | str/unicode | string | string | string | String (UTF-8) |
| <a name="bytes" /> bytes | May contain any arbitrary sequence of bytes. | string | ByteString | str | []byte | ByteString | string | String (ASCII-8BIT) |

