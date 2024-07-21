# TestTuya
Experiments with a tuya ir/rf remote control  [CBU and SH4]

See [this forum topic](https://www.elektroda.com/rtvforum/topic3975921.html)

I'm trying to adapt the [official tuya driver](https://github.com/tuya/tuya-bsp-gpl-public-components/tree/main/driver_src_tuya/ty_driver/src/subg) to the cbu module using [libretiny](https://docs.libretiny.eu/).

It doesn't work yet (it seems I can initialize the SH4 module, I can put it in receive mode, see the state transitions but never see any data in the fifo).

