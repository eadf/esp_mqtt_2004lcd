# esp8266_tachometer
A simple tachometer implementation. This used to be a sub-module in [another project](https://github.com/eadf/esp8266_digoleserial).

It works by setting up a periodic timer and an interrupt. 
The interrupt increases a counter each time the GPIO alters state. 
The timer calculates the number of alternations over time and presents the frequency [Hz] to the 'clients'.

###Public API

Setup the GPIO,interrupt and timer:

```void tachometer_init(uint8_t ioPin);```

Returns the frequency from the last timer period:

```uint32_t tachometer_getSample(void);```

###Example

See [esp8266_digoleserial demo.](https://github.com/eadf/esp8266_digoleserial)
