![BoomRTX logo](../graphics/logo_small_black.png)
# Communication Protocol
## Framing
BoomRTX uses framing to transmit data between nodes.
The structure of a frame is as follow:
<dl>
<table>
  <tr>
    <th>START_SYMBOL (8 bits)</th>
    <th>FLAGS (8 bits)</th>
    <th>OPCODE (8 bits)</th>
    <th>PAYLOAD SIZE (8 bit)</th>
    <th>PAYLOAD (Variable Size, Max=255bytes)</th>
    <th>Checksum<br>(16 bits)</th>
    <th>END_SYMBOL (8 bits)</th>
  </tr>
  <tr>
    <td>0xBE</td>
    <td>Can be used to specify<br>frame encoding/encryption options<br>and stuff</td>
    <td>The kind of operation<br>the frame encodes<br>(please refer to the "Packets" chapter<br>of this document)</td>
    <td>Size in bytes of the<br>payload. So, indeed, maximum payload size is 255.</td>
    <td>Payload content</td>
    <td>CCITT CRC 16 (poly: 0x1021, start=0x00)<br>Computed on the following elements:<br>OPCODE, PAYLOAD_SIZE, PAYLOAD</td>
    <td>0xEF</td>
  </tr>
</table>
</dl>

## Packets
BoomRTX uses several different packets, identified by OpCodes and using the frame structure which has already been documented. OpCodes are all declared in the protocol.h file. Here is an exhaustive list, and the description of the corresponding packet:
* **OP_PING**: Sent in order to query alive devices.  Payload for this OP is:
> [ (0 bits) ]

* **OP_PONG**: Should be sent in response to OP_PING.  Payload for this OP is:
> [ device_id (8 bits) ][ device_type (8 bits) ]

* **OP_SET_DEVICE_ID**: Used through configuration serial link. The device receiving this frame should set its device ID accordingly.  Payload for this OP is:
> [ new_device_id (8 bits) ]

* **OP_GET_RADIO_CHANNEL**: Used to query the current selected radio channel on the target device.  Payload for this OP is:
> [ target_device_id (8 bits) ]

* **OP_SET_RADIO_CHANNEL**: Used through configuration serial link. The device receiving this frame should set its device ID accordingly.  Payload for this OP is:
> [ target_device_id (8 bits) ][ channel_id (8 bits) ]

* **OP_RADIO_CHANNEL_ACK**: Used through configuration serial link. Used to inform other devices which radio channel we're using, or that we succesfully changed our radio channel (after receiving **OP_SET_RADIO_CHANNEL**). Payload for this OP is:
> [ device_id (8 bits) ][ channel_id (8 bits) ]

* **OP_GET_RADIO_POWER**: Used to query the current selected radio power on the target device. Payload for this OP is:
> [ target_device_id (8 bits) ]

* **OP_SET_RADIO_POWER**: Used through configuration serial link. The device receiving this frame should set its device ID accordingly.  Payload for this OP is:
> [ target_device_id (8 bits) ][ channel_id (8 bits) ]

* **OP_RADIO_POWER_ACK**: Used through configuration serial link. Used to inform other devices which radio power level we're using, or that we succesfully changed our radio power (after receiving **OP_SET_RADIO_POWER**).  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

* **OP_GET_RADIO_QUALITY**: Used through configuration serial link of a TRANSMITTER.  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

* **OP_RADIO_QUALITY_ACK**: Used through configuration serial link of a TRANSMITTER.  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

* **OP_GET_TARGET_ID**: Used through configuration serial link of a receiver.  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

* **OP_SET_TARGET_ID**: Used through configuration serial link of a receiver.  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

* **OP_TARGET_ID_ACK**: Used through configuration serial link of a receiver.  Payload for this OP is:
> [ device_id (8 bits) ][ power_level (8 bits) ]

## Examples
(Maybe I should use [mscgen](http://www.mcternan.me.uk/mscgen/index.html))

### Transmitter PINGing Receiver
