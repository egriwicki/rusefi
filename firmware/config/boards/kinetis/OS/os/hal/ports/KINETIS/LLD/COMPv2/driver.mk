ifeq ($(KINETIS_CONTRIB),)
  KINETIS_CONTRIB = $(CHIBIOS_CONTRIB)
endif

PLATFORMSRC_CONTRIB += ${KINETIS_CONTRIB}/os/hal/ports/KINETIS/LLD/COMPv2/hal_comp_lld.c

PLATFORMINC_CONTRIB += ${KINETIS_CONTRIB}/os/hal/ports/KINETIS/LLD/COMPv2
