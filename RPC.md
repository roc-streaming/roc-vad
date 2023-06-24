# Protocol Documentation
<a name="top"></a>

## Table of Contents

- [rpc/driver_protocol.proto](#rpc_driver_protocol-proto)
    - [PrDeviceInfo](#rocvad-PrDeviceInfo)
    - [PrDeviceList](#rocvad-PrDeviceList)
    - [PrDeviceSelector](#rocvad-PrDeviceSelector)
    - [PrDriverInfo](#rocvad-PrDriverInfo)
    - [PrEndpointInfo](#rocvad-PrEndpointInfo)
    - [PrEndpointRequest](#rocvad-PrEndpointRequest)
    - [PrLocalConfig](#rocvad-PrLocalConfig)
    - [PrLogEntry](#rocvad-PrLogEntry)
    - [PrNone](#rocvad-PrNone)
    - [PrReceiverConfig](#rocvad-PrReceiverConfig)
    - [PrSenderConfig](#rocvad-PrSenderConfig)
  
    - [PrChannelSet](#rocvad-PrChannelSet)
    - [PrDeviceType](#rocvad-PrDeviceType)
    - [PrFecEncoding](#rocvad-PrFecEncoding)
    - [PrInterface](#rocvad-PrInterface)
    - [PrLogEntry.Level](#rocvad-PrLogEntry-Level)
    - [PrPacketEncoding](#rocvad-PrPacketEncoding)
    - [PrResamplerBackend](#rocvad-PrResamplerBackend)
    - [PrResamplerProfile](#rocvad-PrResamplerProfile)
  
    - [DriverProtocol](#rocvad-DriverProtocol)
  
- [Scalar Value Types](#scalar-value-types)



<a name="rpc_driver_protocol-proto"></a>
<p align="right"><a href="#top">Top</a></p>

## rpc/driver_protocol.proto



<a name="rocvad-PrDeviceInfo"></a>

### PrDeviceInfo
Virtual device info.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| type | [PrDeviceType](#rocvad-PrDeviceType) |  | Device type. Each virtual device can be either sender (output device) or receiver (input device). Required field. |
| index | [uint32](#uint32) | optional | Device index identifier. Index is a small numeric value that is reused for new devices after device deletion. When retrieving device info, always present and non-zero. When creating device, keep unset to automaitcally select free index. When set, should not be zero. |
| uid | [string](#string) | optional | Device UID identifier. UID is a long string identifier, unique across all audio devices, and very unlikely to be ever reused. When retrieving device info, always present and non-empty. When creating device, keep unset to generate random UID. When set, should not be empty. |
| name | [string](#string) | optional | Human-readable device name. Device name is shown to the user in UI. When retrieving device info, always present and non-empty. When creating device, keep unset to generate name automatically. When set, should not be empty. |
| local_config | [PrLocalConfig](#rocvad-PrLocalConfig) |  | Local configuration of device. Parameters of virtual device, as it&#39;s shown to the local apps. |
| sender_config | [PrSenderConfig](#rocvad-PrSenderConfig) |  | Configuration for sender device. Should be used if device type is PR_DEVICE_TYPE_SENDER. |
| receiver_config | [PrReceiverConfig](#rocvad-PrReceiverConfig) |  | Configuration for receiver device. Should be used if device type is PR_DEVICE_TYPE_RECEIVER. |
| local_endpoints | [PrEndpointInfo](#rocvad-PrEndpointInfo) | repeated | List of local endpoints on which device is receiving traffic or control requests. Local endpoints can be added intially via add_device() or on fly via bind(). |
| remote_endpoints | [PrEndpointInfo](#rocvad-PrEndpointInfo) | repeated | List of remote endpoints to which device is sending traffic or control requests. Remote endpoints can be added intially via add_device() or on fly via connect(). |






<a name="rocvad-PrDeviceList"></a>

### PrDeviceList
Virtual device list.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| devices | [PrDeviceInfo](#rocvad-PrDeviceInfo) | repeated |  |






<a name="rocvad-PrDeviceSelector"></a>

### PrDeviceSelector
Virtual device selector.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| index | [uint32](#uint32) |  | Select device by index. |
| uid | [string](#string) |  | Select device by UID. |






<a name="rocvad-PrDriverInfo"></a>

### PrDriverInfo
Info about driver.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| version | [string](#string) |  | Driver version (comes from git tag). |
| commit | [string](#string) |  | Driver commit hash (comes from git commit). |






<a name="rocvad-PrEndpointInfo"></a>

### PrEndpointInfo
Endpoint description.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| slot | [uint32](#uint32) | optional | Slot to which this endpoint belongs. Slot is a group of related endpoints, like complementary transport and control stream pair connecting two peers. Multiple slots can be used to connect a sender to a few remote addresses, or to bind a receiver to a few different local addresses. When an endpoint is bound or connected, specified slot is created automatically if it does not exist yet. When retrieving endpoint info, always present. When sending endpoint info, keep unset to use default slot (0). |
| interface | [PrInterface](#rocvad-PrInterface) |  | Interface to which this endpoint attached. Interface defines type of data transfered via endpoint, and list of allowed endpoint protocols (URI schemas). Each slot can have up to one endpoint of every interface type, e.g. one source endpoint and one control endpoint. |
| uri | [string](#string) |  | URI is address associated with endpoint. For local endpoint, URI defines address to which endpoint is bound. For remote endpoint, URI defines address to which endpoint is connected. Allowed URI schemas are defined by endpoint interface. |






<a name="rocvad-PrEndpointRequest"></a>

### PrEndpointRequest
Endpoint bind or connect request.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| device | [PrDeviceSelector](#rocvad-PrDeviceSelector) |  | Virtual device to which request is send. |
| endpoint | [PrEndpointInfo](#rocvad-PrEndpointInfo) |  | New local or remote endpoint to be bound or connected. |






<a name="rocvad-PrLocalConfig"></a>

### PrLocalConfig
Local parameters of virtual device.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| sample_rate | [uint32](#uint32) | optional | Virtual device sample rate, in Hertz (e.g. 44100). When retrieving device info, always present and non-zero. When creating device, keep unset to use default (44100). |
| channel_set | [PrChannelSet](#rocvad-PrChannelSet) | optional | Virtual device channel set (e.g. stereo). When retrieving device info, always present. When creating device, keep unset to use default (PR_CHANNEL_SET_STEREO). |






<a name="rocvad-PrLogEntry"></a>

### PrLogEntry
Driver log message.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| time | [google.protobuf.Timestamp](#google-protobuf-Timestamp) |  | Absolute time when message was generated. |
| level | [PrLogEntry.Level](#rocvad-PrLogEntry-Level) |  | Message level. |
| text | [string](#string) |  | Formatted message text (without time and level). |






<a name="rocvad-PrNone"></a>

### PrNone
No data.






<a name="rocvad-PrReceiverConfig"></a>

### PrReceiverConfig
Parameters of receiver device.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| target_latency | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Target session latency. When retrieving device info, always present. When creating device, keep unset to use default (100ms). When set, should not be zero. |
| resampler_backend | [PrResamplerBackend](#rocvad-PrResamplerBackend) | optional | Resampling engine. When retrieving device info, always present. When creating device, keep unset to use default (PR_RESAMPLER_BACKEND_SPEEX). |
| resampler_profile | [PrResamplerProfile](#rocvad-PrResamplerProfile) | optional | Resampling quality. When retrieving device info, always present. When creating device, keep unset to use default (PR_RESAMPLER_PROFILE_MEDIUM). |






<a name="rocvad-PrSenderConfig"></a>

### PrSenderConfig
Parameters of sender device.


| Field | Type | Label | Description |
| ----- | ---- | ----- | ----------- |
| packet_encoding | [PrPacketEncoding](#rocvad-PrPacketEncoding) | optional | Encoding for network packets. When retrieving device info, always present. When creating device, keep unset to use default (PR_PACKET_ENCODING_AVP_L16). |
| packet_length | [google.protobuf.Duration](#google-protobuf-Duration) | optional | Duration of a single packet. When retrieving device info, always present. When creating device, keep unset to use default (7ms). When set, should not be zero. |
| fec_encoding | [PrFecEncoding](#rocvad-PrFecEncoding) | optional | Forward Error Correction encoding. When retrieving device info, always present. When creating device, keep unset to use default (PR_FEC_ENCODING_RS8M). |
| fec_block_source_packets | [uint32](#uint32) | optional | Number of source packets per FEC block. When retrieving device info, always present. When creating device, keep unset to use default (20). When set, should not be zero. |
| fec_block_repair_packets | [uint32](#uint32) | optional | Number of repair packets per FEC block. When retrieving device info, always present. When creating device, keep unset to use default (10). When set, should not be zero. |





 


<a name="rocvad-PrChannelSet"></a>

### PrChannelSet
Device channel set.
Defines what channels will be shown to apps that use the device.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_CHANNEL_SET_STEREO | 0 | Two channels: left and right. |



<a name="rocvad-PrDeviceType"></a>

### PrDeviceType
Device type.
Defines whether device will be input or output.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_DEVICE_TYPE_SENDER | 0 | Output device that sends sound to remote receiver. |
| PR_DEVICE_TYPE_RECEIVER | 1 | Input device that receives sound from remote sender. |



<a name="rocvad-PrFecEncoding"></a>

### PrFecEncoding
Forward Error Correction encoding.
Defines method for repairing lost packets to improve quality on
unreliable networks.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_FEC_ENCODING_DISABLE | 0 | No FEC encoding. |
| PR_FEC_ENCODING_RS8M | 1 | Reed-Solomon FEC encoding (RFC 6865) with m=8. Good for small block sizes (below 256 packets). |
| PR_FEC_ENCODING_LDPC_STAIRCASE | 2 | LDPC-Staircase FEC encoding (RFC 6816). Good for large block sizes (above 1024 packets). |



<a name="rocvad-PrInterface"></a>

### PrInterface
Endpoint interface type.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_INTERFACE_CONSOLIDATED | 0 | Interface that consolidates all types of streams. |
| PR_INTERFACE_AUDIO_SOURCE | 1 | Interface for audio stream source data. |
| PR_INTERFACE_AUDIO_REPAIR | 2 | Interface for audio stream repair data. |
| PR_INTERFACE_AUDIO_CONTROL | 3 | Interface for audio control messages. |



<a name="rocvad-PrLogEntry-Level"></a>

### PrLogEntry.Level


| Name | Number | Description |
| ---- | ------ | ----------- |
| CRIT | 0 |  |
| ERROR | 1 |  |
| WARN | 2 |  |
| INFO | 3 |  |
| DEBUG | 4 |  |
| TRACE | 5 |  |



<a name="rocvad-PrPacketEncoding"></a>

### PrPacketEncoding
Network packet encoding.
Defines how samples are encoded when sent over network.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_PACKET_ENCODING_AVP_L16 | 0 | PCM signed 16-bit. &#34;L16&#34; encoding from RTP A/V Profile (RFC 3551). Uncompressed samples coded as interleaved 16-bit signed big-endian integers in two&#39;s complement notation. |



<a name="rocvad-PrResamplerBackend"></a>

### PrResamplerBackend
Resampler backend.
Affects speed and quality.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_RESAMPLER_BACKEND_BUILTIN | 0 | Slow built-in resampler. |
| PR_RESAMPLER_BACKEND_SPEEX | 1 | Fast good-quality resampler from SpeexDSP. |



<a name="rocvad-PrResamplerProfile"></a>

### PrResamplerProfile
Resampler profile.
Affects speed and quality.
Each resampler backend treats profile in its own way.

| Name | Number | Description |
| ---- | ------ | ----------- |
| PR_RESAMPLER_PROFILE_DISABLE | 0 | Do not perform resampling. Clock drift compensation will be disabled in this case. If in doubt, do not disable resampling. |
| PR_RESAMPLER_PROFILE_HIGH | 1 | High quality, low speed. |
| PR_RESAMPLER_PROFILE_MEDIUM | 2 | Medium quality, medium speed. |
| PR_RESAMPLER_PROFILE_LOW | 3 | Low quality, high speed. |


 

 


<a name="rocvad-DriverProtocol"></a>

### DriverProtocol
RPC interface for Roc Virtual Audio Device driver.

| Method Name | Request Type | Response Type | Description |
| ----------- | ------------ | ------------- | ------------|
| ping | [PrNone](#rocvad-PrNone) | [PrNone](#rocvad-PrNone) | Check driver presence. This command does nothing and just returns success. |
| driver_info | [PrNone](#rocvad-PrNone) | [PrDriverInfo](#rocvad-PrDriverInfo) | Get driver info. |
| stream_logs | [PrNone](#rocvad-PrNone) | [PrLogEntry](#rocvad-PrLogEntry) stream | Stream driver logs to client. Logs are duplicated to all clients that want to stream them. Logs are also duplicated to syslog. |
| get_all_devices | [PrNone](#rocvad-PrNone) | [PrDeviceList](#rocvad-PrDeviceList) | Get info for all virtual devices. |
| get_device | [PrDeviceSelector](#rocvad-PrDeviceSelector) | [PrDeviceInfo](#rocvad-PrDeviceInfo) | Get info for one virtual device. Device can be selected by index or UID. |
| add_device | [PrDeviceInfo](#rocvad-PrDeviceInfo) | [PrDeviceInfo](#rocvad-PrDeviceInfo) | Create new virtual device. Returns updated device info with all fields set. |
| delete_device | [PrDeviceSelector](#rocvad-PrDeviceSelector) | [PrNone](#rocvad-PrNone) | Delete virtual device. Device can be selected by index or UID. |
| bind | [PrEndpointRequest](#rocvad-PrEndpointRequest) | [PrEndpointInfo](#rocvad-PrEndpointInfo) | Bind device to local endpoint. |
| connect | [PrEndpointRequest](#rocvad-PrEndpointRequest) | [PrEndpointInfo](#rocvad-PrEndpointInfo) | Connect device to remote endpoint. |

 



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

