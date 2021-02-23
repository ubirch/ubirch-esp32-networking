set(COMPONENT_SRCS
        networking_wifi.c
        #networking_eth.c
        sntp_time.c
        )
set(COMPONENT_ADD_INCLUDEDIRS ".")
#set(COMPONENT_PRIV_REQUIRES ethernet)
register_component()