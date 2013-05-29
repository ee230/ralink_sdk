deps_config := \
	extra/Configs/Config.in.arch \
	extra/Configs/Config.x86_64 \
	extra/Configs/Config.v850 \
	extra/Configs/Config.sparc \
	extra/Configs/Config.sh64 \
	extra/Configs/Config.sh \
	extra/Configs/Config.powerpc \
	extra/Configs/Config.mips \
	extra/Configs/Config.microblaze \
	extra/Configs/Config.nios2 \
	extra/Configs/Config.nios \
	extra/Configs/Config.m68k \
	extra/Configs/Config.i960 \
	extra/Configs/Config.i386 \
	extra/Configs/Config.h8300 \
	extra/Configs/Config.frv \
	extra/Configs/Config.e1 \
	extra/Configs/Config.cris \
	extra/Configs/Config.bfin \
	extra/Configs/Config.arm \
	extra/Configs/Config.alpha \
	extra/Configs/Config.in

.config include/linux/autoconf.h: $(deps_config)

$(deps_config):
